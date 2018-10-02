#include "interpolation.hpp"

glm::vec3
interpolation::evalLERP(glm::vec3 const& p0, glm::vec3 const& p1, float const t)
{
	auto T = glm::vec2(1, t);
	glm::vec2 c0 = glm::vec2(1, -1);
	glm::vec2 c1 = glm::vec2(0, 1);
	auto M = glm::mat2(c0, c1);

	auto TM = T*M;
	return TM[0] * p0 + TM[1] * p1;
}

glm::vec3
interpolation::evalCatmullRom(glm::vec3 const& p0, glm::vec3 const& p1,
                              glm::vec3 const& p2, glm::vec3 const& p3,
                              float const tau, float const t)
{

    auto T = glm::vec4(1, t, std::pow(t, 2), std::pow(t, 3));
    auto c0 = glm::vec4(0, -tau, 2 * tau, -tau);
    auto c1 = glm::vec4(1, 0, tau - 3, 2 - tau);
    auto c2 = glm::vec4(0, tau, 3 - 2*tau, tau - 2);
    auto c3 = glm::vec4(0, 0, -tau, tau);
    auto M = glm::mat4(c0, c1, c2, c3);
    glm::vec4 TM = T*M;
    auto sum = TM[0]*p0 + TM[1]*p1 + TM[2]*p2 + TM[3]*p3;

	return sum;
}
