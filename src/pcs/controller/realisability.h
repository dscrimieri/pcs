#pragma once

#include <string>
#include <span>
#include <vector>

#include "pcs/lts/state.h"
#include "pcs/lts/lts.h"
#include "pcs/product/recipe.h"
#include "pcs/controller/topology.h"

namespace pcs::controller {
	class Realisability {
	public:
		pcs::lts::LabelledTransitionSystem<std::string> lts;
	private:
		Realisability();
		bool checkRealisability(LabelledTransitionSystem<std::string> lts, Recipe recipe);
	};
	
}