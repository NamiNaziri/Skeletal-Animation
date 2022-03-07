#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Shader.h"
#include "STB/stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "GameObject.h"
#include <vector>
#include <iostream>
#include "Model.h"
#include "Mesh.h"
#include "SkeletalModel.h"
#include <glm/gtx/string_cast.hpp>

#include "Animation.h"
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void WindowResizeHandler(GLFWwindow* window, int w, int h);

// Functions

GLFWwindow* CreateWindow();
unsigned int loadTexture(const char* path);
// camera

Camera cam(	glm::vec3(0.0f, 0.0f, 500.0f) ,
			glm::vec3(0.0f, 1.0f, 0.0f),
			45,
			0,
			-90
			);

int Width = 1080;
int Height = 1080;


float lastX = Width/2, lastY = Height/2;


bool firstMouse = true;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

bool ShowModel = true;

#define POINT_LIGHTS_NUM 4

std::vector<glm::vec3> pos;
void DrawSkeleton(Bone* root, glm::mat4 parentTrnsform)
{
	glm::mat4 newParentTransform = parentTrnsform * root->GetTransform();
	pos.push_back(glm::vec3(newParentTransform[3]));
	for(int i = 0 ; i < root->GetChildren().size() ; i++)
	{
		DrawSkeleton(root->GetChildren()[i], newParentTransform);
	}
	
}

int main()
{

	GLFWwindow* window = CreateWindow();

	Shader SimpleShader("VertexShader.vert", "FragmentShader.frag");
	Shader LightShader("VertexShader.vert", "LightFragmentShader.frag");

	// set upVector vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	
	std::vector<float>  vertices = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  3.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	

	// Light cube object

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};
	
	glm::vec3 lightPosition(0.f, 0.f, -6.f);
	GameObject lights[POINT_LIGHTS_NUM] = {GameObject(vertices),GameObject(vertices) ,GameObject(vertices) ,GameObject(vertices)};
	for(int i = 0 ; i < POINT_LIGHTS_NUM; i++)
	{
		lights[i].SetPosition(pointLightPositions[i]);
	}


	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);


	const std::string FBXResourcePath = "resources/objects/MixamoCharacter/Zombie Stand Up.fbx";
	SkeletalModel ourModel(FBXResourcePath);
	std::cout << "Model Loaded" << std::endl;

	// Load animation Clip
	 AnimationClipManager animationClipManager(FBXResourcePath, ourModel.GetSkeleton());
	
	
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		// input
		processInput(window);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetWindowSizeCallback(window, WindowResizeHandler);
		// rendering commands

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe
		
		
		// render container
		SimpleShader.use(); // don't forget to activate/use the shader before setting uniforms!
							// either set it manually like so:

		// material
		SimpleShader.SetFloat("material.shininess", 32.0f);

		// camera view
		SimpleShader.SetVec3("viewPosition", cam.GetPosition());

		// light

		// directional light
		SimpleShader.SetVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
		SimpleShader.SetVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		SimpleShader.SetVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
		SimpleShader.SetVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		// point light 1
		SimpleShader.SetVec3("pointLights[0].position", pointLightPositions[0]);
		SimpleShader.SetVec3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		SimpleShader.SetVec3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		SimpleShader.SetVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		SimpleShader.SetFloat("pointLights[0].constant", 1.0f);
		SimpleShader.SetFloat("pointLights[0].linear", 0.09f);
		SimpleShader.SetFloat("pointLights[0].quadratic", 0.032f);
		// point light 2
		SimpleShader.SetVec3("pointLights[1].position", pointLightPositions[1]);
		SimpleShader.SetVec3("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		SimpleShader.SetVec3("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		SimpleShader.SetVec3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		SimpleShader.SetFloat("pointLights[1].constant", 1.0f);
		SimpleShader.SetFloat("pointLights[1].linear", 0.09f);
		SimpleShader.SetFloat("pointLights[1].quadratic", 0.032f);
		// point light 3
		SimpleShader.SetVec3("pointLights[2].position", pointLightPositions[2]);
		SimpleShader.SetVec3("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		SimpleShader.SetVec3("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		SimpleShader.SetVec3("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		SimpleShader.SetFloat("pointLights[2].constant", 1.0f);
		SimpleShader.SetFloat("pointLights[2].linear", 0.09f);
		SimpleShader.SetFloat("pointLights[2].quadratic", 0.032f);
		// point light 4
		SimpleShader.SetVec3("pointLights[3].position", pointLightPositions[3]);
		SimpleShader.SetVec3("pointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		SimpleShader.SetVec3("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		SimpleShader.SetVec3("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		SimpleShader.SetFloat("pointLights[3].constant", 1.0f);
		SimpleShader.SetFloat("pointLights[3].linear", 0.09f);
		SimpleShader.SetFloat("pointLights[3].quadratic", 0.032f);
		// spotLight
		/*SimpleShader.SetVec3("spotLight.position", cam.GetPosition());
		SimpleShader.SetVec3("spotLight.direction", cam.GetForward());
		SimpleShader.SetVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		SimpleShader.SetVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		SimpleShader.SetVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		SimpleShader.SetFloat("spotLight.constant", 1.0f);
		SimpleShader.SetFloat("spotLight.linear", 0.09f);
		SimpleShader.SetFloat("spotLight.quadratic", 0.032f);
		SimpleShader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		SimpleShader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));*/


		glm::mat4 view = cam.GetViewMatrix();
		SimpleShader.SetMat4("view", view);


		
		glm::mat4 projection = glm::mat4(1);

		projection = glm::perspective(glm::radians(cam.GetFOV()), ((float)Width / (float)Height), 0.1f, 50000.0f);
		SimpleShader.SetMat4("projection", projection);
		

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));	// it's a bit too big for our scene, so scale it down
		SimpleShader.SetMat4("model", model);

		glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));
		SimpleShader.SetMat3("normalMatrix", normalMatrix);


		if (ShowModel)
		{
			ourModel.Draw(SimpleShader);
		}
		
		// LightShader
		LightShader.use();
		LightShader.SetMat4("view", view);
		LightShader.SetMat4("projection", projection);

		//Creates the bone of the the skeleton
		ourModel.DrawSkeletonJoints(LightShader);
		


		for (int i = 0; i < POINT_LIGHTS_NUM; i++)
		{
			//lights[i].Render(LightShader);
		}
		
		
		
		// check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();

	return 0;
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if(glfwGetKey(window,GLFW_KEY_H) == GLFW_PRESS)
	{
		ShowModel = !ShowModel;
	}


	const float cameraSpeed = cam.GetSpeed() * deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.Translate(cam.GetForward(), cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.Translate(cam.GetForward(), -cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.Translate(cam.GetRight(), -cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.Translate(cam.GetRight(), cameraSpeed);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) // initially set to true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	float pitch = cam.GetPitch();
	float yaw = cam.GetYaw();

	yaw += xoffset;
	pitch += yoffset;
	

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;


	cam.Rotate(pitch, yaw);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	float speed = cam.GetSpeed();
	speed += (float)yoffset * 10 ;
	/*if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;*/

	cam.SetSpeed(speed);
}



GLFWwindow* CreateWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(Width, Height, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		//return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed To initialize GLAD" << std::endl;
		//return -1;
	}

	glViewport(0, 0, Width, Height);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	return window;
}


void WindowResizeHandler(GLFWwindow* window, int w, int h)
{
	std::cout << "Window Resized. W: " << w << "  H: " << h << std::endl;
	Width = w;
	Height = h;
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}