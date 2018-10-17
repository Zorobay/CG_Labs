#include <core/node.hpp>
#include <core/helpers.hpp>
#include "Snejk.h"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/geometric.hpp"
#include <cmath>

Node createSpecialFood(Snejk snek)
{
    auto food_node = Node();
    int kind = rand() % 100;
    std::string texture_img = "";
    if (kind < 5)
    {
        //redbull
        texture_img = "redsneek.png";
    }
    else if (kind < 10)
    {
        //confusion
        texture_img = "purpl.png";
    }
    else if (kind < 20)
    {
        //speed and points
        texture_img = "lightblue.png";
    }
    else
    {
        //normal
        texture_img = "sneeek.png";
    }
    food_node.add_texture("diffuse_texture", bonobo::loadTexture2D(texture_img), GL_TEXTURE_2D);

    return food_node;
}

Food::Food(bonobo::mesh_data const &shape, GLuint const *const program,
           std::function<void(GLuint)> const &set_uniforms, size_t amount,
           Snejk snek)
{
    std::random_device rd;      // Seed
    std::mt19937 ran_gen(rd()); // Random generator
    std::uniform_int_distribution<int> dist(-world_radi, world_radi);

    for (size_t i = 0; i < amount; i++)
    {
        float x_pos = dist(ran_gen);
        float z_pos = dist(ran_gen);

        while (glm::length(glm::vec3(x_pos, 0.0f, z_pos) - snek.get_position()) < 3)
        {
            x_pos = dist(ran_gen);
            z_pos = dist(ran_gen);
            std::cout << "snake was too close, " << x_pos << ", " << z_pos << " is new position\n";
        }

        auto food_node = createSpecialFood(snek);
        food_node.set_geometry(shape);
        food_node.set_scaling(glm::vec3(food_radi * 0.8));
        food_node.set_translation(glm::vec3(x_pos, 0.0f, z_pos));
        food_node.set_program(program, set_uniforms);
        food.push_back(food_node);
    }
}

void Food::erase(int i)
{
    
}