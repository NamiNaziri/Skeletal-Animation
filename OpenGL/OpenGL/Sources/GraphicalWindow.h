#pragma once
#include <memory>


#include "imgui/ImguiHandler.h"

struct DestroyglfwWin {

	void operator()(GLFWwindow* ptr) {
		glfwDestroyWindow(ptr);
	}

};

class GraphicalWindow
{
public:
	GraphicalWindow(int width, int height);

	const std::unique_ptr<GLFWwindow, DestroyglfwWin>& GetWindow();



private:
	std::unique_ptr<GLFWwindow, DestroyglfwWin> CreateGLFWWindow();
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	
	


private:
	std::unique_ptr<GLFWwindow, DestroyglfwWin> window ;

	int width = 1080;
	int height = 1080;

};

