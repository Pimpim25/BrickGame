#include "controller.h"

namespace my {
Controller::Controller() noexcept {}
Controller::~Controller() noexcept = default;

void Controller::apply_input(UserAction_t action) noexcept {
  userInput(action, 1);
}
}  // namespace my