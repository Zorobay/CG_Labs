#include "parametric_shapes.hpp"
#include "core/Log.h"

#include <glm/glm.hpp>

#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

bonobo::mesh_data
parametric_shapes::createQuad(unsigned int width, unsigned int height, unsigned int resolution) {

    //auto const resolution = 50u;
    auto const vertices_nb = resolution * resolution;
    auto vertices = std::vector<glm::vec3>(vertices_nb);
    auto tangents = std::vector<glm::vec3>(vertices_nb);
    auto binormals = std::vector<glm::vec3>(vertices_nb);
    auto normals = std::vector<glm::vec3>(vertices_nb);
    auto texcoords = std::vector<glm::vec3>(vertices_nb);

    auto const width_step = static_cast<float>(width) / resolution;
    auto const height_step = static_cast<float>(height) / resolution;

    size_t index = 0;
    for (size_t row = 0; row < resolution; row++) {
        for (size_t col = 0; col < resolution; col++) {
            vertices[index] = glm::vec3(col * width_step - (static_cast<float>(width) / 2), 0,
                                        row * height_step - (static_cast<float>(height) / 2));
//            vertices[index] = glm::vec3(col * width_step, 0,
//                                        row * height_step);

            // Tangent
            tangents[index] = glm::vec3(1, 0, 0);

            // Binormal
            binormals[index] = glm::vec3(0, 0, 1);

            // Normal
            normals[index] = glm::vec3(0, 1, 0);

            // Texcoords
            texcoords[index] = glm::vec3(static_cast<float>(row * height_step) / height,
                                         static_cast<float>(col * width_step) / width, 0);

            index++;
        }
    }

    auto indices = std::vector<glm::uvec3>((vertices_nb - 1) * (vertices_nb - 1) * 2);
    index = 0;
    for (size_t row = 0; row < resolution - 1; row++) {
        for (size_t col = 0; col < resolution - 1; col++) {
            indices[index] = glm::uvec3(resolution * row + col, resolution * row + col + 1,
                                        resolution * (row + 1) + col);

            index++;

            indices[index] = glm::uvec3(resolution * row + col + 1, resolution * (row + 1) + col + 1,
                                        resolution * (row + 1) + col);
            index++;
        }
    }
    bonobo::mesh_data data;
    glGenVertexArrays(1, &data.vao);
    assert(data.vao != 0u);
    glBindVertexArray(data.vao);

    auto const vertices_offset = 0u;
    auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
    auto const normals_offset = vertices_size;
    auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
    auto const texcoords_offset = normals_offset + normals_size;
    auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
    auto const tangents_offset = texcoords_offset + texcoords_size;
    auto const tangents_size = static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3));
    auto const binormals_offset = tangents_offset + tangents_size;
    auto const binormals_size = static_cast<GLsizeiptr>(binormals.size() * sizeof(glm::vec3));
    auto const bo_size = static_cast<GLsizeiptr>(vertices_size
                                                 + normals_size
                                                 + texcoords_size
                                                 + tangents_size
                                                 + binormals_size
    );

    glGenBuffers(1, &data.bo);
    assert(data.bo != 0u);
    glBindBuffer(GL_ARRAY_BUFFER, data.bo);
    glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const *>(vertices.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(0x0));

    glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const *>(normals.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(normals_offset));

    glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const *>(texcoords.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(texcoords_offset));

    glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, tangents_size, static_cast<GLvoid const *>(tangents.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::tangents));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::tangents), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(tangents_offset));

    glBufferSubData(GL_ARRAY_BUFFER, binormals_offset, binormals_size, static_cast<GLvoid const *>(binormals.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::binormals));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::binormals), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(binormals_offset));

    glBindBuffer(GL_ARRAY_BUFFER, 0u);

    data.indices_nb = indices.size() * 3u;
    glGenBuffers(1, &data.ibo);
    assert(data.ibo != 0u);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size()) * sizeof(glm::uvec3),
                 reinterpret_cast<GLvoid const *>(indices.data()), GL_STATIC_DRAW);

    glBindVertexArray(0u);
    //glBindBuffer(GL_ARRAY_BUFFER, 0u);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

    return data;
}

