#include "pcs/controller/core.h"

#include <span>
#include <vector>

#include <spdlog/fmt/bundled/color.h>
#include <spdlog/fmt/ranges.h>

#include "lts/lts.h"
#include "pcs/controller/plan_transition.h"
#include "pcs/common/log.h"
#include "pcs/operation/task_expression.h"
#include "pcs/operation/composite.h"

namespace pcs {

	using ControllerType = nightly::LTS<std::pair<std::string, std::vector<std::string>>, std::vector<std::string>,
		                                 boost::hash<std::pair<std::string, std::vector<std::string>>>>;

	void ApplyTransition(const PlanTransition& plan_t, ControllerType& controller) {
		controller.AddTransition({ *plan_t.recipe_state, *plan_t.from }, plan_t.label, { *plan_t.recipe_state, *plan_t.to });
		PCS_INFO(fmt::format(fmt::fg(fmt::color::royal_blue) | fmt::emphasis::bold,
			"Adding controller transition from {} with label ({}) to {}", fmt::join(*plan_t.from, ","),
			fmt::join(plan_t.label, ","), fmt::join(*plan_t.to, ",")));
	}

	void ApplyAllTransitions(const std::span<PlanTransition>& plan_transitions, ControllerType& controller) {
		for (const auto& t : plan_transitions) {
			ApplyTransition(t, controller);
		}
	}

}