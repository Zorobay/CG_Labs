//
// Created by sebastian on 2018-10-15.
//

#ifndef CG_LABS_SNEJK_H
#define CG_LABS_SNEJK_H

#include "core/Misc.h"
#include "core/node.hpp"
#include "core/ShaderProgramManager.hpp"
#include "core/InputHandler.h"

// Contants
const float move_speed = 0.002;
const float turn_speed = 0.05;

class Snejk {
public:
    Snejk(GLuint const* const shader, std::function<void (GLuint)> const& set_uniforms, bonobo::mesh_data const& shape);
    void render(glm::mat4 const& world_to_clip, const float delta_time);
    void handle_input(InputHandler inputHandler);
    void add_node(const float size_multiplier);
    glm::vec3 get_position();

private:
    GLuint const* _shader;
    std::function<void (GLuint)> _set_uniforms;
    bonobo::mesh_data const* _shape;
};


#endif //CG_LABS_SNEJK_H
