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


int main()
{
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
	camera.mWorld.SetTranslate(glm::vec3(0.0f, 0.0f, 6.0f));
	camera.mMouseSensitivity = 0.003f;
	camera.mMovementSpeed = 0.25f * 12.0f;

	//
	// Set up the windowing system and create the window
	//
	WindowManager window_manager;
	WindowManager::WindowDatum window_datum{ input_handler, camera, config::resolution_x, config::resolution_y, 0, 0, 0, 0};
	GLFWwindow* window = window_manager.CreateWindow("EDAF80: Assignment 1", window_datum, config::msaa_rate);
	if (window == nullptr) {
		LogError("Failed to get a window: exiting.");

		Log::View::Destroy();
		Log::Destroy();

		return EXIT_FAILURE;
	}

	//
	// Load the sphere geometry
	//
	std::vector<bonobo::mesh_data> const objects = bonobo::loadObjects("sphere.obj");
	if (objects.empty()) {
		LogError("Failed to load the sphere geometry: exiting.");

		Log::View::Destroy();
		Log::Destroy();

		return EXIT_FAILURE;
	}
	bonobo::mesh_data const& sphere = objects.front();


	//
	// Create the shader program
	//
	ShaderProgramManager program_manager;
	GLuint shader = 0u;
	program_manager.CreateAndRegisterProgram({ { ShaderType::vertex, "EDAF80/default.vert" },
	                                           { ShaderType::fragment, "EDAF80/default.frag" } },
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
    Node solar_system_node;

    Node sun_node;
    sun_node.set_geometry(sphere);
    GLuint const sun_texture = bonobo::loadTexture2D("sunmap.png");
    sun_node.add_texture("diffuse_texture", sun_texture, GL_TEXTURE_2D);
    float const sun_spin_speed = glm::two_pi<float>() / 25.0f; // Full rotation in six seconds

    Node earth_node;
    Node earth_pivot;
    Node earth_translation;
    earth_node.set_geometry(sphere);
    GLuint const earth_texture = bonobo::loadTexture2D("earthmap1k.png");
    earth_node.add_texture("diffuse_texture", earth_texture, GL_TEXTURE_2D);
    float const earth_spin_speed = glm::two_pi<float>() / 1.0f;
    float const earth_pivot_speed = glm::two_pi<float>() / 10.0f;
    earth_translation.set_translation(glm::vec3(3,0,0));
    earth_node.set_scaling(glm::vec3(0.25,0.25,0.25));

    Node moon_node;
    Node moon_pivot;
    moon_node.set_geometry(sphere);
    GLuint const moon_texture = bonobo::loadTexture2D("moonmap1k.png");
    moon_node.add_texture("diffuse_texture", moon_texture, GL_TEXTURE_2D);
    float const moon_spin_speed = glm::two_pi<float>() / 10.0f;
    moon_node.set_translation(glm::vec3(1,0,0));
    moon_node.set_scaling(glm::vec3(0.1,0.1,0.1));

    solar_system_node.add_child(&sun_node);
    solar_system_node.add_child(&earth_pivot);
    earth_pivot.add_child(&earth_translation);
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

		ImGuiIO const& io = ImGui::GetIO();
		input_handler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);
		input_handler.Advance();
		camera.Update(delta_time, input_handler);

		if (input_handler.GetKeycodeState(GLFW_KEY_F3) & JUST_RELEASED)
			show_logs = !show_logs;
		if (input_handler.GetKeycodeState(GLFW_KEY_F2) & JUST_RELEASED)
			show_gui = !show_gui;


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
		sun_node.rotate_y(sun_spin_speed * delta_time);
        earth_pivot.rotate_y(earth_pivot_speed * delta_time);
        earth_node.rotate_y(earth_spin_speed*-1 * delta_time);
        moon_node.rotate_y(moon_spin_speed *10* delta_time);
        moon_pivot.rotate_y(moon_spin_speed *10* delta_time);

		//
		// Traverse the scene graph and render all nodes
		//
        std::stack<Node const*> node_stack({ &solar_system_node });
		std::stack<glm::mat4> matrix_stack({ glm::mat4(1.0f) });
		// TODO: Replace this explicit rendering of the Sun with a
		// traversal of the scene graph and rendering of all its nodes.
		//sun_node.render(camera.GetWorldToClipMatrix(), sun_node.get_transform(), shader, [](GLuint /*program*/){});

        while(!node_stack.empty())
        {
            const Node *current_node = node_stack.top();
            size_t nr_of_children = current_node->get_children_nb();
            node_stack.pop();
            glm::mat4 total_transform = matrix_stack.top() * current_node->get_transform();
            matrix_stack.pop();

            current_node->render(camera.GetWorldToClipMatrix(), total_transform, shader, [](GLuint /*program*/){});

            for (size_t i = 0; i < nr_of_children; i++)
            {
                node_stack.push(current_node->get_child(i));
                matrix_stack.push(total_transform);

            }
        }

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

void render_nodes(Node &root)
{


}
