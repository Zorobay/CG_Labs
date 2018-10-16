#pragma once

#include <core/node.hpp>
#include "core/InputHandler.h"
#include "core/FPSCamera.h"
#include "Snejk.h"
#include "core/WindowManager.hpp"
#include <random>


class Window;


namespace edaf80 {
    //! \brief Wrapper class for Assignment 5
    class Assignment5 {
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
                           std::function<void(GLuint)> const &set_uniforms, size_t amount, glm::vec3 snek_pos);


    private:
        FPSCameraf mCamera;
        InputHandler inputHandler;
        WindowManager mWindowManager;
        GLFWwindow *window;
        const float world_radi = 50.0f;
        const float food_radi = 0.3f;
        std::vector<Node> food;
        const float camera_y_disp = 5.0f;
        const float camera_z_disp = 8.0f;

    };
}
