#include "assignment2.hpp"
#include "interpolation.hpp"
#include "parametric_shapes.hpp"

#include "config.hpp"
#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/Log.h"
#include "core/LogView.h"
#include "core/Misc.h"
#include "core/node.hpp"
#include "core/ShaderProgramManager.hpp"
#include <imgui.h>
#include "external/imgui_impl_glfw_gl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdlib>
#include <stdexcept>

enum class polygon_mode_t : unsigned int {
    fill = 0u,
    line,
    point
};

static polygon_mode_t get_next_mode(polygon_mode_t mode) {
    return static_cast<polygon_mode_t>((static_cast<unsigned int>(mode) + 1u) % 3u);
}

edaf80::Assignment2::Assignment2() :
        mCamera(0.5f * glm::half_pi<float>(),
                static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
                0.01f, 1000.0f),
        inputHandler(), mWindowManager(), window(nullptr) {
    Log::View::Init();

    WindowManager::WindowDatum window_datum{inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0,
                                            0};

    window = mWindowManager.CreateWindow("EDAF80: Assignment 2", window_datum, config::msaa_rate);
    if (window == nullptr) {
        Log::View::Destroy();
        throw std::runtime_error("Failed to get a window: aborting!");
    }
}

edaf80::Assignment2::~Assignment2() {
    Log::View::Destroy();
}

