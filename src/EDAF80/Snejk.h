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
    Snejk(GLuint const* const program, std::function<void (GLuint)> const& set_uniforms, bonobo::mesh_data const& shape);
    void render(glm::mat4 const& world_to_clip);
};


#endif //CG_LABS_SNEJK_H
