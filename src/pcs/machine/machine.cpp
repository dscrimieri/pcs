#include "pcs/machine/machine.h"

#include "pcs/controller/topology.h"

namespace pcs {

	Machine::Machine(std::vector<pcs::lts::LabelledTransitionSystem<std::string>>&& resources) {
		resources_ = std::move(resources);
	}

	Machine::Machine(const std::span<pcs::lts::LabelledTransitionSystem<std::string>>& resources)  {
		resources_.assign(resources.begin(), resources.end());
	}

	void Machine::ComputeTopology() {
		topology_ = pcs::topology::Combine(resources_);
	}

}
