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
	auto T = glm::mat4{0, 1, 0, 0,
					-t, 0, t, 0,
					2*t, t-3, 3-2*t, -t,
					-t, 2-t, t-2, t};
	auto XT = T*X;
	return glm::vec3(XT[0]*p0[0] + XT[1]*p1[0] + XT[2]*p2[0] + XT[3]*p3[0],
			XT[0]*p0[1] + XT[1]*p1[1] + XT[2]*p2[1] + XT[3]*p3[1],
			XT[0]*p0[2] + XT[1]*p1[2] + XT[2]*p2[2] + XT[3]*p3[2]);
}
