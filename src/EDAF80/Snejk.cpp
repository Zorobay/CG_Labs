//
// Created by sebastian on 2018-10-15.
//

#include <core/node.hpp>
#include <core/helpers.hpp>
#include "Snejk.h"

auto head_node = Node();
auto body_node = Node();

Snejk::Snejk(GLuint const* const program, std::function<void (GLuint)> const& set_uniforms, bonobo::mesh_data const& shape) {
    head_node.set_program(program, set_uniforms);
    body_node.set_program(program, set_uniforms);
    head_node.set_geometry(shape);
    body_node.set_geometry(shape);
    body_node.translate(glm::vec3(2, 0, 0));
}

void Snejk::render(glm::mat4 const& world_to_clip) {
    head_node.render(world_to_clip, head_node.get_transform());
    body_node.render(world_to_clip, body_node.get_transform());
}

void Snejk::handle_input(InputHandler inputHandler) {
    if (inputHandler.GetKeycodeState(GLFW_KEY_LEFT) & JUST_PRESSED){
        head_node.translate(glm::vec3(-0.01, ))
    }
}
