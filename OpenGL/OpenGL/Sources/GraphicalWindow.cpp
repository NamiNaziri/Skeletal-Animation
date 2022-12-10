#include "GraphicalWindow.h"

#include <iostream>
#include <glad/glad.h>

GraphicalWindow::GraphicalWindow(int width, int height) : width(width), height(height)
{
    this->window = CreateGLFWWindow();
}

const std::unique_ptr<GLFWwindow, DestroyglfwWin>& GraphicalWindow::GetWindow()
{
    return window;
}

std::unique_ptr<GLFWwindow, DestroyglfwWin> GraphicalWindow::CreateGLFWWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //std::unique_ptr<GLFWwindow> window = std::make_unique<GLFWwindow>()
    GLFWwindow* window = glfwCreateWindow(width, height, "Skeletal Animation", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW Window" << std::endl;
        glfwTerminate();
        //return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // disable cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed To initialize GLAD" << std::endl;
        //return -1;
    }

    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
    return std::unique_ptr<GLFWwindow, DestroyglfwWin>(window);
}

void GraphicalWindow::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
