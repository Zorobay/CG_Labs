//
// Created by sebastian on 2018-10-15.
//

#include <core/node.hpp>
#include <core/helpers.hpp>
#include "Snejk.h"

auto head_node = Node();

Snejk::Snejk(GLuint const* const program, std::function<void (GLuint)> const& set_uniforms, bonobo::mesh_data const& shape) {
    head_node.set_program(program, set_uniforms);
    head_node.set_geometry(shape);
}

void Snejk::render(glm::mat4 const& world_to_clip) {
    head_node.render(world_to_clip, head_node.get_transform());
}