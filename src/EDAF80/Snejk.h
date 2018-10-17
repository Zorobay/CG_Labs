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
#include <ctime>
#include <cmath>



class Snejk {
public:
    Snejk(GLuint const* const shader, std::function<void (GLuint)> const& set_uniforms, bonobo::mesh_data const& shape, float world_radi);
    void render(glm::mat4 const& world_to_clip, const float delta_time);
    void handle_input(InputHandler inputHandler);
    void add_node(const float size_multiplier);
    void add_node();
    glm::vec3 get_position();
    glm::vec3 get_move_direction();
    float get_rotation_y();
    float get_radius();
    bool is_alive();
    void speed_up();
    int get_points();
    void add_points(int p);
    void disable_movement();

private:
    int points = 0;
    GLuint const* _shader;
    std::function<void (GLuint)> _set_uniforms;
    bonobo::mesh_data _shape;
    GLuint _texture_bump = bonobo::loadTexture2D("fieldstone_bump.png");
    GLuint _texture_diffuse = bonobo::loadTexture2D("bluesnek.png");
    std::vector<Node> _nodes;
    std::vector<glm::vec3> _positions;
    std::vector<glm::vec3> _directions;
    std::vector<float> _rotations;
    size_t _counter = 0;
    float _rotation = glm::half_pi<float>();
    const float _tail_radi = 0.5f;
    const float _head_radi = 0.8f;
    size_t _tail_segment_offset = 20;
    float move_speed;
    float turn_speed;
    const float base_move_speed = 0.005;
    const float base_turn_speed = 0.006;
    const float move_speed_factor = 0.0001;
    const float turn_speed_factor = 0.00003;
    const float segment_distance = 1.7f;
    float _world_radi;

//    float speed_up_timer = 0.0f;
//    const float max_speed = base_move_speed * 2.0f;
//    bool is_sped_up = false;
};



#endif //CG_LABS_SNEJK_H
