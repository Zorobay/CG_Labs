//
// Created by sebastian on 2018-10-15.
//

#include <core/node.hpp>
#include <core/helpers.hpp>
#include "Snejk.h"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/geometric.hpp"

auto head_node = Node();
glm::vec3 head_position = glm::vec3(0, 0, 0);
auto move_direction = glm::normalize(glm::vec3(0, 0, -1));


Snejk::Snejk(GLuint const *const shader, std::function<void(GLuint)> const &set_uniforms,
             bonobo::mesh_data const &shape) {

    _shader = shader;
    _set_uniforms = set_uniforms;
    _shape = shape;

    head_node.set_program(_shader, _set_uniforms);
    head_node.set_geometry(_shape);
    head_node.set_translation(head_position);
    head_node.set_scaling(glm::vec3(_head_radi));
    head_node.add_texture("normal_map", _texture_bump, GL_TEXTURE_2D);
    head_node.add_texture("diffuse_texture", _texture_diffuse, GL_TEXTURE_2D);

    for(int i = 0; i < 100; i++){
        _positions.push_back(head_position);
        _directions.push_back(move_direction);
    }
}

void Snejk::render(glm::mat4 const &world_to_clip, const float delta_time) {

    // Check if dead
    if (!isAlive()){
        return;
    }
    // Move head
    head_position += move_direction * delta_time * move_speed;
    head_node.set_translation(head_position);
    head_node.set_rotation_y(_rotation);
    head_node.render(world_to_clip, head_node.get_transform());
    _positions.push_back(head_position);
    _directions.push_back(move_direction);

    // Move tail
    for (size_t i = 0; i < _nodes.size(); i++) { // move all nodes
        size_t pos_i = std::min(_positions.size() - 1, (_positions.size() - 1 - (i + 1) * _tail_segment_offset));
        _nodes[i].set_translation(_positions[pos_i]);
        _nodes[i].render(world_to_clip, _nodes[i].get_transform());
    }


    _counter++;
}

void Snejk::handle_input(InputHandler inputHandler) {
    float rotation_amount = glm::two_pi<float>() * turn_speed;

    if (inputHandler.GetKeycodeState(GLFW_KEY_LEFT) & PRESSED) { //Turn left
        _rotation += rotation_amount;
        move_direction = glm::normalize(glm::rotateY(move_direction, rotation_amount));
    }
    if (inputHandler.GetKeycodeState(GLFW_KEY_RIGHT) & PRESSED) { //Turn right
        _rotation -= rotation_amount;
        move_direction = glm::normalize(glm::rotateY(move_direction, -rotation_amount));
    }
    if (inputHandler.GetKeycodeState(GLFW_KEY_N) & JUST_PRESSED) { //Add new body part
        add_node(_tail_radi);
    }
}

glm::vec3 Snejk::get_position() {
    return head_position;
}

float Snejk::get_rotation_y() {
    return _rotation;
}

void Snejk::add_node(const float size_multiplier) {
    auto new_node = Node();
    new_node.set_geometry(_shape);
    new_node.set_program(_shader, _set_uniforms);
    new_node.add_texture("normal_map", _texture_bump, GL_TEXTURE_2D);
    new_node.add_texture("diffuse_texture", _texture_diffuse, GL_TEXTURE_2D);
    new_node.set_scaling(glm::vec3(size_multiplier));

    _nodes.push_back(new_node);

    size_t pos_i = std::min(_positions.size() - 1, (_positions.size() - 1 - _nodes.size() * _tail_segment_offset));
    new_node.set_translation(_positions[pos_i]);
}


void Snejk::add_node() {
    add_node(_tail_radi);
}

// Function that tests for collision between head and tail
bool Snejk::isAlive() {
    for (Node n: _nodes) {
        if (_tail_radi + 1.0f > glm::distance(n.get_translation(), head_position)) {//distance of center < tail + head radi
            return false;
        }
    }
    return true;
}

glm::vec3 Snejk::get_move_direction() {
    return move_direction;
}

float Snejk::get_radius() {
    return _head_radi;
}


