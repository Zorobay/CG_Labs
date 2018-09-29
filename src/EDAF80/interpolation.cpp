#include "interpolation.hpp"
#include "vector"

glm::vec3
interpolation::evalLERP(glm::vec3 const& p0, glm::vec3 const& p1, float const x)
{
	return (1u-x)*p0 + x*p1;
}

glm::vec3
interpolation::evalCatmullRom(glm::vec3 const& p0, glm::vec3 const& p1,
							  glm::vec3 const& p2, glm::vec3 const& p3,
							  float const t, float const x)
{
	auto X = glm::vec4{1, x, pow(x, 2), pow(x, 3)};
	auto T = glm::mat4{{0, 1, 0, 0},
					   {-t, 0, t, 0},
					   {2*t, t-3, 3-2*t, -t},
					   {-t, 2-t, t-2, t}};
	auto P = glm::mat4x3{p0,p1,p2,p3};
	return P*T*X;
}