// Generates a recipe, a number of resources and runs various benchmarks
// Like extended.cpp, but with two solutions (with different cost)
// Size of recipe and resources can be small, medium or big
// Benchmarks include global and local best controller synthesis

#include <benchmark/benchmark.h>
#include "pcs/controller/controller.h"

#include <vector>
#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>

#include "pcs/topology/topology.h"
#include "lts/parsers/parsers.h"
#include "../memory_manager.h"

const static std::string machine_name_small = "extended2_small";
const static std::string machine_name_medium = "extended2_medium";
const static std::string machine_name_big = "extended2_big";

static std::string machine_name = machine_name_small;
static std::string machine_dir = "../../data/" + machine_name;
static std::string resource_file_prefix = machine_dir + "/Resource";
static std::string recipe_dir = machine_dir + "/experiments/";
static std::string costs_file = machine_dir + "/costs.txt";

static const int nres_controller1 = 0;
static const int nres_best_controller_min_res1 = 0;
static const int nres_best_controller_min_trans1 = 0;
static const int nres_best_controller_min_cost1 = 0;
static const int nres_best_controller_min_cost_estimate1 = 0;
static const int nres_local_best_controller_min_res1 = 0;
static const int nres_local_best_controller_min_cost1 = 0;

static const int nres_controller2 = 0;
static const int nres_best_controller_min_res2 = 0;
static const int nres_best_controller_min_trans2 = 0;
static const int nres_best_controller_min_cost2 = 18;
static const int nres_best_controller_min_cost_estimate2 = 20;
static const int nres_local_best_controller_min_res2 = 0;
static const int nres_local_best_controller_min_cost2 = 182;

