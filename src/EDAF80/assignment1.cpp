#include "config.hpp"
#include "core/FPSCamera.h"
#include "core/helpers.hpp"
#include "core/Log.h"
#include "core/LogView.h"
#include "core/Misc.h"
#include "core/node.hpp"
#include "core/ShaderProgramManager.hpp"
#include "core/WindowManager.hpp"

#include <imgui.h>
#include <external/imgui_impl_glfw_gl3.h>

#include <stack>

#include <cstdlib>


int main() {
    //
    // Set up the logging system
    //
    Log::Init();
    Log::View::Init();

    //
    // Set up the camera
    //
    InputHandler input_handler;
    FPSCameraf camera(0.5f * glm::half_pi<float>(),
                      static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
                      0.01f, 1000.0f);
    camera.mWorld.SetTranslate(glm::vec3(-6.0f, 6.0f, 6.0f));
    camera.mWorld.LookAt(glm::vec3(0, 0, 0));
    camera.mMouseSensitivity = 0.003f;
    camera.mMovementSpeed = 0.25f * 12.0f;

    //
    // Set up the windowing system and create the window
    //
    WindowManager window_manager;
    WindowManager::WindowDatum window_datum{input_handler, camera, config::resolution_x, config::resolution_y, 0, 0, 0,
                                            0};
    GLFWwindow *window = window_manager.CreateWindow("EDAF80: Assignment 1", window_datum, config::msaa_rate);
    if (window == nullptr) {
        LogError("Failed to get a window: exiting.");

        Log::View::Destroy();
        Log::Destroy();

        return EXIT_FAILURE;
    }



    //
    // Load the sphere geometry
    //
    std::vector<bonobo::mesh_data> objects = bonobo::loadObjects("sphere.obj");
    if (objects.empty()) {
        LogError("Failed to load the sphere geometry: exiting.");

        Log::View::Destroy();
        Log::Destroy();

        return EXIT_FAILURE;
    }
    bonobo::mesh_data const &sphere = objects.front();
    objects = bonobo::loadObjects("ring.obj");

    if (objects.empty()) {
        LogError("Failed to load ring geometry. Exiting!");

        Log::View::Destroy();
        Log::Destroy();

        return EXIT_FAILURE;
    }
    bonobo::mesh_data const &ring = objects.front();


    //
    // Create the shader program
    //
    ShaderProgramManager program_manager;
    GLuint shader = 0u;
    program_manager.CreateAndRegisterProgram({{ShaderType::vertex,   "EDAF80/default.vert"},
                                              {ShaderType::fragment, "EDAF80/default.frag"}},
                                             shader);
    if (shader == 0u) {
        LogError("Failed to generate the shader program: exiting.");

        Log::View::Destroy();
        Log::Destroy();

        return EXIT_FAILURE;
    }


    //
    // Set up the sun node and other related attributes
    //
    float speed_param = 1.0f;
    Node solar_system_node;

    Node sun_node;
    sun_node.set_geometry(sphere);
    GLuint const sun_texture = bonobo::loadTexture2D("sunmap.png");
    sun_node.add_texture("diffuse_texture", sun_texture, GL_TEXTURE_2D);
    sun_node.set_scaling(glm::vec3(1, 1, 1));
    float const sun_spin_speed = glm::two_pi<float>() / 25.0f;

    Node mercury_node;
    Node mercury_pivot;
    mercury_node.set_geometry(sphere);
    GLuint const mercury_texture = bonobo::loadTexture2D("mercurymap.png");
    mercury_node.add_texture("diffuse_texture", mercury_texture, GL_TEXTURE_2D);
    float const mercury_spin_speed = glm::two_pi<float>() / 58.0f;
    float const mercury_pivot_speed = glm::two_pi<float>() / 87.0f;
    mercury_node.set_scaling(glm::vec3(0.1, 0.1, 0.1));
    mercury_node.set_translation(glm::vec3(3, 0, 0));

    Node venus_node;
    Node venus_pivot;
    venus_node.set_geometry(sphere);
    GLuint const venus_texture = bonobo::loadTexture2D("venusmap.png");
    venus_node.add_texture("diffuse_texture", venus_texture, GL_TEXTURE_2D);
    float const venus_spin_speed = glm::two_pi<float>() / -223.0f;
    float const venus_pivot_speed = glm::two_pi<float>() / 224.0f;
    venus_node.set_scaling(glm::vec3(0.2, 0.2, 0.2));
    venus_node.set_translation(glm::vec3(5, 0, 0));

    Node earth_node;
    Node earth_pivot;
    Node earth_translation;
    earth_node.set_geometry(sphere);
    GLuint const earth_texture = bonobo::loadTexture2D("earthmap1k.png");
    earth_node.add_texture("diffuse_texture", earth_texture, GL_TEXTURE_2D);
    float const earth_spin_speed = glm::two_pi<float>() / 1.0f;
    float const earth_pivot_speed = glm::two_pi<float>() / 365.0f;
    earth_translation.set_translation(glm::vec3(7, 0, 0));
    earth_node.set_scaling(glm::vec3(0.2, 0.2, 0.2));
    earth_node.rotate_z(glm::two_pi<float>() / 12.0f);

    Node moon_node;
    Node moon_pivot;
    moon_node.set_geometry(sphere);
    GLuint const moon_texture = bonobo::loadTexture2D("moonmap1k.png");
    moon_node.add_texture("diffuse_texture", moon_texture, GL_TEXTURE_2D);
    float const moon_spin_speed = glm::two_pi<float>() / 27.0f;
    float const moon_pivot_speed = glm::two_pi<float>() / 27.0f;
    moon_node.set_translation(glm::vec3(1, 0, 0));
    moon_node.set_scaling(glm::vec3(0.1, 0.1, 0.1));

    Node mars_node;
    Node mars_pivot;
    mars_node.set_geometry(sphere);
    GLuint const mars_texture = bonobo::loadTexture2D("mars_1k_color.png");
    mars_node.add_texture("diffuse_texture", mars_texture, GL_TEXTURE_2D);
    float const mars_spin_speed = glm::two_pi<float>() / 1.0f;
    float const mars_pivot_speed = glm::two_pi<float>() / 686.0f;
    mars_node.set_scaling(glm::vec3(0.15, 0.15, 0.15));
    mars_node.set_translation(glm::vec3(9, 0, 0));

    Node jupiter_node;
    Node jupiter_pivot;
    jupiter_node.set_geometry(sphere);
    GLuint const jupiter_texture = bonobo::loadTexture2D("jupitermap.png");
    jupiter_node.add_texture("diffuse_texture", jupiter_texture, GL_TEXTURE_2D);
    float const jupiter_spin_speed = glm::two_pi<float>() / 0.4f;
    float const jupiter_pivot_speed = glm::two_pi<float>() / 398.0f;
    jupiter_node.set_scaling(glm::vec3(0.5, 0.5, 0.5));
    jupiter_node.set_translation(glm::vec3(11, 0, 0));

//    Node saturn_node;
//    Node saturn_pivot;
//    //Node saturn_ring;
//    saturn_node.set_geometry(sphere);
//    //saturn_ring.set_geometry(ring);
//    GLuint const saturn_texture = bonobo::loadTexture2D("saturnmap.png");
//    GLuint const saturn_ring_texture = bonobo::loadTexture2D("saturnringcolor.png");
//    saturn_node.add_texture("diffuse_texture", saturn_texture, GL_TEXTURE_2D);
//    //saturn_ring.add_texture("diffuse_texture", saturn_ring_texture, GL_TEXTURE_2D);
//    float const saturn_spin_speed = glm::two_pi<float>() / 0.5f;
//    float const saturn_pivot_speed = glm::two_pi<float>() / 378.0f;
//    float const saturn_ring_spin_speed = glm::two_pi<float>() / 10.0f;
//    saturn_node.set_scaling(glm::vec3(0.4, 0.4, 0.4));
//    saturn_node.set_translation(glm::vec3(13, 0, 0));

    //Add planets to solar system
    solar_system_node.add_child(&sun_node);
    solar_system_node.add_child(&mercury_pivot);
    solar_system_node.add_child(&venus_pivot);
    solar_system_node.add_child(&earth_pivot);
    solar_system_node.add_child(&mars_pivot);
    solar_system_node.add_child(&jupiter_pivot);
    //solar_system_node.add_child(&saturn_pivot);

    //Setup planet pivots
    mercury_pivot.add_child(&mercury_node);
    venus_pivot.add_child(&venus_node);
    earth_pivot.add_child(&earth_translation);
    mars_pivot.add_child(&mars_node);
    jupiter_pivot.add_child(&jupiter_node);
    //saturn_pivot.add_child(&saturn_node);
    earth_translation.add_child(&earth_node);
    earth_translation.add_child(&moon_pivot);
    moon_pivot.add_child(&moon_node);


    //
    // TODO: Create nodes for the remaining of the solar system
    //


    // Retrieve the actual framebuffer size: for HiDPI monitors, you might
    // end up with a framebuffer larger than what you actually asked for.
    // For example, if you ask for a 1920x1080 framebuffer, you might get a
    // 3840x2160 one instead.
    int framebuffer_width, framebuffer_height;
    glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);

    glViewport(0, 0, framebuffer_width, framebuffer_height);
    glClearDepthf(1.0f);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);


    size_t fpsSamples = 0;
    double lastTime = GetTimeSeconds();
    double fpsNextTick = lastTime + 1.0;


    bool show_logs = true;
    bool show_gui = true;

    while (!glfwWindowShouldClose(window)) {
        //
        // Compute timings information
        //
        double const nowTime = GetTimeSeconds();
        double const delta_time = nowTime - lastTime;
        lastTime = nowTime;
        if (nowTime > fpsNextTick) {
            fpsNextTick += 1.0;
            fpsSamples = 0;
        }
        ++fpsSamples;


        //
        // Process inputs
        //
        glfwPollEvents();

        ImGuiIO const &io = ImGui::GetIO();
        input_handler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);
        input_handler.Advance();
        camera.Update(delta_time, input_handler);

        if (input_handler.GetKeycodeState(GLFW_KEY_F3) & JUST_RELEASED)
            show_logs = !show_logs;
        if (input_handler.GetKeycodeState(GLFW_KEY_F2) & JUST_RELEASED)
            show_gui = !show_gui;
        if (input_handler.GetKeycodeState(GLFW_KEY_K) & JUST_RELEASED)
            speed_param += 0.5f;
        if (input_handler.GetKeycodeState(GLFW_KEY_J) & JUST_RELEASED)
            speed_param -= 0.5f;

        //
        // Start a new frame for Dear ImGui
        //
        ImGui_ImplGlfwGL3_NewFrame();


        //
        // Clear the screen
        //
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


        //
        // Update the transforms
        //
        sun_node.rotate_y(speed_param * sun_spin_speed * delta_time);
        mercury_node.rotate_y(speed_param * mercury_spin_speed * delta_time);
        mercury_pivot.rotate_y(speed_param * mercury_pivot_speed * delta_time);
        venus_node.rotate_y(speed_param * venus_spin_speed * delta_time);
        venus_pivot.rotate_y(speed_param * venus_pivot_speed * delta_time);
        earth_pivot.rotate_y(speed_param * earth_pivot_speed * delta_time);
        earth_node.rotate_y(speed_param * earth_spin_speed * delta_time);
        mars_pivot.rotate_y(speed_param * mars_pivot_speed * delta_time);
        mars_node.rotate_y(speed_param * mars_spin_speed * delta_time);
        jupiter_pivot.rotate_y(speed_param * jupiter_pivot_speed * delta_time);
        jupiter_node.rotate_y(speed_param * jupiter_spin_speed * delta_time);
