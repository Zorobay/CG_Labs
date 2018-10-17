#include <core/node.hpp>
#include <core/helpers.hpp>
#include "Snejk.h"
#include "Food.hpp"
#include <cmath>

Food::Food() {}

void Food::new_node(Node node)
{
    food_node = node;
};

void Food::render(glm::mat4 const &WVP, glm::mat4 const &world) const
{
    food_node.render(WVP, world);
};

glm::mat4x4 Food::get_transform() const
{
    return food_node.get_transform();
};

glm::vec3 Food::get_translation() const
{
    return food_node.get_translation();
};

int Food::kind() {
    return static_cast<int>(food_node.get_children_nb());
}