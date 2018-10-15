//
// Created by sebastian on 2018-10-15.
//

#include <core/node.hpp>
#include <core/helpers.hpp>
#include "Snejk.h"
#include "glm/gtx/rotate_vector.hpp"

auto head_node = Node();
std::vector<Node> nodes = {head_node};
auto head_position = glm::vec3(0,0,0);
auto move_direction = glm::normalize(glm::vec3(0, 0, -1));

//GLuint texture_bump = bonobo::loadTexture2D("fieldstone_bump.png");
//GLuint texture_diffuse = bonobo::loadTexture2D("fieldstone_diffuse.png");

Snejk::Snejk(GLuint const* const shader, std::function<void (GLuint)> const& set_uniforms, bonobo::mesh_data const& shape) {

    _shader = shader;
    _set_uniforms = set_uniforms;
    _shape = &shape;

    head_node.set_program(_shader, _set_uniforms);
    head_node.set_geometry(*_shape);
    head_node.set_translation(head_position);
    //head_node.add_texture("normal_map", texture_bump, GL_TEXTURE_2D);
    //head_node.add_texture("diffuse_texture", texture_diffuse, GL_TEXTURE_2D);

    Node body_node = Node();
    body_node.set_geometry(*_shape);
    body_node.set_program(_shader, _set_uniforms);
    body_node.set_scaling(glm::vec3(0.4f));
    body_node.translate(glm::vec3(2, 0, 0));
}

void Snejk::render(glm::mat4 const& world_to_clip, const float delta_time) {
    head_position += move_direction * delta_time * move_speed;
    head_node.set_translation(head_position);

    head_node.render(world_to_clip, head_node.get_transform());
    //body_node.render(world_to_clip, body_node.get_transform());
}

void Snejk::handle_input(InputHandler inputHandler) {
    if (inputHandler.GetKeycodeState(GLFW_KEY_LEFT) & JUST_PRESSED){ //Turn left
        move_direction = glm::normalize(glm::rotateY(move_direction, 1.0f));
    }
}

glm::vec3 Snejk::get_position() {
    return head_position;
}

void Snejk::add_node(const float size_multiplier) {
//    auto new_node = Node();
//    new_node.set_geometry(_shape);
//    new_node.set_program(_shader, _uniforms);
//    new_node.set_scaling(glm::vec3(size_multiplier));
//    //new_node.translate(glm::vec3(2, 0, 0));
//    new_node.add_texture("normal_map", texture_bump, GL_TEXTURE_2D);
//    new_node.add_texture("diffuse_texture", texture_diffuse, GL_TEXTURE_2D);
}
