// Generates a recipe, a number of resources and runs various benchmarks
// Like test_models.cpp, but with bi-directional routes across the resources
// Size of recipe and resources can be small, medium or big
// Benchmarks include global and local best controller synthesis

#include <benchmark/benchmark.h>
#include "pcs/controller/controller.h"

#include <vector>
#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>

#include "pcs/topology/topology.h"
#include "lts/parsers/parsers.h"
#include "../memory_manager.h"

const static std::string machine_name_small = "small_test_model2";
const static std::string machine_name_medium = "medium_test_model2";
const static std::string machine_name_big = "big_test_model2";

static std::string machine_name = machine_name_small;
static std::string machine_dir = "../../data/" + machine_name;
static std::string resource_file_prefix = machine_dir + "/Resource";
static std::string recipe_dir = machine_dir + "/experiments/";
static std::string costs_file = machine_dir + "/costs.txt";

static const int num_resources = 16;

enum ModelSize {
	small,
	medium,
	big
};

static enum ModelSize model_size = ModelSize::small;

static bool resources_created = true;
static bool recipes_created = true;

static void CreateResource(int i) {
	const std::string filepath = resource_file_prefix + std::to_string(i) + ".txt";
	std::ofstream stream;
	stream.exceptions(std::ofstream::badbit);

	try {
		stream.open(filepath, std::ios::out | std::ios::trunc);

		stream << "s0\n";
		stream << "s0. nop. s0.\n";
		stream << "s1. nop. s1.\n";		

		if (i == 1) {
			if (model_size == ModelSize::small)
			{
				stream << "s0. o11. s1.\n";
				stream << "s0. o1. s1.\n";
				stream << "s1. out:1. s0.\n";
				stream << "s1. rem. s0.\n";
			}
			else if (model_size == ModelSize::medium || model_size == ModelSize::big)
			{
				stream << "s2. nop. s2.\n";
				stream << "s0. o11. s1.\n";
				stream << "s0. o1. s1.\n";
				stream << "s1. out:1. s0.\n";
				stream << "s1. rem. s0.\n";
				stream << "s1. o12. s2.\n";
				stream << "s1. o1a. s2.\n";
				stream << "s2. out:1. s0.\n";
				stream << "s2. rem. s0.\n";

				if (model_size == ModelSize::big)
				{
					stream << "s2. o13. s3.\n";
					stream << "s2. o1b. s3.\n";
					stream << "s3. out:1. s0.\n";
					stream << "s3. rem. s0.\n";
				}
			}
		} else {
			stream << "s0. o" + std::to_string(i) + "1. s1.\n";
			stream << "s0. in:" + std::to_string(i) + ". s1.\n";
			stream << "s1. out:" + std::to_string(i) + ". s0.\n";
			stream << "s1. rem. s0.\n";
			stream << "s1. o" + std::to_string(i) + "2. s2.\n";
			stream << "s1. o" + std::to_string(i) + ". s2.\n";
			
			if (i == num_resources - 3)
			{
				stream << "s1. o" + std::to_string(i + 2) + ". s2.\n";
			}
			
			stream << "s2. out:" + std::to_string(i) + ". s0.\n";
			stream << "s2. rem. s0.\n";

			if (model_size == ModelSize::medium || model_size == ModelSize::big)
			{
				stream << "s2. o" + std::to_string(i) + "3. s3.\n";
				stream << "s2. o" + std::to_string(i) + "a. s3.\n";
				stream << "s3. out:" + std::to_string(i) + ". s0.\n";
				stream << "s3. rem. s0.\n";

				if (model_size == ModelSize::big)
				{
					stream << "s3. o" + std::to_string(i) + "4. s4.\n";
					stream << "s3. o" + std::to_string(i) + "b. s4.\n";
					stream << "s4. out:" + std::to_string(i) + ". s0.\n";
					stream << "s4. rem. s0.\n";
				}
			}
		}
	}
	catch (const std::ofstream::failure& e) {
		throw;
	}
}

static void CreateTransportResource(int t, int n) {
	const std::string filepath = resource_file_prefix + std::to_string(t) + ".txt";
	std::ofstream stream;
	stream.exceptions(std::ofstream::badbit);

	try {
		stream.open(filepath, std::ios::out | std::ios::trunc);
		stream << "s0\n";
		stream << "s0. nop. s0.\n";

		for (int i = 1; i < n; i++) {
			stream << "s0. in:" + std::to_string(i) + ". s" + std::to_string(i) + ".\n";
			stream << "s" + std::to_string(i) + ". out:" + std::to_string(i + 1) + ". s0.\n";
			if (i == num_resources - 2)
			{
				stream << "s" + std::to_string(i) + ". out:" + std::to_string(i - 1) + ". s0.\n";
			}
		}
	}
	catch (const std::ofstream::failure& e) {
		throw;
	}
}

