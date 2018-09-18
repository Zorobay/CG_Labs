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

	Node sun_node;
	sun_node.set_geometry(sphere);
	GLuint const sun_texture = bonobo::loadTexture2D("sunmap.png");
	sun_node.add_texture("diffuse_texture", sun_texture, GL_TEXTURE_2D);
	float const sun_spin_speed = glm::two_pi<float>() / 12.0f; // Full rotation in six seconds

    Node earth_node;
    earth_node.set_geometry(sphere);
    GLuint const earth_texture = bonobo::loadTexture2D("earthmap1k.png");
    earth_node.add_texture("diffuse_texture", earth_texture, GL_TEXTURE_2D);

    Node moon_node;
    moon_node.set_geometry(sphere);
    GLuint const moon_texture = bonobo::loadTexture2D("moonmap1k.png");
    moon_node.add_texture("diffuse_texture", moon_texture, GL_TEXTURE_2D);

    Node earth_pivot_node;
    Node earth_trans_node;

    Node moon_pivot_node;

    Node solar_system_node;
    solar_system_node.add_child(&sun_node);
    solar_system_node.add_child(&earth_pivot_node);

	earth_pivot_node.add_child(&earth_trans_node);
    earth_trans_node.add_child(&moon_pivot_node);
    earth_trans_node.add_child(&earth_node);
	moon_pivot_node.add_child(&moon_node);


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


		//node
		// Clear the screen
		//
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


		//
		// Update the transforms
		//
		sun_node.rotate_y(sun_spin_speed * delta_time);
		sun_node.translate(glm::vec3(0,0,0));
		sun_node.scale(glm::vec3(1,1,1));

		earth_pivot_node.rotate_y(sun_spin_speed*delta_time*1.5);
        earth_trans_node.set_translation(glm::vec3(5,0,0));
        earth_node.rotate_y(10*sun_spin_speed*delta_time);
        earth_node.set_scaling(glm::vec3(0.5,0.5,0.5));

        moon_pivot_node.rotate_y(sun_spin_speed*delta_time*3);

        moon_node.set_translation(glm::vec3(1.5,0,0));
        moon_node.set_scaling(glm::vec3(0.2,0.2,0.2));
        moon_node.rotate_y(5*sun_spin_speed*delta_time);

		//
		// Traverse the scene graph and render all nodes
		//
		std::stack<Node const*> node_stack({ &solar_system_node });
		std::stack<glm::mat4> matrix_stack({ glm::mat4(1.0f) });

		while(!node_stack.empty()){
			Node const *curNode = node_stack.top();
			node_stack.pop();
			glm::mat4 curMatrix = matrix_stack.top()*(curNode->get_transform());
			matrix_stack.pop();
			curNode->render(camera.GetWorldToClipMatrix(), curMatrix, shader, [](GLuint /*program*/){});
			size_t nbr = curNode->get_children_nb();
			for(size_t i = 0; i < nbr; i++){
				node_stack.push(curNode->get_child(i));
				matrix_stack.push(curMatrix);
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
