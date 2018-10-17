#include <core/node.hpp>
#include <core/helpers.hpp>
#include "Snejk.h"
#include "Food.hpp"
#include <cmath>

Food::Food() {}

void Food::new_node(Node node, Snejk const &snejk, std::string type)
{
    food_node = node;
    food_snejk = snejk;
    my_type = type;
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

void Food::eatenEffect() {
    switch(my_type){
        case "LIT": food_snejk.speed_up();food_snejk.speed_up();food_snejk.speed_up();food_snejk.speed_up();food_snejk.speed_up();food_snejk.speed_up();food_snejk.speed_up();food_snejk.speed_up();food_snejk.speed_up();food_snejk.speed_up();food_snejk.speed_up();food_snejk.speed_up();food_snejk.speed_up();break;
        default: break;
    }
}

~Food::Food(){
    ~food_node;
    ~my_type(); 
} 