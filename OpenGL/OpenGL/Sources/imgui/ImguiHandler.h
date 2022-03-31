#pragma once
#include <GLFW/glfw3.h>

class ImguiHandler
{
public:
	ImguiHandler(GLFWwindow& window);
	
	void BeginFrame();
	void Update(); //todo this has to be implemented after having a decent storage class
	void EndFrame();
	void Render();

private:
	void Init(GLFWwindow& window);

	
	
	
};

