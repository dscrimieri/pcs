#include "pcs/controller/realisability.h"
#include "pcs/lts/state.h"


namespace pcs::controller {
    Realisability::Realisability()
    {

    }

    bool Realisability::checkRealisability(LabelledTransitionSystem<std::string> lts, Recipe recipe)
    {
        // add loop for the size of recipe states if the states of recipe are not in sequential order
        if (lts.GetInitialState() == recipe.lts_.GetInitialState()) {
            std::string ltsState = lts.GetInitialState();    
            for (auto ele : recipe.lts_.states_) {
                if (ele.first == ltsState) {
                    for (auto transitionStates : ele.second.transitions_) {
                        for (auto ltsTransitionStates : lts.states_[ltsState].transitions_) {
                            if (transitionStates.second == ltsTransitionStates.second) {
                                ltsState = ltsTransitionStates.second;
                            }
                            else {
                                return false;
                            }
                        }
                    }
                }
                else {
                    return false;
                }
            }
        }
        return false;
    }

}