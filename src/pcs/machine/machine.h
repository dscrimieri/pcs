#pragma once

#include <vector>
#include <span>

#include "pcs/lts/lts.h"

namespace pcs {

	using LabelledTransitionSystem = pcs::lts::LabelledTransitionSystem<std::string>;
	using State = pcs::lts::State<std::string>;

	class Machine {
	private:
		std::vector<pcs::lts::LabelledTransitionSystem<std::string>> resources_;
		pcs::lts::LabelledTransitionSystem<std::string> topology_;
	public:
		Machine();
		Machine(const std::span<pcs::lts::LabelledTransitionSystem<std::string>>& resources);
		Machine(std::vector<pcs::lts::LabelledTransitionSystem<std::string>>&& resources);
		void ComputeTopology();
		void ComputeTology(std::initializer_list<size_t> resources);
		void RemoveResource();
		void AddResource();
	};
}