static const int nres_controller3 = 0;
static const int nres_best_controller_min_res3 = 0;
static const int nres_best_controller_min_trans3 = 0;
static const int nres_best_controller_min_cost3 = 18;
static const int nres_best_controller_min_cost_estimate3 = 20;
static const int nres_local_best_controller_min_res3 = 0;
static const int nres_local_best_controller_min_cost3 = 142;

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
				stream << "s0. o1_1. s1.\n";
				stream << "s0. o1. s1.\n";
				stream << "s1. out:1. s0.\n";
				stream << "s1. rem. s0.\n";
			}
			else if (model_size == ModelSize::medium || model_size == ModelSize::big)
			{
				stream << "s2. nop. s2.\n";
				stream << "s0. o1_1. s1.\n";
				stream << "s0. o1. s1.\n";
				stream << "s1. out:1. s0.\n";
				stream << "s1. rem. s0.\n";
				stream << "s1. o1_2. s2.\n";
				stream << "s1. o1a. s2.\n";
				stream << "s2. out:1. s0.\n";
				stream << "s2. rem. s0.\n";

				if (model_size == ModelSize::big)
				{
					stream << "s2. o1_3. s3.\n";
					stream << "s2. o1b. s3.\n";
					stream << "s3. out:1. s0.\n";
					stream << "s3. rem. s0.\n";
				}
			}
		} else {
			stream << "s0. o" + std::to_string(i) + "_1. s1.\n";
			stream << "s0. in:" + std::to_string(i) + ". s1.\n";
			stream << "s1. out:" + std::to_string(i) + ". s0.\n";
			stream << "s1. rem. s0.\n";
			stream << "s1. o" + std::to_string(i) + "_2. s2.\n";
			stream << "s1. o" + std::to_string(i) + ". s2.\n";
			stream << "s2. out:" + std::to_string(i) + ". s0.\n";
			stream << "s2. rem. s0.\n";

			if (model_size == ModelSize::medium || model_size == ModelSize::big)
			{
				stream << "s2. o" + std::to_string(i) + "_3. s3.\n";
				stream << "s2. o" + std::to_string(i) + "a. s3.\n";
				stream << "s3. out:" + std::to_string(i) + ". s0.\n";
				stream << "s3. rem. s0.\n";

				if (model_size == ModelSize::big)
				{
					stream << "s3. o" + std::to_string(i) + "_4. s4.\n";
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

static void CreateAlternativeResource(int i, int j) {
	const std::string filepath = resource_file_prefix + std::to_string(i) + ".txt";
	std::ofstream stream;
	stream.exceptions(std::ofstream::badbit);

	try {
		stream.open(filepath, std::ios::out | std::ios::trunc);

		stream << "s0\n";
		stream << "s0. nop. s0.\n";
		stream << "s1. nop. s1.\n";

		assert((j > 1));

		stream << "s0. o" + std::to_string(j) + "_1. s1.\n";
		stream << "s0. in:" + std::to_string(i) + ". s1.\n";
		stream << "s1. out:" + std::to_string(i) + ". s0.\n";
		stream << "s1. rem. s0.\n";
		stream << "s1. o" + std::to_string(j) + "_2. s2.\n";
		stream << "s1. o" + std::to_string(j) + ". s2.\n";
		stream << "s2. out:" + std::to_string(i) + ". s0.\n";
		stream << "s2. rem. s0.\n";

		if (model_size == ModelSize::medium || model_size == ModelSize::big)
		{
			stream << "s2. o" + std::to_string(j) + "_3. s3.\n";
			stream << "s2. o" + std::to_string(j) + "a. s3.\n";
			stream << "s3. out:" + std::to_string(i) + ". s0.\n";
			stream << "s3. rem. s0.\n";

			if (model_size == ModelSize::big)
			{
				stream << "s3. o" + std::to_string(j) + "_4. s4.\n";
				stream << "s3. o" + std::to_string(j) + "b. s4.\n";
				stream << "s4. out:" + std::to_string(i) + ". s0.\n";
				stream << "s4. rem. s0.\n";
			}
		}
	}
	catch (const std::ofstream::failure& e) {
		throw;
	}
}

static void CreateTransportResource(int transport, int nres) {
	const std::string filepath = resource_file_prefix + std::to_string(transport) + ".txt";
	std::ofstream stream;
	stream.exceptions(std::ofstream::badbit);

	try {
		stream.open(filepath, std::ios::out | std::ios::trunc);
		stream << "s0\n";
		stream << "s0. nop. s0.\n";
		stream << "s0a. nop. s0a.\n";
		stream << "s0b. nop. s0b.\n";

		stream << "s0. in:1. s1.\n";
		stream << "s1. out:2. s0a.\n";
		stream << "s1. out:" + std::to_string(nres - 1 + 2) + ". s0b.\n";

		for (int i = 1; i < nres; i++) {
			stream << "s0a. in:" + std::to_string(i) + ". s" + std::to_string(i) + ".\n";
			if (i > 1) {
				stream << "s" + std::to_string(i) + ". out:" + std::to_string(i + 1) + ". s0a.\n";
				if (i > 2) {
					stream << "s" + std::to_string(i) + ". out:" + std::to_string(i - 1) + ". s0a.\n";
				}
			}			

			if (i > 1) {
				stream << "s0b. in:" + std::to_string(nres - 1 + i) + ". s" + std::to_string(nres - 1 + i) + ".\n";
				stream << "s" + std::to_string(nres - 1 + i) + ". out:" + std::to_string(nres - 1 + i + 1) + ". s0b.\n";
				if (i > 2) {
					stream << "s" + std::to_string(nres - 1 + i) + ". out:" + std::to_string(nres - 1 + i - 1) + ". s0b.\n";
				}
			}
		}
	}
	catch (const std::ofstream::failure& e) {
		throw;
	}
}

static void LoadResources(pcs::Environment& machine, int nres, int transport) {
	resources_created = std::filesystem::is_directory(machine_dir);
	if (!resources_created) {
		std::filesystem::create_directory(machine_dir);

		for (int i = 1; i < nres; i++) {
			CreateResource(i);
		}

		for (int i = 2; i < nres; i++) {
			CreateAlternativeResource(nres - 1 + i, i);
		}

		CreateTransportResource(transport, nres);
		resources_created = true;
	}

	try {
		for (int i = 1; i < nres * 2 - 1; i++) {
			machine.AddResource(resource_file_prefix + std::to_string(i) + ".txt", false);
		}
	}
	catch (const std::ifstream::failure& e) {
		throw;
	}
}

static void CreateRecipe(int recipe) {
	const std::string filepath = recipe_dir + "recipe" + std::to_string(recipe) + "R.json";
	std::ofstream stream;
	stream.exceptions(std::ofstream::badbit);

	try {
		stream.open(filepath, std::ios::out | std::ios::trunc);
		stream << "{\n";
		stream << "  \"initialState\": \"A1\",\n";
		stream << "  \"transitions\": [\n";

		for (int i = 1; i <= recipe; i++) {
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

				if (i < recipe) {
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

					if (i < recipe) {
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

					if (i < recipe) {
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

static void LoadRecipe(pcs::Recipe& recipe, int nres) {
	recipes_created = std::filesystem::is_directory(recipe_dir);
	if (!recipes_created) {
		std::filesystem::create_directory(recipe_dir);
		CreateRecipe(nres - 1);
		recipes_created = true;
	}

	recipe.set_recipe(recipe_dir + "recipe" + std::to_string(nres - 1) + "R.json");
}

static void CreateCostsFile(int nres) {	
	std::ofstream stream;
	stream.exceptions(std::ofstream::badbit);

	try {
		stream.open(costs_file, std::ios::out | std::ios::trunc);
		
		for (int i = nres * 2 - 2; i >= 1; i--) {
			int cost = ceil((float)i / (nres * 2 - 2) * 10);
			stream << std::to_string(cost) + "\n";
		}
	}
	catch (const std::ofstream::failure& e) {
		throw;
	}
}

static std::string getDate()
{
	std::time_t rawtime;
	std::tm* timeinfo;
	char buffer[80];

	std::time(&rawtime);
	timeinfo = std::localtime(&rawtime);
	std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);

	return std::string(buffer);
}

static const std::string logfile = "log.csv";

static void WriteLog(const pcs::Environment& machine, std::string benchmark, int nres)
{
	std::ofstream log(logfile, std::ios::out | std::ios::app);

	log << getDate() << ","
		<< benchmark << ","
		<< model_size + 1 << ","
		<< nres << ","
		<< std::to_string(machine.topology()->lts().NumOfStates()) << ","
		<< std::to_string(machine.topology()->lts().NumOfTransitions()) << std::endl;
}

static void WriteLog(size_t cost, const std::list<size_t>& resources, const pcs::Environment& machine, std::string benchmark, int nres)
{
	std::ofstream log(logfile, std::ios::out | std::ios::app);

	log << getDate() << ","
		<< benchmark << ","
		<< model_size + 1 << ","
		<< nres << ","
		<< std::to_string(machine.topology()->lts().NumOfStates()) << ","
		<< std::to_string(machine.topology()->lts().NumOfTransitions()) << ","
		<< std::to_string(cost) << ",{";

	for (auto& r : resources) {
		log << std::to_string(r) << " ";
	}

	log << "}" << std::endl;
}

static void WriteLog(size_t cost, const std::unordered_set<size_t>& resources, const pcs::Environment& machine, std::string benchmark, int nres)
{
	std::list<size_t> resource_list = std::list(resources.begin(), resources.end());
	WriteLog(cost, resource_list, machine, benchmark, nres);
}

static void CreateController(benchmark::State& state, int nres, int transport) {
	pcs::Environment machine;
	LoadResources(machine, nres, transport);
	pcs::Recipe recipe;
	LoadRecipe(recipe, nres);

	for (auto _ : state) {
		machine.Incremental();
		pcs::Controller con(&machine, machine.topology(), &recipe);
		auto controller_lts = con.Generate();
		benchmark::DoNotOptimize(controller_lts);
		benchmark::ClobberMemory();
	}

	WriteLog(machine, "CreateController", nres);
}

static void CreateBestControllerMinRes(benchmark::State& state, int nres, int transport) {
	pcs::Environment machine;
	LoadResources(machine, nres, transport);
	pcs::Recipe recipe;
	LoadRecipe(recipe, nres);

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

	WriteLog(cost, resources, machine, "CreateBestControllerMinRes", nres);
}

static void CreateBestControllerMinTrans(benchmark::State& state, int nres, int transport) {
	pcs::Environment machine;
	LoadResources(machine, nres, transport);
	pcs::Recipe recipe;
	LoadRecipe(recipe, nres);

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

	WriteLog(cost, resources, machine, "CreateBestControllerMinTrans", nres);
}

static void CreateBestControllerMinCost(benchmark::State& state, int nres, int transport) {
	pcs::Environment machine;
	LoadResources(machine, nres, transport);
	pcs::Recipe recipe;
	LoadRecipe(recipe, nres);
	CreateCostsFile(nres);

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

	WriteLog(cost, resources, machine, "CreateBestControllerMinCost", nres);
}

static void CreateBestControllerMinCostEstimate(benchmark::State& state, int nres, int transport) {
	pcs::Environment machine;
	LoadResources(machine, nres, transport);
	pcs::Recipe recipe;
	LoadRecipe(recipe, nres);
	CreateCostsFile(nres);

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

	WriteLog(cost, resources, machine, "CreateBestControllerMinCostEstimate", nres);
}

static void CreateLocalBestControllerMinRes(benchmark::State& state, int nres, int transport) {
	pcs::Environment machine;
	LoadResources(machine, nres, transport);
	pcs::Recipe recipe;
	LoadRecipe(recipe, nres);

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

	WriteLog(cost, resources, machine, "CreateLocalBestControllerMinRes", nres);
}

static void CreateLocalBestControllerMinCost(benchmark::State& state, int nres, int transport) {
	pcs::Environment machine;
	LoadResources(machine, nres, transport);
	pcs::Recipe recipe;
	LoadRecipe(recipe, nres);
	CreateCostsFile(nres);

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

	WriteLog(cost, resources, machine, "CreateLocalBestControllerMinCost", nres);
}

static void CreateFilenames(ModelSize size, int nres) {
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

	machine_dir = "../../data/" + machine_name + " " + std::to_string(nres);
	resource_file_prefix = machine_dir + "/Resource";
	recipe_dir = machine_dir + "/experiments/";
	costs_file = machine_dir + "/costs.txt";
}

static void CreateController1(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::small, nres);
	CreateController(state, nres, nres);
}

static void CreateBestControllerMinRes1(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::small, nres);
	CreateBestControllerMinRes(state, nres, nres);
}

static void CreateBestControllerMinTrans1(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::small, nres);
	CreateBestControllerMinTrans(state, nres, nres);
}

static void CreateBestControllerMinCost1(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::small, nres);
	CreateBestControllerMinCost(state, nres, nres);
}

static void CreateBestControllerMinCostEstimate1(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::small, nres);
	CreateBestControllerMinCostEstimate(state, nres, nres);
}

static void CreateLocalBestControllerMinRes1(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::small, nres);
	CreateLocalBestControllerMinRes(state, nres, nres);
}

static void CreateLocalBestControllerMinCost1(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::small, nres);
	CreateLocalBestControllerMinCost(state, nres, nres);
}

static void CreateController2(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::medium, nres);
	CreateController(state, nres, nres);
}

