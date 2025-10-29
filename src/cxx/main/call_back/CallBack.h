//
// Created by 30367 on 2025/10/28.
//

#ifndef INITIALGL_CALLBACK_H
#define INITIALGL_CALLBACK_H

#include "GLFW\glfw3.h"

namespace CallBack {
    void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
}

#endif //INITIALGL_CALLBACK_H