#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/Core/imgui.h"
#include "imgui/Core/imgui_impl_glfw.h"
#include "imgui/Core/imgui_impl_opengl3.h"

#include "Camera.h"
#include "Shader.h"
#include "STB/stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "GameObject.h"
#include <vector>
#include <iostream>
#include <glm/gtx/matrix_decompose.hpp>

#include "Model.h"
#include "Mesh.h"
#include "SkeletalModel.h"
#include <glm/gtx/string_cast.hpp>
#include "UIFunctions.h"
#include "Animation/Animation.h"
#include "Animation/Animator.h"
#include "Animation/AnimationClipManager.h"
#include "imgui/Plugin/imfilebrowser.h"
#include "CubeMap.h"


#include "imgui/ImguiHandler.h"
#include "ModelData.h"
#include "StateMachine/StateMachine.h"
struct DestroyglfwWin {

	void operator()(GLFWwindow* ptr) {
		glfwDestroyWindow(ptr);
	}

};

float archerSpeed = 0;
void processInput(GLFWwindow* window);
void processInput(GLFWwindow* window, SkeletalModelGameObject* gameObject);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void WindowResizeHandler(GLFWwindow* window, int w, int h);

// Functions

std::unique_ptr<GLFWwindow, DestroyglfwWin> CreateGLFWWindow();
unsigned int loadTexture(const char* path);


bool wolfMode = false;

