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
    camera.mWorld.SetTranslate(glm::vec3(0.0f, 25.0f, 26.5f));
    camera.mWorld.SetRotateX(-.8f);
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

	Node mercury_node;
	mercury_node.set_geometry(sphere);
	GLuint const mercury_texture = bonobo::loadTexture2D("mercurymap1k.png");
	mercury_node.add_texture("diffuse_texture", mercury_texture, GL_TEXTURE_2D);

    Node venus_node;
    venus_node.set_geometry(sphere);
    GLuint const venus_texture = bonobo::loadTexture2D("venusmap1k.png");
    venus_node.add_texture("diffuse_texture", venus_texture, GL_TEXTURE_2D);

    Node earth_node;
    earth_node.set_geometry(sphere);
    GLuint const earth_texture = bonobo::loadTexture2D("earthmap1k.png");
    earth_node.add_texture("diffuse_texture", earth_texture, GL_TEXTURE_2D);

    Node moon_node;
    moon_node.set_geometry(sphere);
    GLuint const moon_texture = bonobo::loadTexture2D("moonmap1k.png");
    moon_node.add_texture("diffuse_texture", moon_texture, GL_TEXTURE_2D);

    Node mars_node;
    mars_node.set_geometry(sphere);
    GLuint const mars_texture = bonobo::loadTexture2D("marsmap1k.png");
    mars_node.add_texture("diffuse_texture", mars_texture, GL_TEXTURE_2D);

    Node phobos_node;
    phobos_node.set_geometry(sphere);
    GLuint const phobos_texture = bonobo::loadTexture2D("phobosmap1k.png");
    phobos_node.add_texture("diffuse_texture", phobos_texture, GL_TEXTURE_2D);

    Node deimos_node;
    deimos_node.set_geometry(sphere);
    GLuint const deimos_texture = bonobo::loadTexture2D("deimosmap1k.png");
    deimos_node.add_texture("diffuse_texture", deimos_texture, GL_TEXTURE_2D);

    Node jupiter_node;
    jupiter_node.set_geometry(sphere);
    GLuint const jupiter_texture = bonobo::loadTexture2D("jupitermap1k.png");
    jupiter_node.add_texture("diffuse_texture", jupiter_texture, GL_TEXTURE_2D);

    Node saturn_node;
    saturn_node.set_geometry(sphere);
    GLuint const saturn_texture = bonobo::loadTexture2D("saturnmap1k.png");
    saturn_node.add_texture("diffuse_texture", saturn_texture, GL_TEXTURE_2D);

    Node uranus_node;
    uranus_node.set_geometry(sphere);
    GLuint const uranus_texture = bonobo::loadTexture2D("uranusmap1k.png");
    uranus_node.add_texture("diffuse_texture", uranus_texture, GL_TEXTURE_2D);

    Node neptune_node;
    neptune_node.set_geometry(sphere);
    GLuint const neptune_texture = bonobo::loadTexture2D("neptunemap1k.png");
    neptune_node.add_texture("diffuse_texture", neptune_texture, GL_TEXTURE_2D);

    Node pluto_node;
    pluto_node.set_geometry(sphere);
    GLuint const pluto_texture = bonobo::loadTexture2D("plutomap1k.png");
    pluto_node.add_texture("diffuse_texture", pluto_texture, GL_TEXTURE_2D);

    Node mercury_pivot_node;
    Node venus_pivot_node;
    Node earth_pivot_node;
    Node earth_trans_node;
    Node moon_pivot_node;
    Node mars_pivot_node;
    Node mars_trans_node;
    Node phobos_pivot_node;
    Node deimos_pivot_node;
    Node jupiter_pivot_node;
    Node saturn_pivot_node;
    Node uranus_pivot_node;
    Node neptune_pivot_node;
    Node pluto_pivot_node;

    Node solar_system_node;
    solar_system_node.add_child(&sun_node);
    solar_system_node.add_child(&mercury_pivot_node);
    solar_system_node.add_child(&venus_pivot_node);
    solar_system_node.add_child(&earth_pivot_node);
    solar_system_node.add_child(&mars_pivot_node);
    solar_system_node.add_child(&jupiter_pivot_node);
    solar_system_node.add_child(&saturn_pivot_node);
    solar_system_node.add_child(&uranus_pivot_node);
    solar_system_node.add_child(&neptune_pivot_node);
    solar_system_node.add_child(&pluto_pivot_node);

    mercury_pivot_node.add_child(&mercury_node);

    venus_pivot_node.add_child(&venus_node);

	earth_pivot_node.add_child(&earth_trans_node);
    earth_trans_node.add_child(&moon_pivot_node);
    earth_trans_node.add_child(&earth_node);
	moon_pivot_node.add_child(&moon_node);

    mars_pivot_node.add_child(&mars_trans_node);
    mars_trans_node.add_child(&mars_node);
    mars_trans_node.add_child(&phobos_pivot_node);
    mars_trans_node.add_child(&deimos_pivot_node);
    phobos_pivot_node.add_child(&phobos_node);
    deimos_pivot_node.add_child(&deimos_node);

    jupiter_pivot_node.add_child(&jupiter_node);

    saturn_pivot_node.add_child(&saturn_node);

    uranus_pivot_node.add_child(&uranus_node);

    neptune_pivot_node.add_child(&neptune_node);

    pluto_pivot_node.add_child(&pluto_node);

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

        mercury_pivot_node.rotate_y(sun_spin_speed*delta_time*2.5);
        mercury_node.set_translation(glm::vec3(1.5,0,0));
        mercury_node.rotate_y(sun_spin_speed*delta_time*4);
        mercury_node.set_scaling(glm::vec3(.13,.13,.13));

        venus_pivot_node.rotate_y(sun_spin_speed*delta_time*2);
        venus_node.set_translation(glm::vec3(2.5,0,0));
        venus_node.rotate_y(sun_spin_speed*delta_time*3);
        venus_node.set_scaling(glm::vec3(.15,.15,.15));

        earth_pivot_node.rotate_y(sun_spin_speed*delta_time*1.5);
        earth_trans_node.set_translation(glm::vec3(4,0,0));
        earth_node.rotate_y(10*sun_spin_speed*delta_time);
        earth_node.set_scaling(glm::vec3(.25,.25,.25));

        moon_pivot_node.rotate_y(sun_spin_speed*delta_time*4);
        moon_node.set_translation(glm::vec3(1,0,0));
        moon_node.set_scaling(glm::vec3(.1,.1,.1));
        moon_node.rotate_y(5*sun_spin_speed*delta_time);

        mars_pivot_node.rotate_y(sun_spin_speed*delta_time*1.3);
        mars_trans_node.set_translation(glm::vec3(6.5,0,0));
        mars_node.rotate_y(8*sun_spin_speed*delta_time);
        mars_node.set_scaling(glm::vec3(.18,.18,.18));

        phobos_pivot_node.rotate_y(sun_spin_speed*delta_time*3.5);
        phobos_node.set_translation(glm::vec3(.8,0,0));
        phobos_node.set_scaling(glm::vec3(.08,.08,.08));
        phobos_node.rotate_y(5*sun_spin_speed*delta_time);

        deimos_pivot_node.rotate_y(sun_spin_speed*delta_time*2.5);
        deimos_node.set_translation(glm::vec3(1.1,0,0));
        deimos_node.set_scaling(glm::vec3(.06,.06,.06));
        deimos_node.rotate_y(5*sun_spin_speed*delta_time);

        jupiter_pivot_node.rotate_y(sun_spin_speed*delta_time*1.0);
        jupiter_node.set_translation(glm::vec3(10,0,0));
        jupiter_node.rotate_y(6*sun_spin_speed*delta_time);
        jupiter_node.set_scaling(glm::vec3(.7,.7,.7));

        saturn_pivot_node.rotate_y(sun_spin_speed*delta_time*.9);
        saturn_node.set_translation(glm::vec3(14,0,0));
        saturn_node.rotate_y(4*sun_spin_speed*delta_time);
        saturn_node.set_scaling(glm::vec3(.5,.5,.5));

        uranus_pivot_node.rotate_y(sun_spin_speed*delta_time*.8);
        uranus_node.set_translation(glm::vec3(17,0,0));
        uranus_node.rotate_y(3*sun_spin_speed*delta_time);
        uranus_node.set_scaling(glm::vec3(.6,.6,.6));

        neptune_pivot_node.rotate_y(sun_spin_speed*delta_time*.65);
        neptune_node.set_translation(glm::vec3(20,0,0));
        neptune_node.rotate_y(5*sun_spin_speed*delta_time);
        neptune_node.set_scaling(glm::vec3(.45,.45,.45));

        pluto_pivot_node.rotate_y(sun_spin_speed*delta_time*.25);
        pluto_node.set_translation(glm::vec3(35,0,0));
        pluto_node.rotate_y(5*sun_spin_speed*delta_time);
        pluto_node.set_scaling(glm::vec3(.1,.1,.1));

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
    glDeleteTextures(1, &venus_texture);
    glDeleteTextures(1, &earth_texture);
    glDeleteTextures(1, &moon_texture);
    glDeleteTextures(1, &mars_texture);


	Log::View::Destroy();
	Log::Destroy();

	return EXIT_SUCCESS;
}