bonobo::mesh_data
parametric_shapes::createSphere(unsigned int const res_theta,
                                unsigned int const res_phi, float const radius) {

    auto const vertices_nb = res_theta * res_phi;
    auto vertices = std::vector<glm::vec3>(vertices_nb);
    auto tangents = std::vector<glm::vec3>(vertices_nb);
    auto binormals = std::vector<glm::vec3>(vertices_nb);
    auto normals = std::vector<glm::vec3>(vertices_nb);
    auto texcoords = std::vector<glm::vec3>(vertices_nb);

    float theta = 0.0f,                                                           // 'stepping'-variable for theta: will go 0 - 2PI
            dtheta =
            glm::two_pi<float>() / (static_cast<float>(res_theta) - 1.0f); // step size, depending on the resolution

    float phi = 0.0f,
            dphi = glm::pi<float>() / (static_cast<float>(res_phi) - 1.0f); //phi will go from 0 to pi


    // Generate vertices
    size_t index = 0u;

    for (unsigned int i = 0u; i < res_theta; ++i) {
        float cos_theta = std::cos(theta);
        float sin_theta = std::sin(theta);

        phi = 0.0f;

        for (unsigned int j = 0u; j < res_phi; ++j) {
            float sin_phi = std::sin(phi),
                    cos_phi = std::cos(phi);

            vertices[index] = glm::vec3(radius * sin_theta * sin_phi,
                                        -radius * cos_phi,
                                        radius * cos_theta * sin_phi);

            // texture coordinate
            texcoords[index] = glm::vec3(static_cast<float>(j) / (static_cast<float>(res_phi) - 1.0f),
                                         static_cast<float>(i) / (static_cast<float>(res_theta) - 1.0f),
                                         0.0f);

            // tangent
            auto t = glm::vec3(radius * cos_theta,
                               0,
                               -radius * sin_theta);
            t = glm::normalize(t);
            tangents[index] = t;

            // binormal
            auto b = glm::vec3(radius * sin_theta * cos_phi,
                               radius * sin_phi,
                               radius * cos_theta * cos_phi);
            b = glm::normalize(b);
            binormals[index] = b;

            // normal
            auto const n = glm::cross(t, b);
            normals[index] = n;

            phi += dphi;
            ++index;
        }

        theta += dtheta;
    }


    auto indices = std::vector<glm::uvec3>(2u * (res_theta - 1u) * (res_phi - 1u));

    // Calculate indices
    index = 0u;
    for (unsigned int i = 0u; i < res_theta - 1; ++i) {
        for (unsigned int j = 0u; j < res_phi - 1; ++j) {
            // first triangle
            indices[index] = glm::uvec3(res_phi * i + j,
                                        res_phi * i + j + 1u + res_phi,
                                        res_phi * i + j + 1u);

            ++index;

            //second triangle
            indices[index] = glm::uvec3(res_phi * i + j,
                                        res_phi * i + j + res_phi,
                                        res_phi * i + j + res_phi + 1u);
            ++index;
        }
    }

    bonobo::mesh_data data;
    glGenVertexArrays(1, &data.vao);
    assert(data.vao != 0u);
    glBindVertexArray(data.vao);

    auto const vertices_offset = 0u;
    auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
    auto const normals_offset = vertices_size;
    auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
    auto const texcoords_offset = normals_offset + normals_size;
    auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
    auto const tangents_offset = texcoords_offset + texcoords_size;
    auto const tangents_size = static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3));
    auto const binormals_offset = tangents_offset + tangents_size;
    auto const binormals_size = static_cast<GLsizeiptr>(binormals.size() * sizeof(glm::vec3));
    auto const bo_size = static_cast<GLsizeiptr>(vertices_size
                                                 + normals_size
                                                 + texcoords_size
                                                 + tangents_size
                                                 + binormals_size
    );

    glGenBuffers(1, &data.bo);
    assert(data.bo != 0u);
    glBindBuffer(GL_ARRAY_BUFFER, data.bo);
    glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const *>(vertices.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(0x0));

    glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const *>(normals.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(normals_offset));

    glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const *>(texcoords.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(texcoords_offset));

    glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, tangents_size, static_cast<GLvoid const *>(tangents.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::tangents));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::tangents), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(tangents_offset));

    glBufferSubData(GL_ARRAY_BUFFER, binormals_offset, binormals_size, static_cast<GLvoid const *>(binormals.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::binormals));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::binormals), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(binormals_offset));

    glBindBuffer(GL_ARRAY_BUFFER, 0u);

    data.indices_nb = indices.size() * 3u;
    glGenBuffers(1, &data.ibo);
    assert(data.ibo != 0u);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size()) * sizeof(glm::uvec3),
                 reinterpret_cast<GLvoid const *>(indices.data()), GL_STATIC_DRAW);

    glBindVertexArray(0u);
    //glBindBuffer(GL_ARRAY_BUFFER, 0u);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

    return data;
}

