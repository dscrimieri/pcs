#pragma once

#include <string>
#include <span>
#include <vector>

#include "pcs/lts/state.h"
#include "pcs/lts/lts.h"
#include "pcs/product/recipe.h"
#include "pcs/controller/topology.h"

namespace pcs::controller {
	bool checkRealisability(std::pair<std::string, std::string> transition, Recipe recipe);
	bool exist(std::string element, std::vector<std::pair<std::string, std::string>> lts);

	class Realisability {
	public:
		pcs::lts::LabelledTransitionSystem<std::string> lts;
	private:
		Realisability();
	};
	
}