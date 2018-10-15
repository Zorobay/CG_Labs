//
// Created by sebastian on 2018-10-15.
//

#ifndef CG_LABS_SNEJK_H
#define CG_LABS_SNEJK_H

#include "core/Misc.h"
#include "core/node.hpp"
#include "core/ShaderProgramManager.hpp"

class Snejk {
public:
    Snejk(const GLuint * const shader, const std::function<void(GLuint)> &set_uniforms, bonobo::mesh_data shape);
    void render();
};


#endif //CG_LABS_SNEJK_H
