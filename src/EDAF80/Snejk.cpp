//
// Created by sebastian on 2018-10-15.
//

#include <core/node.hpp>
#include <core/helpers.hpp>
#include "Snejk.h"
#include "glm/gtx/rotate_vector.hpp"

auto head_node = Node();
auto head_position = glm::vec3(0,0,0);
auto move_direction = glm::normalize(glm::vec3(0, 0, -1));


Snejk::Snejk(GLuint const* const shader, std::function<void (GLuint)> const& set_uniforms, bonobo::mesh_data const& shape) {

    _shader = shader;
    _set_uniforms = set_uniforms;
    _shape = shape;

    head_node.set_program(_shader, _set_uniforms);
    head_node.set_geometry(_shape);
    head_node.set_translation(head_position);
    head_node.set_scaling(glm::vec3(1, 1, 0.5));
    head_node.add_texture("normal_map", _texture_bump, GL_TEXTURE_2D);
    head_node.add_texture("diffuse_texture", _texture_diffuse, GL_TEXTURE_2D);
}

void Snejk::render(glm::mat4 const& world_to_clip, const float delta_time) {

    // Move head
    head_position += move_direction * delta_time * move_speed;
    head_node.set_translation(head_position);
    head_node.set_rotation_y(_rotation);
    head_node.render(world_to_clip, head_node.get_transform());
    _positions.push_back(head_position);
    _directions.push_back(move_direction);

    size_t offset = 50;
    // Move tail
    for (size_t i = 0; i < _nodes.size(); i++){ // move all nodes
        size_t pos_i = std::min(_positions.size()-1, (_positions.size()- 1 - (i+1)*offset));
        _nodes[i].set_translation(_positions[pos_i]);
        _nodes[i].render(world_to_clip, _nodes[i].get_transform());
    }

    _counter++;
}

void Snejk::handle_input(InputHandler inputHandler) {
    float rotation_amount = glm::two_pi<float>() * turn_speed;

    if (inputHandler.GetKeycodeState(GLFW_KEY_LEFT) & PRESSED){ //Turn left
        _rotation += rotation_amount;
        move_direction = glm::normalize(glm::rotateY(move_direction, rotation_amount));
    }
    if (inputHandler.GetKeycodeState(GLFW_KEY_RIGHT) & PRESSED) { //Turn right
        _rotation -= rotation_amount;
        move_direction = glm::normalize(glm::rotateY(move_direction, -rotation_amount));
    }
    if (inputHandler.GetKeycodeState(GLFW_KEY_N) & JUST_PRESSED) { //Add new body part
        add_node(0.5f);
    }
}

glm::vec3 Snejk::get_position() {
    return head_position;
}

void Snejk::add_node(const float size_multiplier) {
    auto new_node = Node();
    new_node.set_geometry(_shape);
    new_node.set_program(_shader, _set_uniforms);
    new_node.add_texture("normal_map", _texture_bump, GL_TEXTURE_2D);
    new_node.add_texture("diffuse_texture", _texture_diffuse, GL_TEXTURE_2D);

    new_node.set_scaling(glm::vec3(size_multiplier));

    _nodes.push_back(new_node);
}
