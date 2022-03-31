#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

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
#include "UIFunctions.h"
#include "Animation/Animation.h"
#include "Animation/Animator.h"
#include "Animation/AnimationClipManager.h"
#include "imgui/Plugin/imfilebrowser.h"
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void WindowResizeHandler(GLFWwindow* window, int w, int h);

// Functions

GLFWwindow* CreateGLFWWindow();
void SetupImgui(GLFWwindow* window);
unsigned int loadTexture(const char* path);


// camera
Camera cam(glm::vec3(0.0f, 0.0f, 500.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	45,
	0,
	-90
);

int Width = 1080;
int Height = 1080;


float lastX = Width / 2, lastY = Height / 2;


bool firstMouse = true;

double deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

bool ShowModel = true;

#define POINT_LIGHTS_NUM 4


//


int main()
{

	GLFWwindow* window = CreateGLFWWindow();

	//SetupImgui( window);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
	

	Shader SimpleShader("VertexShader.vert", "FragmentShader.frag");
	Shader LightShader("SkeletonVertexShader.vert", "LightFragmentShader.frag");

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


	// Light cube object
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};
	
	GameObject lights[POINT_LIGHTS_NUM] = { GameObject(vertices),GameObject(vertices) ,GameObject(vertices) ,GameObject(vertices) };
	for (int i = 0; i < POINT_LIGHTS_NUM; i++)
	{
		lights[i].SetPosition(pointLightPositions[i]);
	}

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);


	/************** Loading model and Animations **************/
	
	const std::string FBXResourcePath = "resources/objects/Archer/Yelling While Standing.fbx";
	const std::string animationPath = "resources/objects/Archer/Dying.fbx";
	// Load the model
	SkeletalModel ourModel(FBXResourcePath);
	std::cout << "Model Loaded" << std::endl;

	// Load the animation Clip
	AnimationClipManager animationClipManager(animationPath, ourModel.GetSkeleton());
	if (animationClipManager.GetLoadedAnimationClips().empty())
	{
		std::cout << "Please load an animation";
		return 0;
	}

	// Get an animation from anim manager and pass it to animation
	// this way we could easily create state machines. Of course in its specific class

	int animationSelector = 0;
	
	AnimationClip* anim = animationClipManager.GetLoadedAnimationClips()[animationSelector];
	Animator animator(animationClipManager.GetSkeleton(), *anim, glfwGetTime());


	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetWindowSizeCallback(window, WindowResizeHandler);

	
	glEnable(GL_DEPTH_TEST);
	static double limitFPS = 1.0 / 60.0;

	const double fpsLimit = 1.0 / 60.0;
	double lastUpdateTime = 0;  // number of seconds since the last loop
	double lastFrameTime = 0;   // number of seconds since the last frame


	// create a file browser instance
	ImGui::FileBrowser fileDialog;

	// (optional) set browser properties
	fileDialog.SetTitle("Select an animation");
	fileDialog.SetTypeFilters({ ".fbx" });

	
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		const double now = glfwGetTime();
		deltaTime = now - lastUpdateTime;
		// input

		glfwPollEvents();

		processInput(window);

		// update your application logic here,
		// using deltaTime if necessary (for physics, tweening, etc.)

		// Updating animations
		animator.Update(deltaTime);

		// This if-statement only executes once every 60th of a second


		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Draw the UI
		ImGui::ShowDemoWindow();

		// Create a window called "My First Tool", with a menu bar.
		ImGui::Begin("Animation Controller");
	
		UIFunctions::DrawSkeletonTreeHelper(*(ourModel.GetSkeleton().GetRootBone()));

		if (ImGui::Button("Next Anim"))
		{
			animationSelector++;
			if(animationSelector >= animationClipManager.GetLoadedAnimationClips().size())
			{
				animationSelector = 0;
			}
			AnimationClip* anim = animationClipManager.GetLoadedAnimationClips()[animationSelector];
			animator.ChangeAnimationClip(*anim, glfwGetTime());
		}


		ImGui::End();

		// Getting new animation from user
		
		
		UIFunctions::AddNewAnimationUI(fileDialog); // dialogue so that the user can choose the animation
		
		if (fileDialog.HasSelected()) 
		{
			std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
			animationClipManager.AddNewAnimationClip(fileDialog.GetSelected().string());
			fileDialog.ClearSelected();
		}

		ImGui::Render();
		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}


		
		if ((now - lastFrameTime) >= fpsLimit)
		{

			// draw your frame here
			// rendering commands

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe

			// render the mesh
			SimpleShader.use(); // don't forget to activate/use the shader before setting uniforms!
								// either set it manually like so:

			// material
			SimpleShader.SetFloat("material.shininess", 32.0f);

			// camera view
			SimpleShader.SetVec3("viewPosition", cam.GetPosition());

			// light

			// directional light
			SimpleShader.SetVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
			SimpleShader.SetVec3("dirLight.ambient", glm::vec3(0.4f, 0.4f, 0.4f));
			SimpleShader.SetVec3("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.f));
			SimpleShader.SetVec3("dirLight.specular", glm::vec3(1.5f, 1.5f, 1.5f));
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
			SimpleShader.SetVec3("spotLight.position", cam.GetPosition());
			SimpleShader.SetVec3("spotLight.direction", cam.GetForward());
			SimpleShader.SetVec3("spotLight.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
			SimpleShader.SetVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
			SimpleShader.SetVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
			SimpleShader.SetFloat("spotLight.constant", 1.0f);
			SimpleShader.SetFloat("spotLight.linear", 0.09f);
			SimpleShader.SetFloat("spotLight.quadratic", 0.032f);
			SimpleShader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(55.5f)));
			SimpleShader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(65.0f)));


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

			//Draw Skeleton Joints 
			//ourModel.DrawSkeletonJoints(LightShader);

			for (int i = 0; i < POINT_LIGHTS_NUM; i++)
			{
				lights[i].Render(LightShader);
			}


			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			

			
			// check and call events and swap buffers
			glfwSwapBuffers(window);


			// only set lastFrameTime when you actually draw something
			lastFrameTime = now;
		}

		
		

		// set lastUpdateTime every iteration
		lastUpdateTime = now;




	}
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	
	return 0;
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
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

	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
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
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	
	float speed = cam.GetSpeed();
	speed += (float)yoffset * 10;
	/*if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;*/

	cam.SetSpeed(speed);
}



GLFWwindow* CreateGLFWWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(Width, Height, "Skeletal Animation", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		//return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
	
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed To initialize GLAD" << std::endl;
		//return -1;
	}

	glViewport(0, 0, Width, Height);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	return window;
}

void SetupImgui(GLFWwindow* window)
{
	
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