void
edaf80::Assignment2::run() {
    // Load the geometry
    auto const shape = parametric_shapes::createCircleRing(5u, 4u, 1.0f, 2.0f);
    auto const shape_quad = parametric_shapes::createQuad(2.0f, 2.0f);
    auto const shape_sphere = parametric_shapes::createSphere(12, 10, 2.0f);
    auto const shape_torus = parametric_shapes::createTorus(10.0f, 10.0f, 1.5f, 0.5f);
    if (shape.vao == 0u || shape_quad.vao == 0u || shape_sphere.vao == 0u || shape_torus.vao == 0u)
        return;

    // Set up the camera
    mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 0.0f, 6.0f));
    mCamera.mMouseSensitivity = 0.003f;
    mCamera.mMovementSpeed = 0.25f * 12.0f;

    // Create the shader programs
    ShaderProgramManager program_manager;
    GLuint fallback_shader = 0u;
    program_manager.CreateAndRegisterProgram({{ShaderType::vertex,   "EDAF80/fallback.vert"},
                                              {ShaderType::fragment, "EDAF80/fallback.frag"}},
                                             fallback_shader);
    if (fallback_shader == 0u) {
        LogError("Failed to load fallback shader");
        return;
    }

    GLuint diffuse_shader = 0u;
    program_manager.CreateAndRegisterProgram({{ShaderType::vertex,   "EDAF80/diffuse.vert"},
                                              {ShaderType::fragment, "EDAF80/diffuse.frag"}},
                                             diffuse_shader);
    if (diffuse_shader == 0u)
        LogError("Failed to load diffuse shader");

    GLuint normal_shader = 0u;
    program_manager.CreateAndRegisterProgram({{ShaderType::vertex,   "EDAF80/normal.vert"},
                                              {ShaderType::fragment, "EDAF80/normal.frag"}},
                                             normal_shader);
    if (normal_shader == 0u)
        LogError("Failed to load normal shader");

    GLuint texcoord_shader = 0u;
    program_manager.CreateAndRegisterProgram({{ShaderType::vertex,   "EDAF80/texcoord.vert"},
                                              {ShaderType::fragment, "EDAF80/texcoord.frag"}},
                                             texcoord_shader);
    if (texcoord_shader == 0u)
        LogError("Failed to load texcoord shader");

    auto const light_position = glm::vec3(-2.0f, 4.0f, 2.0f);
    auto const set_uniforms = [&light_position](GLuint program) {
        glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
    };

    // Set the default tensions value; it can always be changed at runtime
    // through the "Scene Controls" window.
    float catmull_rom_tension = 0.0f;

    // Set whether the default interpolation algorithm should be the linear one;
    // it can always be changed at runtime through the "Scene Controls" window.
    bool use_linear = true;

    auto circle_rings = Node();
    circle_rings.set_geometry(shape_sphere);
    circle_rings.set_program(&fallback_shader, set_uniforms);
    circle_rings.set_translation(glm::vec3(2, 0, 0));
    circle_rings.set_scaling(glm::vec3(0.5,0.5,0.5));

    //Create quad node
    Node node_quad;
    node_quad.set_geometry(shape_sphere);
    node_quad.set_program(&fallback_shader, set_uniforms);
    node_quad.set_translation(glm::vec3(0, 0, 0));
    //node_quad.set_scaling(glm::vec3(0.3, 0.3, 0.3));

    // Create sphere node
    Node node_sphere;
    node_sphere.set_geometry(shape_torus);
    node_sphere.set_program(&fallback_shader, set_uniforms);
    node_sphere.set_translation(glm::vec3(0, 0, 0));
    //node_sphere.set_scaling(glm::vec3(0.5, 0.5, 0.5));

    //! \todo Create a tesselated sphere and a tesselated torus
    // Create list of points to interpolate inbetween
    auto points = std::vector<glm::vec3>{glm::vec3(0, 0, 0),
                                         glm::vec3(1, 1, 0),
                                         glm::vec3(1, 1, 1),
                                         glm::vec3(-1, 1, 1),
                                         glm::vec3(-1, 0, 0),
                                         glm::vec3(-2, 0, -1),
                                         glm::vec3(-3, 2, 1),
                                         glm::vec3(-2, 1, 0),
                                         glm::vec3(-2, 0, 1),
                                         glm::vec3(-1, 0, 0.5)};

    int t_nbr = 50;
    float speed = 60.0f;
    int total_pts_nbr = t_nbr * points.size();
    auto lerps = std::vector<glm::vec3>(total_pts_nbr);

    int index = 0;
    for (int i = 0; i < points.size(); i++) {
        for (int j = 0; j < t_nbr; j++) {
            float t = j / static_cast<float>(t_nbr);
            lerps[index] = interpolation::evalLERP(points[i], points[(i + 1) % points.size()], t);
            index++;
        }
    }
    float lerps_idx = 0.0f;

    float catmull_idx = 0.0f;
    auto polygon_mode = polygon_mode_t::fill;

    glEnable(GL_DEPTH_TEST);

    // Enable face culling to improve performance
    glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);
    glCullFace(GL_BACK);


    f64 ddeltatime;
    size_t fpsSamples = 0;
    double nowTime, lastTime = GetTimeSeconds();
    double fpsNextTick = lastTime + 1.0;

    bool show_logs = true;
    bool show_gui = true;

    while (!glfwWindowShouldClose(window)) {
        nowTime = GetTimeSeconds();
        ddeltatime = nowTime - lastTime;
        if (nowTime > fpsNextTick) {
            fpsNextTick += 1.0;
            fpsSamples = 0;
        }
        fpsSamples++;

        auto &io = ImGui::GetIO();
        inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);

        glfwPollEvents();
        inputHandler.Advance();
        mCamera.Update(ddeltatime, inputHandler);

        if (inputHandler.GetKeycodeState(GLFW_KEY_F3) & JUST_RELEASED)
            show_logs = !show_logs;
        if (inputHandler.GetKeycodeState(GLFW_KEY_F2) & JUST_RELEASED)
            show_gui = !show_gui;
        if (inputHandler.GetKeycodeState(GLFW_KEY_R) & JUST_RELEASED) {
            catmull_idx = 0;
            lerps_idx = 0;
        }


        ImGui_ImplGlfwGL3_NewFrame();


        if (inputHandler.GetKeycodeState(GLFW_KEY_1) & JUST_PRESSED) {
            circle_rings.set_program(&fallback_shader, set_uniforms);
            node_quad.set_program(&fallback_shader, set_uniforms);
            node_sphere.set_program(&fallback_shader, set_uniforms);
        }
        if (inputHandler.GetKeycodeState(GLFW_KEY_2) & JUST_PRESSED) {
            circle_rings.set_program(&diffuse_shader, set_uniforms);
            node_quad.set_program(&diffuse_shader, set_uniforms);
            node_sphere.set_program(&diffuse_shader, set_uniforms);
        }
        if (inputHandler.GetKeycodeState(GLFW_KEY_3) & JUST_PRESSED) {
            circle_rings.set_program(&normal_shader, set_uniforms);
            node_quad.set_program(&normal_shader, set_uniforms);
            node_sphere.set_program(&normal_shader, set_uniforms);
        }
        if (inputHandler.GetKeycodeState(GLFW_KEY_4) & JUST_PRESSED) {
            circle_rings.set_program(&texcoord_shader, set_uniforms);
        }
        if (inputHandler.GetKeycodeState(GLFW_KEY_Z) & JUST_PRESSED) {
            polygon_mode = get_next_mode(polygon_mode);
        }

        switch (polygon_mode) {
            case polygon_mode_t::fill:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            case polygon_mode_t::line:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            case polygon_mode_t::point:
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                break;
        }

        //! \todo Interpolate the movement of a shape between various
        //!        control points
        int closest_pt_idx = std::floor(catmull_idx / static_cast<float>(t_nbr));
        float t = (catmull_idx - closest_pt_idx * t_nbr) / static_cast<float>(t_nbr);
        auto next_pos = interpolation::evalCatmullRom(points[(closest_pt_idx - 1) % points.size()], points[closest_pt_idx % points.size()], points[(closest_pt_idx + 1) % points.size()],
                                                        points[(closest_pt_idx + 2) % points.size()], catmull_rom_tension, t);
        node_quad.set_translation(next_pos);

        int idx_int = static_cast<int>(std::floor(lerps_idx)) % total_pts_nbr;
        node_sphere.set_translation(lerps[idx_int]);

        // Increase interpolation counters
        lerps_idx += ddeltatime * speed;
        catmull_idx += ddeltatime * speed;

        int framebuffer_width, framebuffer_height;
        glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
        glViewport(0, 0, framebuffer_width, framebuffer_height);
        glClearDepthf(1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        //circle_rings.render(mCamera.GetWorldToClipMatrix(), circle_rings.get_transform());
        node_quad.render(mCamera.GetWorldToClipMatrix(), node_quad.get_transform());
        node_sphere.render(mCamera.GetWorldToClipMatrix(), node_sphere.get_transform());

        bool const opened = ImGui::Begin("Scene Controls", nullptr, ImVec2(300, 100), -1.0f, 0);
        if (opened) {
            ImGui::SliderFloat("Catmull-Rom tension", &catmull_rom_tension, 0.0f, 5.0f);
            ImGui::SliderFloat("Speed", &speed, 0.0f, 200.0f);
            ImGui::Checkbox("Use linear interpolation", &use_linear);
        }
        ImGui::End();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        if (show_logs)
            Log::View::Render();
        if (show_gui)
            ImGui::Render();

        glfwSwapBuffers(window);
        lastTime = nowTime;
    }
}

int main() {
    Bonobo::Init();
    try {
        edaf80::Assignment2 assignment2;
        assignment2.run();
    } catch (std::runtime_error const &e) {
        LogError(e.what());
    }
    Bonobo::Destroy();
}
