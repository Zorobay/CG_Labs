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

// Contants
const float base_move_speed = 0.005;
const float turn_speed = 0.005;

class Snejk {
public:
    Snejk(GLuint const* const shader, std::function<void (GLuint)> const& set_uniforms, bonobo::mesh_data const& shape);
    void render(glm::mat4 const& world_to_clip, const float delta_time);
    void handle_input(InputHandler inputHandler);
    void add_node(const float size_multiplier);
    void add_node();
    glm::vec3 get_position();
    glm::vec3 get_move_direction();
    float get_rotation_y();
    float get_radius();



private:
    bool isAlive();
    GLuint const* _shader;
    std::function<void (GLuint)> _set_uniforms;
    bonobo::mesh_data _shape;
    GLuint _texture_bump = bonobo::loadTexture2D("fieldstone_bump.png");
    GLuint _texture_diffuse = bonobo::loadTexture2D("face.png");
    std::vector<Node> _nodes;
    std::vector<glm::vec3> _positions;
    std::vector<glm::vec3> _directions;
    size_t _counter = 0;
    float _rotation = glm::half_pi<float>();
    const float _tail_radi = 0.5f;
    const float _head_radi = 0.8f;
    size_t _tail_segment_offset = 30;
    float move_speed;
};



#endif //CG_LABS_SNEJK_H