static void LoadResources(pcs::Environment& machine, int n, int transport) {
	resources_created = std::filesystem::is_directory(machine_dir);
	if (!resources_created) {
		std::filesystem::create_directory(machine_dir);

		for (int i = 1; i < num_resources; i++) {
			CreateResource(i);
		}

		CreateTransportResource(transport, num_resources);
		resources_created = true;
	}

	try {
		for (int i = 1; i < n; i++) {
			machine.AddResource(resource_file_prefix + std::to_string(i) + ".txt", false);
		}

		machine.AddResource(resource_file_prefix + std::to_string(transport) + ".txt", false);
	}
	catch (const std::ifstream::failure& e) {
		throw;
	}
}

static void CreateRecipe(int r) {
	const std::string filepath = recipe_dir + "recipe" + std::to_string(r) + "R.json";
	std::ofstream stream;
	stream.exceptions(std::ofstream::badbit);

	try {
		stream.open(filepath, std::ios::out | std::ios::trunc);
		stream << "{\n";
		stream << "  \"initialState\": \"A1\",\n";
		stream << "  \"transitions\": [\n";

		for (int i = 1; i <= r; i++) {
			stream << "    {\n";
			stream << "      \"startState\": \"A" + std::to_string(i) + "\",\n";
			stream << "      \"label\": {\n";
			stream << "        \"guard\": {},\n";
			stream << "        \"sequential\": [\n";
			stream << "          {\n";
			stream << "            \"name\": \"o" + std::to_string(i) + "\",\n";
			stream << "            \"input\": [";

			if (i > 1) {
				stream << "\"p" + std::to_string(i - 1) + "\"";
			}

			stream << "],\n";
			stream << "            \"parameters\": [],\n";

			if (model_size == ModelSize::small)
			{
				stream << "            \"output\": [\"p" + std::to_string(i) + "\"]\n";
			}
			else if (model_size == ModelSize::medium || model_size == ModelSize::big)
			{
				stream << "            \"output\": [\"p" + std::to_string(i) + "\'\"]\n";
			}

			stream << "          }\n";
			stream << "        ],\n";
			stream << "        \"parallel\": []\n";
			stream << "      },\n";

			if (model_size == ModelSize::small)
			{
				stream << "      \"endState\": \"A" + std::to_string(i + 1) + "\"\n";
				stream << "    }";

				if (i < r) {
					stream << ",";
				}

				stream << "\n";
			}
			else if (model_size == ModelSize::medium || model_size == ModelSize::big)
			{
				stream << "      \"endState\": \"A" + std::to_string(i) + "\'\"\n";
				stream << "    },\n";

				stream << "    {\n";
				stream << "      \"startState\": \"A" + std::to_string(i) + "\'\",\n";
				stream << "      \"label\": {\n";
				stream << "        \"guard\": {},\n";
				stream << "        \"sequential\": [\n";
				stream << "          {\n";
				stream << "            \"name\": \"o" + std::to_string(i) + "a\",\n";
				stream << "            \"input\": [\"p" + std::to_string(i) + "\'\"";
				stream << "],\n";
				stream << "            \"parameters\": [],\n";

				if (model_size == ModelSize::medium)
				{
					stream << "            \"output\": [\"p" + std::to_string(i) + "\"]\n";
				}
				else if (model_size == ModelSize::big)
				{
					stream << "            \"output\": [\"p" + std::to_string(i) + "\'\'\"]\n";
				}
				
				stream << "          }\n";
				stream << "        ],\n";
				stream << "        \"parallel\": []\n";
				stream << "      },\n";

				if (model_size == ModelSize::medium)
				{
					stream << "      \"endState\": \"A" + std::to_string(i + 1) + "\"\n";
					stream << "    }";

					if (i < r) {
						stream << ",";
					}

					stream << "\n";
				}
				else if (model_size == ModelSize::big)
				{
					stream << "      \"endState\": \"A" + std::to_string(i) + "\'\'\"\n";
					stream << "    },\n";

					stream << "    {\n";
					stream << "      \"startState\": \"A" + std::to_string(i) + "\'\'\",\n";
					stream << "      \"label\": {\n";
					stream << "        \"guard\": {},\n";
					stream << "        \"sequential\": [\n";
					stream << "          {\n";
					stream << "            \"name\": \"o" + std::to_string(i) + "b\",\n";
					stream << "            \"input\": [\"p" + std::to_string(i) + "\'\'\"";
					stream << "],\n";
					stream << "            \"parameters\": [],\n";
					stream << "            \"output\": [\"p" + std::to_string(i) + "\"]\n";
					stream << "          }\n";
					stream << "        ],\n";
					stream << "        \"parallel\": []\n";
					stream << "      },\n";

					stream << "      \"endState\": \"A" + std::to_string(i + 1) + "\"\n";
					stream << "    }";

					if (i < r) {
						stream << ",";
					}

					stream << "\n";
				}
			}
		}

		stream << "  ]\n";
		stream << "}\n";
	}
	catch (const std::ofstream::failure& e) {
		throw;
	}
}

