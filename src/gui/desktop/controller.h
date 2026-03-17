// #include "../../brick_game/snake/model.h"
#include "../../brick_game/tetris/backend.h"
#include "../../brick_game/tetris/specification.h"

namespace my {
class Controller {
 public:
  Controller() noexcept;
  ~Controller() noexcept;

  void apply_input(UserAction_t action) noexcept;
};
}  // namespace my