static void CreateBestControllerMinRes2(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::medium, nres);
	CreateBestControllerMinRes(state, nres, nres);
}

static void CreateBestControllerMinTrans2(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::medium, nres);
	CreateBestControllerMinTrans(state, nres, nres);
}

static void CreateBestControllerMinCost2(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::medium, nres);
	CreateBestControllerMinCost(state, nres, nres);
}

static void CreateBestControllerMinCostEstimate2(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::medium, nres);
	CreateBestControllerMinCostEstimate(state, nres, nres);
}

static void CreateLocalBestControllerMinRes2(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::medium, nres);
	CreateLocalBestControllerMinRes(state, nres, nres);
}

static void CreateLocalBestControllerMinCost2(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::medium, nres);
	CreateLocalBestControllerMinCost(state, nres, nres);
}

static void CreateController3(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::big, nres);
	CreateController(state, nres, nres);
}

static void CreateBestControllerMinRes3(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::big, nres);
	CreateBestControllerMinRes(state, nres, nres);
}

static void CreateBestControllerMinTrans3(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::big, nres);
	CreateBestControllerMinTrans(state, nres, nres);
}

static void CreateBestControllerMinCost3(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::big, nres);
	CreateBestControllerMinCost(state, nres, nres);
}

static void CreateBestControllerMinCostEstimate3(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::big, nres);
	CreateBestControllerMinCostEstimate(state, nres, nres);
}

