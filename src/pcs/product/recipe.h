#pragma once

#include <vector>
#include <unordered_map>

#include "pcs/lts/lts.h"
#include "pcs/operation/composite.h"

namespace pcs {

	using namespace pcs::lts;

	class Recipe {
	public:
		pcs::lts::LabelledTransitionSystem<CompositeOperation> lts_;
		pcs::lts::LabelledTransitionSystem<std::string> mylts_;
	public:
		Recipe();
		Recipe(const std::filesystem::path& filepath);
	};
}