// camera
Camera cam(glm::vec3(0.0f, 0.0f, 500.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	45,
	0,
	-90,
	50
);

int Width = 1080;
int Height = 1080;

bool freeView = false;

float lastX = Width / 2, lastY = Height / 2;

bool firstMouse = true;

double deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

bool ShowModel = true;

#define POINT_LIGHTS_NUM 4

bool walking = false;

bool running = false;


SkeletalModelGameObject* gameObjectPointer = nullptr;



int main()
{

	std::unique_ptr<GLFWwindow, DestroyglfwWin> window = CreateGLFWWindow();

	std::unique_ptr<ImguiHandler> imguiHandler = std::make_unique<ImguiHandler>(*window) ;
	
	/*
	 * Note: we have to use the absolute path 
	 */
	Shader SimpleShader("E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Sources/Shaders/Vertex/VertexShader.vert",
						"E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Sources/Shaders/Fragment/FragmentShader.frag");
	
	Shader LightShader("E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Sources/Shaders/Vertex/SkeletonVertexShader.vert",
				       "E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Sources/Shaders/Fragment/LightFragmentShader.frag");

	Shader CubeMapShader("E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Sources/Shaders/Vertex/SkyBoxVertexShader.vert",
		"E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Sources/Shaders/Fragment/SkyBoxFragmentShader.frag");

	CubeMap cubeMap("Resources/CubeMap/skybox");
	
	// set upVector vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	glm::vec3 rotationVector = glm::vec3(1.f, 0.f, 0.1f);
	float envAngle = glm::radians( 270.f);
	// Light cube object
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  100.f,  2.0f),
		glm::vec3(2.3f, 100.f, -4.0f),
		glm::vec3(-4.0f,  100.f, -12.0f),
		glm::vec3(0.0f,  100.f, -53.0f)
	};
	
	/*GameObject lights[POINT_LIGHTS_NUM] = { GameObject(vertices),GameObject(vertices) ,GameObject(vertices) ,GameObject(vertices) };
	for (int i = 0; i < POINT_LIGHTS_NUM; i++)
	{
		lights[i].SetPosition(pointLightPositions[i]);
	}*/

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);


	/************** Loading model and Animations **************/

	/*
	 * Importing archer
	 */

	const std::string animationPath = "Resources/objects/Archer/Animations/idle.fbx";
	const std::string FBXResourcePath = "Resources/objects/Archer/Yelling While Standing.fbx";
	
	SkeletalModel* ourSkeletalModel = new SkeletalModel(FBXResourcePath);
	SkeletalModelGameObject* archer = new SkeletalModelGameObject(ourSkeletalModel);
	gameObjectPointer = archer;
	AnimationClipManager animationClipManager(animationPath, ourSkeletalModel->GetSkeleton());
	
	glm::vec3 currentArcherPosition = archer->GetPosition();
	archer->SetPosition(currentArcherPosition + glm::vec3(90, 93, 90));
	archer->SetRotation(0, glm::vec3(0.f, 1.f, 0.f));
	
	animationClipManager.AddNewAnimationClip("Resources/objects/Archer/animations/walking.fbx");
	animationClipManager.AddNewAnimationClip("Resources/objects/Archer/animations/Jog Forward.fbx");
	animationClipManager.AddNewAnimationClip("Resources/objects/Archer/animations/idle.fbx");
	animationClipManager.AddNewAnimationClip("Resources/objects/Archer/animations/Waving.fbx");

	// Get an animation from anim manager and pass it to animation
	// this way we could easily create state machines. Of course in its specific class

	//int animationSelector = 0;
	
	AnimationClip* anim = animationClipManager.GetLoadedAnimationClips()[0];


	//TODO animator constructor should be changed so it does not take any animations;

	
	Animator* animator = new Animator(animationClipManager.GetSkeleton(), *anim, glfwGetTime());  

	AnimationState* IDLE_STATE = new AnimationState("IDLE",animationClipManager.GetLoadedAnimationClips()[0]); 
	Transition* tranIdleToWalk = new Transition("WALK", []() { return (!wolfMode) && walking; }, 0.13);
	IDLE_STATE->AddNewTransition(tranIdleToWalk); 
	
	AnimationState* Walk_STATE = new AnimationState("WALK", animationClipManager.GetLoadedAnimationClips()[1]); 
	Transition* tranWalkToIdle = new Transition("IDLE", []() {return (!wolfMode) && !walking; }, 0.35);
	Walk_STATE->AddNewTransition(tranWalkToIdle);
	Transition* tranWalkToRun = new Transition("RUN", []() {return (!wolfMode) && running && walking; }, 0.13);
	Walk_STATE->AddNewTransition(tranWalkToRun);
	
	AnimationState* RUN_STATE = new AnimationState("RUN", animationClipManager.GetLoadedAnimationClips()[2]); 
	Transition* tranRunToWalk = new Transition("WALK", []() {return (!wolfMode) && ((!running) || (!walking)); }, 0.13);
	RUN_STATE->AddNewTransition(tranRunToWalk);
	
	AnimationStateMachine animState(animator, IDLE_STATE);
	animState.AddNewState(Walk_STATE);
	animState.AddNewState(RUN_STATE);





	/*
	 * Importing the environment
	 */
	const std::string environmentPath = "Resources/objects/Castle/untitled.fbx_Scen3e.fbx_Scene.fbx";

	Model* environmentModel = new Model(environmentPath);
	ModelGameObject* environmentObject = new ModelGameObject(environmentModel);
	environmentObject->SetRotation(0, glm::vec3(0.f, 1.f, 0.f));

	/*
	 * Importing Elephant
	 */
	const std::string elephantPath = "Resources/objects/Elephant/Elephant.fbx";
	SkeletalModel* elephantModel = new SkeletalModel(elephantPath);
	SkeletalModelGameObject* elephantGameObject = new SkeletalModelGameObject(elephantModel);

	AnimationClipManager elephantClipManager(elephantPath, elephantModel->GetSkeleton());

	elephantGameObject->SetScale(glm::vec3(3, 3, 3));
	elephantGameObject->SetPosition(elephantGameObject->GetPosition() + glm::vec3(500, 0, 500));

	Animator* elephantAnimator = new Animator(elephantClipManager.GetSkeleton(), *(elephantClipManager.GetLoadedAnimationClips()[0]), glfwGetTime());

	/*
	 * Importing Wolf
	 */
	const std::string wolfPath = "Resources/objects/Wolf/wp.fbx";
	SkeletalModel* wolfModel = new SkeletalModel(wolfPath);
	SkeletalModelGameObject* wolfGameObject = new SkeletalModelGameObject(wolfModel);

	AnimationClipManager wolfClipManager(wolfPath, wolfModel->GetSkeleton());

	wolfGameObject->SetScale(glm::vec3(30, 30, 30));
	//wolfGameObject->SetRotation(90, glm::vec3(0.f, 0.f, 1.f));

	glm::vec3 currentWolfPosition = wolfGameObject->GetPosition();

	wolfGameObject->SetPosition(currentWolfPosition + glm::vec3(0, 55, 0));

	Animator* wolfAnimator = new Animator(wolfClipManager.GetSkeleton(), *(wolfClipManager.GetLoadedAnimationClips()[13]), glfwGetTime());

	AnimationState* wolfIDLE_STATE = new AnimationState("IDLE", wolfClipManager.GetLoadedAnimationClips()[13]);
	Transition* wolftranIdleToWalk = new Transition("WALK", []() { return wolfMode && walking; }, 0.13);
	wolfIDLE_STATE->AddNewTransition(wolftranIdleToWalk);

	AnimationState* wolfWalk_STATE = new AnimationState("WALK", wolfClipManager.GetLoadedAnimationClips()[11]);
	Transition* wolftranWalkToIdle = new Transition("IDLE", []() {return wolfMode && !walking; }, 0.35);
	wolfWalk_STATE->AddNewTransition(wolftranWalkToIdle);
	Transition* wolftranWalkToRun = new Transition("RUN", []() {return wolfMode &&  running && walking; }, 0.13);
	wolfWalk_STATE->AddNewTransition(wolftranWalkToRun);

	AnimationState* wolfRUN_STATE = new AnimationState("RUN", wolfClipManager.GetLoadedAnimationClips()[10]);
	Transition* wolftranRunToWalk = new Transition("WALK", []() {return wolfMode && ((!running) || (!walking)); }, 0.13);
	wolfRUN_STATE->AddNewTransition(wolftranRunToWalk);

	AnimationStateMachine wolfAnimState(wolfAnimator, wolfIDLE_STATE);
	wolfAnimState.AddNewState(wolfWalk_STATE);
	wolfAnimState.AddNewState(wolfRUN_STATE);
	wolfAnimState.Update(deltaTime);
	

	glfwSetCursorPosCallback(window.get(), mouse_callback);
	glfwSetScrollCallback(window.get(), scroll_callback);
	glfwSetWindowSizeCallback(window.get(), WindowResizeHandler);

	
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

	while (!glfwWindowShouldClose(window.get()))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		const double now = glfwGetTime();
		deltaTime = now - lastUpdateTime;
		// input

		glfwPollEvents();

		if(!wolfMode)
		{
			gameObjectPointer = archer;
			processInput(window.get(), archer);
		}
		else
		{
			gameObjectPointer = wolfGameObject;
			processInput(window.get(),wolfGameObject);
		}
		
		if(!freeView)
		{

			if(!wolfMode)
			{
				glm::vec3 finalCamPosition = archer->GetPosition() + cam.GetForward() * -200.0f + cam.GetUp() * 60.f;
				glm::vec3 currentCamPosition = cam.GetPosition();
				glm::vec3 camPosition = glm::mix(currentCamPosition, finalCamPosition, glm::min(float(deltaTime * 4), 1.f));
				cam.SetPosition(camPosition);
			}
			else
			{
				glm::vec3 finalCamPosition = wolfGameObject->GetPosition() + cam.GetForward() * -200.0f + cam.GetUp() * 60.f;
				glm::vec3 currentCamPosition = cam.GetPosition();
				glm::vec3 camPosition = glm::mix(currentCamPosition, finalCamPosition, glm::min(float(deltaTime * 4), 1.f));
				cam.SetPosition(camPosition);
			}
			
		}

		
		// update your application logic here,
		// using deltaTime if necessary (for physics, tweening, etc.)

		// Updating animations

		if(!wolfMode)
		{
			
		}
		else
		{
			
		}
		animState.Update(deltaTime);
		wolfAnimState.Update(deltaTime);
		elephantAnimator->Update(deltaTime);
		
		
		// This if-statement only executes once every 60th of a second


		imguiHandler->BeginFrame();

		// Draw the UI
		ImGui::ShowDemoWindow();

		// Create a window called "My First Tool", with a menu bar.
		ImGui::Begin("Animation Controller");

		if(ourSkeletalModel->GetSkeleton().GetRootBone())
			UIFunctions::DrawSkeletonTreeHelper(*(ourSkeletalModel->GetSkeleton().GetRootBone()));

		ImGui::End();

		// Getting new animation from user


		UIFunctions::AddNewAnimationUI(fileDialog); // dialogue so that the user can choose the animation

		if (fileDialog.HasSelected())
		{
			std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
			animationClipManager.AddNewAnimationClip(fileDialog.GetSelected().string());
			fileDialog.ClearSelected();
		}
		
		UIFunctions::ChangeRotationVector(rotationVector, envAngle);
		
		imguiHandler->EndFrame();
		
		

		
		if ((now - lastFrameTime) >= fpsLimit)
		{

			// draw your frame here
			// rendering commands

			glClearColor(0.7f, 0.7f,0.7f, 1.0f);
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
			SimpleShader.SetVec3("spotLight.position", pointLightPositions[3]);
			SimpleShader.SetVec3("spotLight.direction", pointLightPositions[3]);
			SimpleShader.SetVec3("spotLight.ambient", glm::vec3(0, 0.f, 0.f));
			SimpleShader.SetVec3("spotLight.diffuse", glm::vec3(0.f, 0.f, 0.f));
			SimpleShader.SetVec3("spotLight.specular", glm::vec3(0.f, 0.f, 0.f));
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
			
			if (ShowModel)
			{
				archer->Draw(SimpleShader);
			}
			wolfGameObject->Draw(SimpleShader);
			elephantGameObject->Draw(SimpleShader);
			environmentObject->Draw(SimpleShader);

			// LightShader
			LightShader.use();
			LightShader.SetMat4("view", view);
			LightShader.SetMat4("projection", projection);

			//Draw Skeleton Joints 
			//ourSkeletalModel->DrawSkeletonJoints(LightShader);
			/*for (int i = 0; i < POINT_LIGHTS_NUM; i++)
			{
				lights[i].Render(LightShader);
			}*/
			glDepthFunc(GL_LEQUAL);

			view = glm::mat4(glm::mat3(cam.GetViewMatrix()));
			CubeMapShader.use();
			CubeMapShader.SetMat4("view", view);
			CubeMapShader.SetMat4("projection", projection);

			cubeMap.Draw(CubeMapShader);
			glDepthFunc(GL_LESS);
			
			
			imguiHandler->Render();
			
			// check and call events and swap buffers
			glfwSwapBuffers(window.get());


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


	glfwTerminate();
	
	return 0;
}

