#pragma once

#include <core/node.hpp>
#include "core/InputHandler.h"
#include "core/FPSCamera.h"
#include "Snejk.h"
#include "Food.hpp"
#include "core/WindowManager.hpp"
#include <random>

class Window;

namespace edaf80
{
//! \brief Wrapper class for Assignment 5
class Assignment5
{
  public:
	//! \brief Default constructor.
	//!
	//! It will initialise various modules of bonobo and retrieve a
	//! window to draw to.
	Assignment5();

	//! \brief Default destructor.
	//!
	//! It will release the bonobo modules initialised by the
	//! constructor, as well as the window.
	~Assignment5();

	//! \brief Contains the logic of the assignment, along with the
	//! render loop.
	void run();

	// Generate food for snake
	void generate_food(bonobo::mesh_data const &shape, GLuint const *const program,
					   std::function<void(GLuint)> const &set_uniforms, size_t amount, Snejk snejk);

  private:
	FPSCameraf mCamera;
	InputHandler inputHandler;
	WindowManager mWindowManager;
	GLFWwindow *window;
	const float world_radi = 100.0f;
	const float food_radi = 0.3f;
	std::vector<Food> food_list;
	const float camera_y_disp = 3.0f;
	const float camera_z_disp = 8.0f;
	std::vector<int> highscores;
};
} // namespace edaf80