bonobo::mesh_data
parametric_shapes::createTorus(unsigned int const res_theta,
                               unsigned int const res_phi, float const rA,
                               float const rB) {

    //! \todo (Optional) Implement this function
    auto const vertices_nb = res_theta * res_phi;
    auto vertices = std::vector<glm::vec3>(vertices_nb);
    auto tangents = std::vector<glm::vec3>(vertices_nb);
    auto binormals = std::vector<glm::vec3>(vertices_nb);
    auto normals = std::vector<glm::vec3>(vertices_nb);
    // auto texcoords = std::vector<glm::vec3>(vertices_nb);

    float theta = 0.0f,                                                           // 'stepping'-variable for theta: will go 0 - 2PI
            dtheta =
            glm::two_pi<float>() / (static_cast<float>(res_theta) - 1.0f); // step size, depending on the resolution

    float phi = 0.0f,
            dphi = glm::two_pi<float>() / (static_cast<float>(res_phi) - 1.0f); //phi will go from 0 to pi

    // Generate vertices
    size_t index = 0u;

    for (unsigned int i = 0u; i < res_theta; ++i) {
        float cos_theta = std::cos(theta);
        float sin_theta = std::sin(theta);

        phi = 0.0f;

        for (unsigned int j = 0u; j < res_phi; ++j) {
            float sin_phi = std::sin(phi),
                    cos_phi = std::cos(phi);

            vertices[index] = glm::vec3((rA + rB * cos_theta) * cos_phi,
                                        (rA + rB * cos_theta) * sin_phi,
                                        -rB * sin_theta);

            // texture coordinate
//            texcoords[index] = glm::vec3(static_cast<float>(j) / (static_cast<float>(res_phi) - 1.0f),
//                                         static_cast<float>(i) / (static_cast<float>(res_theta) - 1.0f),
//                                         0.0f);

            // tangent
            auto t = glm::vec3(-rB * sin_theta * cos_phi,
                               -rB * sin_theta * sin_phi,
                               -rB * cos_theta);
            t = glm::normalize(t);
            tangents[index] = t;

            // binormal
            auto b = glm::vec3(-(rA + rB * cos_theta) * sin_phi,
                               (rA + rB * cos_theta) * cos_phi,
                               0);
            b = glm::normalize(b);
            binormals[index] = b;

            // normal
            auto const n = glm::cross(t, b);
            normals[index] = n;

            phi += dphi;
            ++index;
        }

        theta += dtheta;
    }


    auto indices = std::vector<glm::uvec3>(2u * (res_theta - 1u) * (res_phi - 1u));

    // Calculate indices
    index = 0u;
    for (unsigned int i = 0u; i < res_theta - 1; ++i) {
        for (unsigned int j = 0u; j < res_phi - 1; ++j) {
            // first triangle
            indices[index] = glm::uvec3(res_phi * i + j,
                                        res_phi * i + j + 1u,
                                        res_phi * i + j + 1u + res_phi);

            ++index;

            //second triangle
            indices[index] = glm::uvec3(res_phi * i + j,
                                        res_phi * i + j + res_phi + 1u,
                                        res_phi * i + j + res_phi);
            ++index;
        }
    }

    bonobo::mesh_data data;
    glGenVertexArrays(1, &data.vao);
    assert(data.vao != 0u);
    glBindVertexArray(data.vao);

    auto const vertices_offset = 0u;
    auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
    auto const normals_offset = vertices_size;
    auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
//    auto const texcoords_offset = normals_offset + normals_size;
//    auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
//    auto const tangents_offset = texcoords_offset + texcoords_size;
//    auto const tangents_size = static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3));
//    auto const binormals_offset = tangents_offset + tangents_size;
//    auto const binormals_size = static_cast<GLsizeiptr>(binormals.size() * sizeof(glm::vec3));
    auto const bo_size = static_cast<GLsizeiptr>(vertices_size
                                                 + normals_size);
//                                                 + texcoords_size
//                                                 + tangents_size
//                                                 + binormals_size
//    );

    glGenBuffers(1, &data.bo);
    assert(data.bo != 0u);
    glBindBuffer(GL_ARRAY_BUFFER, data.bo);
    glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const *>(vertices.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(0x0));

    glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const *>(normals.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(normals_offset));

//    glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const *>(texcoords.data()));
//    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
//    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0,
//                          reinterpret_cast<GLvoid const *>(texcoords_offset));
//
//    glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, tangents_size, static_cast<GLvoid const *>(tangents.data()));
//    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::tangents));
//    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::tangents), 3, GL_FLOAT, GL_FALSE, 0,
//                          reinterpret_cast<GLvoid const *>(tangents_offset));
//
//    glBufferSubData(GL_ARRAY_BUFFER, binormals_offset, binormals_size, static_cast<GLvoid const *>(binormals.data()));
//    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::binormals));
//    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::binormals), 3, GL_FLOAT, GL_FALSE, 0,
//                          reinterpret_cast<GLvoid const *>(binormals_offset));

    glBindBuffer(GL_ARRAY_BUFFER, 0u);

    data.indices_nb = indices.size() * 3u;
    glGenBuffers(1, &data.ibo);
    assert(data.ibo != 0u);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size()) * sizeof(glm::uvec3),
                 reinterpret_cast<GLvoid const *>(indices.data()), GL_STATIC_DRAW);

    glBindVertexArray(0u);
    //glBindBuffer(GL_ARRAY_BUFFER, 0u);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

    return data;
}

