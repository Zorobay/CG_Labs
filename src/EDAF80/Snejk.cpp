//
// Created by sebastian on 2018-10-15.
//

#include <core/node.hpp>
#include <core/helpers.hpp>
#include "Snejk.h"


Snejk::Snejk(const GLuint *const shader, const std::function<void(GLuint)> &set_uniforms, bonobo::mesh_data shape) {
    auto head_node = Node();
    head_node.set_program(shader, set_uniforms);
    head_node.set_geometry(shape);
}

