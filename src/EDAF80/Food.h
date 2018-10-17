//
// Created by sebastian on 2018-10-15.
//

#ifndef CG_LABS_SNEJK_H
#define CG_LABS_SNEJK_H

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
    void erase(float f);

  private:
    std::vector<Node> food;
    const float food_radi = 0.3f;
};

#endif //CG_LABS_SNEJK_H