static void LoadRecipe(pcs::Recipe& recipe, int n) {
	recipes_created = std::filesystem::is_directory(recipe_dir);
	if (!recipes_created) {
		std::filesystem::create_directory(recipe_dir);

		for (int i = 1; i <= num_resources - 1; i++) {
			CreateRecipe(i);
		}

		recipes_created = true;
	}

	recipe.set_recipe(recipe_dir + "recipe" + std::to_string(n - 1) + "R.json");
}

static void CreateCostsFile() {
	std::ofstream stream;
	stream.exceptions(std::ofstream::badbit);

	try {
		stream.open(costs_file, std::ios::out | std::ios::trunc);

		for (int i = 0; i < num_resources; i++) {
			stream << "1\n";
		}
	}
	catch (const std::ofstream::failure& e) {
		throw;
	}
}

static void OutputStats(const pcs::Environment& machine)
{
	std::ofstream stats("stats.txt", std::ios::out | std::ios::app);

	stats << "Number Of States = " + std::to_string(machine.topology()->lts().NumOfStates())
		+ ", Number of Transitions = " + std::to_string(machine.topology()->lts().NumOfTransitions()) << std::endl;
}

static void OutputStats(size_t cost, const std::list<size_t>& resources, const pcs::Environment& machine)
{
	std::ofstream stats("stats.txt", std::ios::out | std::ios::app);

	stats << "Cost = " + std::to_string(cost) + ", Resources = {";

	for (auto& r : resources) {
		stats << std::to_string(r) + ", ";
	}

	stats << "} ";

	OutputStats(machine);
}

static void OutputStats(size_t cost, const std::unordered_set<size_t>& resources, const pcs::Environment& machine)
{
	OutputStats(cost, std::list(resources.begin(), resources.end()), machine);
}

static void CreateController(benchmark::State& state, int n, int transport) {
	pcs::Environment machine;
	LoadResources(machine, n, transport);
	pcs::Recipe recipe;
	LoadRecipe(recipe, n);

	for (auto _ : state) {
		machine.Incremental();
		pcs::Controller con(&machine, machine.topology(), &recipe);
		auto controller_lts = con.Generate();
		benchmark::DoNotOptimize(controller_lts);
		benchmark::ClobberMemory();
	}

	OutputStats(machine);
}

static void CreateBestControllerMinRes(benchmark::State& state, int n, int transport) {
	pcs::Environment machine;
	LoadResources(machine, n, transport);
	pcs::Recipe recipe;
	LoadRecipe(recipe, n);

	size_t cost = 0;
	std::unordered_set<size_t> resources;

	for (auto _ : state) {
		machine.Incremental();
		pcs::BestController con(&machine, machine.topology(), &recipe);
		auto controller_lts = con.Generate(pcs::MinimizeOpt::Resources);
		cost = con.GetCost();
		resources = con.GetResources();
		benchmark::DoNotOptimize(controller_lts);
		benchmark::ClobberMemory();
	}

	OutputStats(cost, resources, machine);
}

static void CreateBestControllerMinTrans(benchmark::State& state, int n, int transport) {
	pcs::Environment machine;
	LoadResources(machine, n, transport);
	pcs::Recipe recipe;
	LoadRecipe(recipe, n);

	size_t cost = 0;
	std::unordered_set<size_t> resources;

	for (auto _ : state) {
		machine.Incremental();
		pcs::BestController con(&machine, machine.topology(), &recipe);
		auto controller_lts = con.Generate(pcs::MinimizeOpt::Transitions);
		cost = con.GetCost();
		resources = con.GetResources();
		benchmark::DoNotOptimize(controller_lts);
		benchmark::ClobberMemory();
	}

	OutputStats(cost, resources, machine);
}

