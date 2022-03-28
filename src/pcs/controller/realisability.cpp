#include "pcs/controller/realisability.h"
#include "pcs/lts/state.h"


namespace pcs::controller {
    Realisability::Realisability()
    {

    }

    bool checkRealisability(std::pair<std::string, std::string> transition, Recipe recipe)
    {
        for (auto ele : recipe.mylts_.states_) {
            for (auto transitionStates : ele.second.transitions_) {
                if (transitionStates.first == transition.first) {
                    return true;
                }
            }
        }
        return false;
    }

    bool exist(std::string element, std::vector<std::pair<std::string, std::string>> lts) {
        for (auto ele : lts) {
            if (element == ele.first) {
                return true;
            }
        }
        return false;
    }
}