static void CreateLocalBestControllerMinRes3(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::big, nres);
	CreateLocalBestControllerMinRes(state, nres, nres);
}

static void CreateLocalBestControllerMinCost3(benchmark::State& state) {
	int nres = state.range(0);
	CreateFilenames(ModelSize::big, nres);
	CreateLocalBestControllerMinCost(state, nres, nres);
}

//BENCHMARK(CreateController1)->DenseRange(2, nres_controller1, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinRes1)->DenseRange(2, nres_best_controller_min_res1, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinTrans1)->DenseRange(2, nres_best_controller_min_trans1, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinCost1)->DenseRange(2, nres_best_controller_min_cost1, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinCostEstimate1)->DenseRange(2, nres_best_controller_min_cost_estimate1, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateLocalBestControllerMinRes1)->DenseRange(2, nres_local_best_controller_min_res1, 10)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateLocalBestControllerMinCost1)->DenseRange(2, nres_local_best_controller_min_cost1, 10)->Unit(benchmark::kMillisecond);

//BENCHMARK(CreateController2)->DenseRange(2, nres_controller2, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinRes2)->DenseRange(2, nres_best_controller_min_res2, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinTrans2)->DenseRange(2, nres_best_controller_min_trans2, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinCost2)->DenseRange(2, nres_best_controller_min_cost2, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinCostEstimate2)->DenseRange(2, nres_best_controller_min_cost_estimate2, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateLocalBestControllerMinRes2)->DenseRange(2, nres_local_best_controller_min_res2, 10)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateLocalBestControllerMinCost2)->DenseRange(2, nres_local_best_controller_min_cost2, 10)->Unit(benchmark::kMillisecond);

//BENCHMARK(CreateController3)->DenseRange(2, nres_controller3, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinRes3)->DenseRange(2, nres_best_controller_min_res3, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinTrans3)->DenseRange(2, nres_best_controller_min_trans3, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinCost3)->DenseRange(2, nres_best_controller_min_cost3, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateBestControllerMinCostEstimate3)->DenseRange(2, nres_best_controller_min_cost_estimate3, 2)->Unit(benchmark::kMillisecond);
//BENCHMARK(CreateLocalBestControllerMinRes3)->DenseRange(2, nres_local_best_controller_min_res3, 10)->Unit(benchmark::kMillisecond);
BENCHMARK(CreateLocalBestControllerMinCost3)->DenseRange(2, nres_local_best_controller_min_cost3, 10)->Unit(benchmark::kMillisecond);

//BENCHMARK_MAIN();
int main(int argc, char** argv)
{
	::benchmark::RegisterMemoryManager(mm.get());
	::benchmark::Initialize(&argc, argv);
	::benchmark::RunSpecifiedBenchmarks();
	::benchmark::RegisterMemoryManager(nullptr);
}