static void CreateBestControllerMinCost(benchmark::State& state, int n, int transport) {
	pcs::Environment machine;
	LoadResources(machine, n, transport);
	pcs::Recipe recipe;
	LoadRecipe(recipe, n);
	CreateCostsFile();

	size_t cost = 0;
	std::list<size_t> resources;

	for (auto _ : state) {
		machine.Incremental();
		pcs::BestController con(&machine, machine.topology(), &recipe);
		auto controller_lts = con.Generate(pcs::MinimizeOpt::Cost, costs_file);
		cost = con.GetCost();
		resources = con.GetResourceList();
		benchmark::DoNotOptimize(controller_lts);
		benchmark::ClobberMemory();
	}

	OutputStats(cost, resources, machine);
}

static void CreateBestControllerMinCostEstimate(benchmark::State& state, int n, int transport) {
	pcs::Environment machine;
	LoadResources(machine, n, transport);
	pcs::Recipe recipe;
	LoadRecipe(recipe, n);
	CreateCostsFile();

	size_t cost = 0;
	std::list<size_t> resources;

	for (auto _ : state) {
		machine.Incremental();
		pcs::BestController con(&machine, machine.topology(), &recipe);
		auto controller_lts = con.Generate(pcs::MinimizeOpt::CostEstimate, costs_file);
		cost = con.GetCost();
		resources = con.GetResourceList();
		benchmark::DoNotOptimize(controller_lts);
		benchmark::ClobberMemory();
	}

	OutputStats(cost, resources, machine);
}

static void CreateLocalBestControllerMinRes(benchmark::State& state, int n, int transport) {
	pcs::Environment machine;
	LoadResources(machine, n, transport);
	pcs::Recipe recipe;
	LoadRecipe(recipe, n);

	size_t cost = 0;
	std::unordered_set<size_t> resources;

	for (auto _ : state) {
		machine.Incremental();
		pcs::LocalBestController con(&machine, machine.topology(), &recipe);
		auto controller_lts = con.Generate(pcs::MinimizeOpt::Resources);
		cost = con.GetCost();
		resources = con.GetResources();
		benchmark::DoNotOptimize(controller_lts);
		benchmark::ClobberMemory();
	}

	OutputStats(cost, resources, machine);
}

static void CreateLocalBestControllerMinCost(benchmark::State& state, int n, int transport) {
	pcs::Environment machine;
	LoadResources(machine, n, transport);
	pcs::Recipe recipe;
	LoadRecipe(recipe, n);
	CreateCostsFile();

	size_t cost = 0;
	std::list<size_t> resources;

	for (auto _ : state) {
		machine.Incremental();
		pcs::LocalBestController con(&machine, machine.topology(), &recipe);
		auto controller_lts = con.Generate(pcs::MinimizeOpt::Cost, costs_file);
		cost = con.GetCost();
		resources = con.GetResourceList();
		benchmark::DoNotOptimize(controller_lts);
		benchmark::ClobberMemory();
	}

	OutputStats(cost, resources, machine);
}

static void CreateFilenames(ModelSize size) {
	model_size = size;

	switch (model_size) {
	case ModelSize::small:
		machine_name = machine_name_small;
		break;
	case ModelSize::medium:
		machine_name = machine_name_medium;
		break;
	case ModelSize::big:
		machine_name = machine_name_big;
		break;
	default:
		assert(false);
	}

	machine_dir = "../../data/" + machine_name;
	resource_file_prefix = machine_dir + "/Resource";
	recipe_dir = machine_dir + "/experiments/";
	costs_file = machine_dir + "/costs.txt";
}

static void CreateController1(benchmark::State& state) {
	CreateFilenames(ModelSize::small);
	CreateController(state, state.range(0), num_resources);
}

static void CreateBestControllerMinRes1(benchmark::State& state) {
	CreateFilenames(ModelSize::small);
	CreateBestControllerMinRes(state, state.range(0), num_resources);
}

static void CreateBestControllerMinTrans1(benchmark::State& state) {
	CreateFilenames(ModelSize::small);
	CreateBestControllerMinTrans(state, state.range(0), num_resources);
}