bonobo::mesh_data
parametric_shapes::createCircleRing(unsigned int const res_radius,
                                    unsigned int const res_theta,
                                    float const inner_radius,
                                    float const outer_radius) {
    auto const vertices_nb = res_radius * res_theta;

    auto vertices = std::vector<glm::vec3>(vertices_nb);
    auto normals = std::vector<glm::vec3>(vertices_nb);
    auto texcoords = std::vector<glm::vec3>(vertices_nb);
    auto tangents = std::vector<glm::vec3>(vertices_nb);
    auto binormals = std::vector<glm::vec3>(vertices_nb);

    float theta = 0.0f,                                                           // 'stepping'-variable for theta: will go 0 - 2PI
            dtheta =
            glm::two_pi<float>() / (static_cast<float>(res_theta) - 1.0f); // step size, depending on the resolution

    float radius = 0.0f,                                                                     // 'stepping'-variable for radius: will go inner_radius - outer_radius
            dradius = (outer_radius - inner_radius) /
                      (static_cast<float>(res_radius) - 1.0f); // step size, depending on the resolution

    // generate vertices iteratively
    size_t index = 0u;
    for (unsigned int i = 0u; i < res_theta; ++i) {
        float cos_theta = std::cos(theta),
                sin_theta = std::sin(theta);

        radius = inner_radius;

        for (unsigned int j = 0u; j < res_radius; ++j) {
            // vertex
            vertices[index] = glm::vec3(radius * cos_theta,
                                        radius * sin_theta,
                                        0.0f);

            // texture coordinates
            texcoords[index] = glm::vec3(static_cast<float>(j) / (static_cast<float>(res_radius) - 1.0f),
                                         static_cast<float>(i) / (static_cast<float>(res_theta) - 1.0f),
                                         0.0f);

            // tangent
            auto t = glm::vec3(cos_theta, sin_theta, 0.0f);
            t = glm::normalize(t);
            tangents[index] = t;

            // binormal
            auto b = glm::vec3(-sin_theta, cos_theta, 0.0f);
            b = glm::normalize(b);
            binormals[index] = b;

            // normal
            auto const n = glm::cross(t, b);
            normals[index] = n;

            radius += dradius;
            ++index;
        }

        theta += dtheta;
    }

    // create index array
    auto indices = std::vector<glm::uvec3>(2u * (res_theta - 1u) * (res_radius - 1u));

    // generate indices iteratively
    index = 0u;
    for (unsigned int i = 0u; i < res_theta - 1u; ++i) {
        for (unsigned int j = 0u; j < res_radius - 1u; ++j) {
            indices[index] = glm::uvec3(res_radius * i + j,
                                        res_radius * i + j + 1u,
                                        res_radius * i + j + 1u + res_radius);
            ++index;

            indices[index] = glm::uvec3(res_radius * i + j,
                                        res_radius * i + j + res_radius + 1u,
                                        res_radius * i + j + res_radius);
            ++index;
        }
    }

    bonobo::mesh_data data;
    glGenVertexArrays(1, &data.vao);
    assert(data.vao != 0u);
    glBindVertexArray(data.vao);

    auto const vertices_offset = 0u;
    auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
    auto const normals_offset = vertices_size;
    auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
    auto const texcoords_offset = normals_offset + normals_size;
    auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
    auto const tangents_offset = texcoords_offset + texcoords_size;
    auto const tangents_size = static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3));
    auto const binormals_offset = tangents_offset + tangents_size;
    auto const binormals_size = static_cast<GLsizeiptr>(binormals.size() * sizeof(glm::vec3));
    auto const bo_size = static_cast<GLsizeiptr>(vertices_size
                                                 + normals_size
                                                 + texcoords_size
                                                 + tangents_size
                                                 + binormals_size
    );
    glGenBuffers(1, &data.bo);
    assert(data.bo != 0u);
    glBindBuffer(GL_ARRAY_BUFFER, data.bo);
    glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const *>(vertices.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(0x0));

    glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const *>(normals.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(normals_offset));

    glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const *>(texcoords.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(texcoords_offset));

    glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, tangents_size, static_cast<GLvoid const *>(tangents.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::tangents));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::tangents), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(tangents_offset));

    glBufferSubData(GL_ARRAY_BUFFER, binormals_offset, binormals_size, static_cast<GLvoid const *>(binormals.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::binormals));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::binormals), 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid const *>(binormals_offset));

    glBindBuffer(GL_ARRAY_BUFFER, 0u);

    data.indices_nb = indices.size() * 3u;
    glGenBuffers(1, &data.ibo);
    assert(data.ibo != 0u);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(glm::uvec3)),
                 reinterpret_cast<GLvoid const *>(indices.data()), GL_STATIC_DRAW);

    glBindVertexArray(0u);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

    return data;
}
