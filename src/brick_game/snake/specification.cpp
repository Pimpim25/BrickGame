#include "specification.h"

#include "model.h"

void init_if_needed() {
  if (!my::gamePtr) {
    my::gamePtr = std::make_unique<my::Model>();
  }
}

void userInput(UserAction_t action, bool hold) {
  init_if_needed();
  my::gamePtr->applyInput(action);
}

GameInfo_t updateCurrentState() {
  init_if_needed();
  if (my::gamePtr->check_the_time()) {
    my::gamePtr->tick();
  }
  return my::gamePtr->snapshot();
}