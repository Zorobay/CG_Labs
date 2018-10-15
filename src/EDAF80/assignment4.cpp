#include "assignment4.hpp"

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
#include <external/imgui_impl_glfw_gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tinyfiledialogs.h>

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

edaf80::Assignment4::Assignment4() :
        mCamera(0.5f * glm::half_pi<float>(),
                static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
                0.01f, 1000.0f),
        inputHandler(), mWindowManager(), window(nullptr) {
    Log::View::Init();

    WindowManager::WindowDatum window_datum{inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0,
                                            0};

    window = mWindowManager.CreateWindow("EDAF80: Assignment 4", window_datum, config::msaa_rate);
    if (window == nullptr) {
        Log::View::Destroy();
        throw std::runtime_error("Failed to get a window: aborting!");
    }
}

edaf80::Assignment4::~Assignment4() {
    Log::View::Destroy();
}

void
edaf80::Assignment4::run() {
    // Set up the camera
    mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 18.0f, 18.0f));
    mCamera.mMouseSensitivity = 0.003f;
    mCamera.mMovementSpeed = 0.025;
    mCamera.mWorld.LookAt(glm::vec3(0, 0, 0));

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

    GLuint cube_map_shader = 0u;
    program_manager.CreateAndRegisterProgram({{ShaderType::vertex,   "EDAF80/cubemap.vert"},
                                              {ShaderType::fragment, "EDAF80/cubemap.frag"}},
                                             cube_map_shader);
    if (cube_map_shader == 0u) {
        LogError("Failed to load cubemap shader");
    }

    GLuint water_shader = 0u;
    program_manager.CreateAndRegisterProgram({{ShaderType::vertex,   "EDAF80/water.vert"},
                                              {ShaderType::fragment, "EDAF80/water.frag"}},
                                             water_shader);
    if (water_shader == 0u) {
        LogError("Failed to load water shader");
    }

    //
    // Todo: Load your geometry
    //
    auto quad_shape = parametric_shapes::createQuad(50, 50, 5u);
    auto sphere_shape = parametric_shapes::createSphere(20, 20, 100.0f);
    if (quad_shape.vao == 0u || sphere_shape.vao == 0) {
        LogError("Failed to retrieve the circle ring mesh");
        return;
    }

    // Create uniforms
    auto light_position = glm::vec3(-2.0f, 4.0f, 2.0f);
    auto const set_uniforms = [&light_position](GLuint program) {
        glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
    };

    // Create water uniforms
    auto t = 1.0f;
    auto camera_position = mCamera.mWorld.GetTranslation();
    auto color_deep = glm::vec4(0.0, 0.0, 0.1, 1.0);
    auto color_shallow = glm::vec4(0.0, 0.4, 0.4, 1.0);
    float wave_amp1 = 1.5;
    float wave_amp2 = 0.5;
    auto dir1 = glm::vec2(-1.0, 0.0);
    auto dir2 = glm::vec2(-0.7, 0.7);
    auto const water_uniforms = [&light_position, &t, &camera_position, &color_deep, &color_shallow, &wave_amp1, &wave_amp2](
            GLuint program) {
        glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
        glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
        glUniform1f(glGetUniformLocation(program, "t"), t);
        glUniform4fv(glGetUniformLocation(program, "color_deep"), 1, glm::value_ptr(color_deep));
        glUniform4fv(glGetUniformLocation(program, "color_shallow"), 1, glm::value_ptr(color_shallow));
        glUniform1f(glGetUniformLocation(program, "amp1"), wave_amp1);
        glUniform1f(glGetUniformLocation(program, "amp2"), wave_amp2);
    };
    // Load cube map and other textures
    std::string skybox = "cloudyhills";
    auto cube_map_id = bonobo::loadTextureCubeMap(skybox + "/posx.png", skybox + "/negx.png", skybox + "/posy.png",
                                                  skybox + "/negy.png", skybox + "/posz.png", skybox + "/negz.png",
                                                  true);
    std::string reflection = "cloudyhills";
    auto cloudy_hills_id = bonobo::loadTextureCubeMap(reflection + "/posx.png", reflection + "/negx.png",
                                                      reflection + "/posy.png",
                                                      reflection + "/negy.png", reflection + "/posz.png",
                                                      reflection + "/negz.png",
                                                      true);
    std::string waves_bump = "waves.png";
    auto waves_bump_id = bonobo::loadTexture2D(waves_bump);

    // Create nodes
    auto sea_node = Node();
    sea_node.set_geometry(quad_shape);
    sea_node.set_program(&water_shader, water_uniforms);
    sea_node.add_texture("reflection_cube_map", cloudy_hills_id, GL_TEXTURE_CUBE_MAP);
    sea_node.add_texture("bump_map", waves_bump_id, GL_TEXTURE_2D);
    sea_node.translate(glm::vec3(0, 0, 0));

    auto skybox_node = Node();
    skybox_node.set_geometry(sphere_shape);
    skybox_node.set_program(&cube_map_shader, set_uniforms);
    skybox_node.add_texture(skybox + "_cube_map", cube_map_id, GL_TEXTURE_CUBE_MAP);

    glEnable(GL_DEPTH_TEST);
    auto polygon_mode = polygon_mode_t::fill;

    // Enable face culling to improve performance:
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);
    //glCullFace(GL_BACK);


    f64 ddeltatime;
    size_t fpsSamples = 0;
    double nowTime, lastTime = GetTimeMilliseconds();
    double fpsNextTick = lastTime + 1000.0;

    bool show_logs = true;
    bool show_gui = true;
    bool shader_reload_failed = false;

    while (!glfwWindowShouldClose(window)) {
        nowTime = GetTimeMilliseconds();
        ddeltatime = nowTime - lastTime;
        if (nowTime > fpsNextTick) {
            fpsNextTick += 1000.0;
            fpsSamples = 0;
        }
        fpsSamples++;

        auto &io = ImGui::GetIO();
        inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);

        glfwPollEvents();
        inputHandler.Advance();
        mCamera.Update(ddeltatime, inputHandler);
        // Update camera position
        camera_position = mCamera.mWorld.GetTranslation();

        if (inputHandler.GetKeycodeState(GLFW_KEY_F3) & JUST_RELEASED)
            show_logs = !show_logs;
        if (inputHandler.GetKeycodeState(GLFW_KEY_F2) & JUST_RELEASED)
            show_gui = !show_gui;
        if (inputHandler.GetKeycodeState(GLFW_KEY_R) & JUST_PRESSED) {
            shader_reload_failed = !program_manager.ReloadAllPrograms();
            if (shader_reload_failed)
                tinyfd_notifyPopup("Shader Program Reload Error",
                                   "An error occurred while reloading shader programs; see the logs for details.\n"
                                   "Rendering is suspended until the issue is solved. Once fixed, just reload the shaders again.",
                                   "error");
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


        ImGui_ImplGlfwGL3_NewFrame();

        //
        // Todo: If you need to handle inputs, you can do it here
        //



        int framebuffer_width, framebuffer_height;
        glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
        glViewport(0, 0, framebuffer_width, framebuffer_height);
        glClearDepthf(1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        if (!shader_reload_failed) {
            //
            // Todo: Render all your geometry here.
            //
            sea_node.translate(glm::vec3());
            sea_node.render(mCamera.GetWorldToClipMatrix(), sea_node.get_transform());
            skybox_node.render(mCamera.GetWorldToClipMatrix(), skybox_node.get_transform());
        }

        // Add to t
        t += 0.001 * ddeltatime;


        //
        // Todo: If you want a custom ImGUI window, you can set it up
        //       here
        //

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        bool opened = ImGui::Begin("Scene Control", &opened, ImVec2(300, 100), -1.0f, 0);
        if (opened) {
            ImGui::ColorEdit4("Deep Color", glm::value_ptr(color_deep));
            ImGui::ColorEdit4("Shallow Color", glm::value_ptr(color_shallow));
            ImGui::SliderFloat("Amp 1", &wave_amp1, -1.0f, 10.0f);
            ImGui::SliderFloat("Amp 2", &wave_amp2, -1.0f, 10.0f);
        }
        ImGui::End();

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
        edaf80::Assignment4 assignment4;
        assignment4.run();
    } catch (std::runtime_error const &e) {
        LogError(e.what());
    }
    Bonobo::Destroy();
}