//        saturn_pivot.rotate_y(speed_param * saturn_pivot_speed * delta_time);
//        saturn_node.rotate_y(speed_param * saturn_spin_speed * delta_time);
        moon_node.rotate_y(speed_param * moon_spin_speed * 10 * delta_time);
        moon_pivot.rotate_y(speed_param * moon_pivot_speed * delta_time);
        moon_pivot.rotate_x(speed_param * moon_pivot_speed * 0.4 * delta_time);

        //
        // Traverse the scene graph and render all nodes
        //
        std::stack<Node const *> node_stack({&solar_system_node});
        std::stack<glm::mat4> matrix_stack({glm::mat4(1.0f)});
        // TODO: Replace this explicit rendering of the Sun with a
        // traversal of the scene graph and rendering of all its nodes.
        //sun_node.render(camera.GetWorldToClipMatrix(), sun_node.get_transform(), shader, [](GLuint /*program*/){});

        while (!node_stack.empty()) {
            const Node *current_node = node_stack.top();
            size_t nr_of_children = current_node->get_children_nb();
            node_stack.pop();
            glm::mat4 total_transform = matrix_stack.top() * current_node->get_transform();
            matrix_stack.pop();

            current_node->render(camera.GetWorldToClipMatrix(), total_transform, shader, [](GLuint /*program*/) {});

            for (size_t i = 0; i < nr_of_children; i++) {
                node_stack.push(current_node->get_child(i));
                matrix_stack.push(total_transform);

            }
        }

        bool isOpen = ImGui::Begin("Params");
        if (isOpen) {
            ImGui::DragFloat("Sped", &speed_param);
        }
        ImGui::End();

        //
        // Display Dear ImGui windows
        //
        if (show_logs)
            Log::View::Render();
        if (show_gui)
            ImGui::Render();

        //
        // Queue the computed frame for display on screen
        //
        glfwSwapBuffers(window);
    }

    glDeleteTextures(1, &sun_texture);
    glDeleteTextures(1, &earth_texture);
    glDeleteTextures(1, &moon_texture);


    Log::View::Destroy();
    Log::Destroy();

    return EXIT_SUCCESS;
}

void render_nodes(Node &root) {


}
