#ifndef CG_LABS_FOOD_H
#define CG_LABS_FOOD_H

#include "Snejk.h"
#include <core/node.hpp>
#include "core/Misc.h"
#include "core/node.hpp"
#include "core/helpers.hpp"
#include "core/ShaderProgramManager.hpp"
#include "core/InputHandler.h"
#include <ctime>
#include <cmath>

class Food
{
public:
  Food();
  void new_node(Node node);
  void render(glm::mat4 const &WVP, glm::mat4 const &world) const;
  glm::mat4x4 get_transform() const;
  glm::vec3 get_translation() const;
  int kind();

private:
  Node food_node;
};

#endif //CG_LABS_FOOD_H