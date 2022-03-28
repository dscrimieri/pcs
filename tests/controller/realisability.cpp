#include <gtest/gtest.h>
#include "pcs/controller/topology.h"

#include <array>

#include "pcs/lts/lts.h"
#include "pcs/lts/state.h"
#include "pcs/lts/parsers.h"
#include "pcs/product/recipe.h"

TEST(realisability, realisability) {
	pcs::lts::LabelledTransitionSystem expected, got;
	std::array<pcs::lts::LabelledTransitionSystem<>, 2> ltss;
	pcs::Recipe recipe;

	pcs::lts::ReadFromFile(ltss[0], "../../tests/lts/testdata/lts_1.txt");
	pcs::lts::ReadFromFile(ltss[1], "../../tests/lts/testdata/lts_1.txt");
	pcs::lts::ReadFromFile(expected, "../../tests/lts/testdata/lts_1_self_merge_expected.txt");

	recipe.mylts_.SetInitialState("A", true);
	recipe.mylts_.AddTransition("A", "a1", "B");
	recipe.mylts_.AddTransition("B", "a5", "C");
	recipe.mylts_.AddTransition("C", "a2", "D");
	recipe.mylts_.AddTransition("C", "a2", "E");


	//got = pcs::topology::Combine(ltss, recipe);
	bool result = pcs::topology::Combine(ltss, recipe);
	std::cout << std::endl << result << std::endl;
	//ASSERT_EQ(expected, got);
}