void processInput(GLFWwindow* window)
{
	ImGuiIO& io = ImGui::GetIO();

	walking = ImGui::IsKeyDown(ImGuiKey_Y);
	running = ImGui::IsKeyDown(ImGuiKey_LeftShift);
	
	if (ImGui::IsKeyPressed(ImGuiKey_H, false))
	{
		ShowModel = !ShowModel;
	}


	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
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

void processInput(GLFWwindow* window, SkeletalModelGameObject* gameObject)
{

	ImGuiIO& io = ImGui::GetIO();

	walking = ImGui::IsKeyDown(ImGuiKey_Y);
	running = ImGui::IsKeyDown(ImGuiKey_LeftShift);

	archerSpeed = 0;
	if (walking)
	{
		if(!wolfMode)
		{
			archerSpeed = 5;
		}
		else
		{
			archerSpeed = 3;
		}
		
	}
	if (running && walking)
	{
		if (!wolfMode)
		{
			archerSpeed = 7;
		}
		else
		{
			archerSpeed = 10;
		}
	}

	gameObject->Translate(normalize(gameObject->GetDirection()), archerSpeed);

	if (ImGui::IsKeyPressed(ImGuiKey_H, false))
	{
		ShowModel = !ShowModel;
	}

	if (ImGui::IsKeyPressed(ImGuiKey_F, false))
	{
		freeView = !freeView;
	}

	if (ImGui::IsKeyPressed(ImGuiKey_K, false))
	{
		wolfMode = !wolfMode;
	}


	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if(freeView)
	{
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

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	ImGuiIO& io = ImGui::GetIO();
	
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

	
	if (firstMouse) // initially set to true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}


	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
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
	
	float angle = gameObjectPointer->GetAngle();
	angle -= xoffset;

	float characterAngle = glm::degrees(atan2(cam.GetForward().x, cam.GetForward().z));

	gameObjectPointer->SetAngle(characterAngle);
	
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



std::unique_ptr<GLFWwindow, DestroyglfwWin> CreateGLFWWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//std::unique_ptr<GLFWwindow> window = std::make_unique<GLFWwindow>()
	GLFWwindow* window = glfwCreateWindow(Width, Height, "Skeletal Animation", NULL, NULL);
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

	glViewport(0, 0, Width, Height);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	return std::unique_ptr<GLFWwindow, DestroyglfwWin>(window);
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