static void CreateBestControllerMinCost1(benchmark::State& state) {
	CreateFilenames(ModelSize::small);
	CreateBestControllerMinCost(state, state.range(0), num_resources);
}

static void CreateBestControllerMinCostEstimate1(benchmark::State& state) {
	CreateFilenames(ModelSize::small);
	CreateBestControllerMinCostEstimate(state, state.range(0), num_resources);
}

static void CreateLocalBestControllerMinRes1(benchmark::State& state) {
	CreateFilenames(ModelSize::small);
	CreateLocalBestControllerMinRes(state, state.range(0), num_resources);
}

static void CreateLocalBestControllerMinCost1(benchmark::State& state) {
	CreateFilenames(ModelSize::small);
	CreateLocalBestControllerMinCost(state, state.range(0), num_resources);
}

static void CreateController2(benchmark::State& state) {
	CreateFilenames(ModelSize::medium);
	CreateController(state, state.range(0), num_resources);
}

static void CreateBestControllerMinRes2(benchmark::State& state) {
	CreateFilenames(ModelSize::medium);
	CreateBestControllerMinRes(state, state.range(0), num_resources);
}

static void CreateBestControllerMinTrans2(benchmark::State& state) {
	CreateFilenames(ModelSize::medium);
	CreateBestControllerMinTrans(state, state.range(0), num_resources);
}

static void CreateBestControllerMinCost2(benchmark::State& state) {
	CreateFilenames(ModelSize::medium);
	CreateBestControllerMinCost(state, state.range(0), num_resources);
}

static void CreateBestControllerMinCostEstimate2(benchmark::State& state) {
	CreateFilenames(ModelSize::medium);
	CreateBestControllerMinCostEstimate(state, state.range(0), num_resources);
}

static void CreateLocalBestControllerMinRes2(benchmark::State& state) {
	CreateFilenames(ModelSize::medium);
	CreateLocalBestControllerMinRes(state, state.range(0), num_resources);
}

static void CreateLocalBestControllerMinCost2(benchmark::State& state) {
	CreateFilenames(ModelSize::medium);
	CreateLocalBestControllerMinCost(state, state.range(0), num_resources);
}

static void CreateController3(benchmark::State& state) {
	CreateFilenames(ModelSize::big);
	CreateController(state, state.range(0), num_resources);
}

static void CreateBestControllerMinRes3(benchmark::State& state) {
	CreateFilenames(ModelSize::big);
	CreateBestControllerMinRes(state, state.range(0), num_resources);
}

static void CreateBestControllerMinTrans3(benchmark::State& state) {
	CreateFilenames(ModelSize::big);
	CreateBestControllerMinTrans(state, state.range(0), num_resources);
}

static void CreateBestControllerMinCost3(benchmark::State& state) {
	CreateFilenames(ModelSize::big);
	CreateBestControllerMinCost(state, state.range(0), num_resources);
}

static void CreateBestControllerMinCostEstimate3(benchmark::State& state) {
	CreateFilenames(ModelSize::big);
	CreateBestControllerMinCostEstimate(state, state.range(0), num_resources);
}

static void CreateLocalBestControllerMinRes3(benchmark::State& state) {
	CreateFilenames(ModelSize::big);
	CreateLocalBestControllerMinRes(state, state.range(0), num_resources);
}

static void CreateLocalBestControllerMinCost3(benchmark::State& state) {
	CreateFilenames(ModelSize::big);
	CreateLocalBestControllerMinCost(state, state.range(0), num_resources);
}

//BENCHMARK(CreateController1)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinRes1)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinTrans1)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinCost1)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
BENCHMARK(CreateBestControllerMinCostEstimate1)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateLocalBestControllerMinRes1)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateLocalBestControllerMinCost1)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);

//BENCHMARK(CreateController2)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinRes2)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinTrans2)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinCost2)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinCostEstimate2)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateLocalBestControllerMinRes2)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateLocalBestControllerMinCost2)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);

//BENCHMARK(CreateController3)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinRes3)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinTrans3)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinCost3)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinCostEstimate3)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateLocalBestControllerMinRes3)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateLocalBestControllerMinCost3)->DenseRange(2, num_resources, 2)->Unit(benchmark::kMillisecond);

//BENCHMARK_MAIN();
int main(int argc, char** argv)
{
	::benchmark::RegisterMemoryManager(mm.get());
	::benchmark::Initialize(&argc, argv);
	::benchmark::RunSpecifiedBenchmarks();
	::benchmark::RegisterMemoryManager(nullptr);
}
