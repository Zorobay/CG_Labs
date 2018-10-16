#include "assignment5.hpp"

#include "config.hpp"
#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/helpers.hpp"
#include "core/Log.h"
#include "core/LogView.h"
#include "core/Misc.h"
#include "core/ShaderProgramManager.hpp"
#include "Snejk.h"
#include "parametric_shapes.hpp"

#include <imgui.h>
#include <external/imgui_impl_glfw_gl3.h>
#include <tinyfiledialogs.h>

#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>

#include <random>

edaf80::Assignment5::Assignment5() :
        mCamera(0.5f * glm::half_pi<float>(),
                static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
                0.01f, 1000.0f),
        inputHandler(), mWindowManager(), window(nullptr) {
    Log::View::Init();

    WindowManager::WindowDatum window_datum{inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0,
                                            0};

    window = mWindowManager.CreateWindow("EDAF80: Assignment 5", window_datum, config::msaa_rate);
    if (window == nullptr) {
        Log::View::Destroy();
        throw std::runtime_error("Failed to get a window: aborting!");
    }
}

edaf80::Assignment5::~Assignment5() {
    Log::View::Destroy();
}

void edaf80::Assignment5::generate_food(bonobo::mesh_data const &shape, GLuint const *const program,
                                        std::function<void(GLuint)> const &set_uniforms, size_t amount) {
    std::random_device rd; // Seed
    std::mt19937 ran_gen(rd()); // Random generator
    std::uniform_int_distribution<int> dist(-world_radi, world_radi);

    for (size_t i = 0; i < amount; i++) {
        float x_pos = dist(ran_gen);
        std::cout << x_pos;
        float z_pos = dist(ran_gen);
        std::cout << ", " << z_pos << "\n";

        auto food_node = Node();
        food_node.set_translation(glm::vec3(x_pos, 0.0f, z_pos));
        food_node.set_scaling(glm::vec3(food_radi));
        food_node.set_geometry(shape);
        food_node.set_program(program, set_uniforms);
        food.push_back(food_node);
    }
}

void
edaf80::Assignment5::run() {

    // Load the sphere geometry
    auto sphere_shape = parametric_shapes::createSphere(20, 20, 1.0f);
    if (sphere_shape.vao == 0u) {
        LogError("Failed to retrieve the circle ring mesh");
        return;
    }
    // Set up the camera
    mCamera.mWorld.SetTranslate(glm::vec3(0.0f, camera_y_disp, camera_z_disp));
    mCamera.mMouseSensitivity = 0.003f;
    mCamera.mMovementSpeed = 0.025;

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

    GLuint default_shader = 0u;
    program_manager.CreateAndRegisterProgram({{ShaderType::vertex,   "EDAF80/default.vert"},
                                              {ShaderType::fragment, "EDAF80/default.frag"}},
                                             default_shader);
    if (default_shader == 0u)
        LogError("Failed to load diffuse shader");

    GLuint cube_map_shader = 0u;
    program_manager.CreateAndRegisterProgram({{ShaderType::vertex,   "EDAF80/cubemap.vert"},
                                              {ShaderType::fragment, "EDAF80/cubemap.frag"}},
                                             cube_map_shader);
    if (cube_map_shader == 0u) {
        LogError("Failed to load cubemap shader");
    }

    GLuint blinn_phong_normal_shader = 0u;
    program_manager.CreateAndRegisterProgram({{ShaderType::vertex,   "EDAF80/blinn_phon_normal.vert"},
                                              {ShaderType::fragment, "EDAF80/blinn_phong_normal.frag"}},
                                             blinn_phong_normal_shader);
    if (blinn_phong_normal_shader == 0u) {
        LogError("Failed to load normal map shader");
    }

    auto light_position = glm::vec3(-2.0f, 4.0f, 2.0f);
    auto const set_uniforms = [&light_position](GLuint program) {
        glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
    };

    // Blinn phong uniforms
    auto camera_position = mCamera.mWorld.GetTranslation();
    auto ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    auto diffuse = glm::vec3(0.7f, 0.2f, 0.4f);
    auto specular = glm::vec3(1.0f, 1.0f, 1.0f);
    auto shininess = 1.0f;
    auto const phong_set_uniforms = [&light_position, &camera_position, &ambient, &diffuse, &specular, &shininess](
            GLuint program) {
        glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
        glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
        glUniform3fv(glGetUniformLocation(program, "ambient"), 1, glm::value_ptr(ambient));
        glUniform3fv(glGetUniformLocation(program, "diffuse"), 1, glm::value_ptr(diffuse));
        glUniform3fv(glGetUniformLocation(program, "specular"), 1, glm::value_ptr(specular));
        glUniform1f(glGetUniformLocation(program, "shininess"), shininess);
    };


    // Create snake
    auto snake = Snejk(&default_shader, phong_set_uniforms, sphere_shape);
    auto node = Node();
    node.set_geometry(sphere_shape);
    node.set_program(&fallback_shader, set_uniforms);
    node.translate(glm::vec3(-3, 0, 0));
    node.set_scaling(glm::vec3(0.5f));

    // Create food nodes
    generate_food(sphere_shape, &fallback_shader, set_uniforms, 20);

    // Create skybox
    std::string skybox = "opensea";
    auto cube_map_id = bonobo::loadTextureCubeMap(skybox + "/posx.png", skybox + "/negx.png", skybox + "/posy.png",
                                                  skybox + "/negy.png", skybox + "/posz.png", skybox + "/negz.png",
                                                  true);
    auto skybox_node = Node();
    skybox_node.set_geometry(sphere_shape);
    skybox_node.set_program(&cube_map_shader, set_uniforms);
    skybox_node.set_scaling(glm::vec3(100.0f));
    skybox_node.add_texture(skybox + "_cube_map", cube_map_id, GL_TEXTURE_CUBE_MAP);


    glEnable(GL_DEPTH_TEST);

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

        ImGui_ImplGlfwGL3_NewFrame();

        //
        // Todo: If you need to handle inputs, you can do it here
        //
        snake.handle_input(inputHandler);

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
            skybox_node.render(mCamera.GetWorldToClipMatrix(), skybox_node.get_transform());
            //node.render(mCamera.GetWorldToClipMatrix(), node.get_transform());
            snake.render(mCamera.GetWorldToClipMatrix(), ddeltatime);

            mCamera.mWorld.SetTranslate(snake.get_position() + glm::vec3(10,8,10));//glm::vec3(-snake.get_move_direction().x*10, camera_y_disp, -snake.get_move_direction().z*10));
            mCamera.mWorld.LookAt(snake.get_position());

            // Render food
            for (size_t i = 0; i < food.size(); i++) {
                Node f = food[i];
                // If food is eaten, remove it and make snake longer
                if (food_radi + snake.get_radius() > glm::distance(f.get_translation(), snake.get_position())){
                    snake.add_node();
                    food.erase(food.begin() + i);
                }else{
                    f.render(mCamera.GetWorldToClipMatrix(), f.get_transform());
                }
            }
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        //
        // Todo: If you want a custom ImGUI window, you can set it up
        //       here
        //

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
        edaf80::Assignment5 assignment5;
        assignment5.run();
    } catch (std::runtime_error const &e) {
        LogError(e.what());
    }
    Bonobo::Destroy();
}