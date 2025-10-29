//
// Created by 30367 on 2025/10/28.
//
#include <GLFW/glfw3.h>
#include "CallBack.h"

extern bool keys[];
extern bool cursorEnabled;
extern GLfloat offset_x, offset_y;
static GLfloat lastx = 0.0f, lasty = 0.0f;
static bool firstMouse = true;
extern float scale;

namespace CallBack {
    void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        //Keyboard handling logic
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
            cursorEnabled = !cursorEnabled;
        if (key >= 0 && key < 1024)
        {
            keys[key] = (action != GLFW_RELEASE);
        }
    }

    void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
        //Mouse handling logic
        if (firstMouse) {
            lastx = static_cast<GLfloat>(xpos);
            lasty = static_cast<GLfloat>(ypos);
            firstMouse = false;
            offset_x = 0.0f;
            offset_y = 0.0f;
            return;
        }
        offset_x = static_cast<GLfloat>(lastx - xpos);
        offset_y = static_cast<GLfloat>(lasty - ypos); // Reversed since y-coordinates go from bottom
        lastx = static_cast<GLfloat>(xpos);
        lasty = static_cast<GLfloat>(ypos);
    }

    void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        //Scroll handling logic
        scale += yoffset;
    }
}