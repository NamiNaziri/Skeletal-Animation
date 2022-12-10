#include "Application.h"

#include <STB/stb_image.h>

#include "UIFunctions.h"
#include "Animation/AnimationClipManager.h"
#include "imgui/Core/imgui.h"
#include "imgui/Core/imgui_impl_glfw.h"
#include "imgui/Core/imgui_impl_opengl3.h"
#include "imgui/Plugin/imfilebrowser.h"

Application::Application() :
	graphicalWindow(Width,Height),
	pointLightPositions{
		glm::vec3(0.7f,  100.f,  2.0f),
		glm::vec3(2.3f, 100.f, -4.0f),
		glm::vec3(-4.0f,  100.f, -12.0f),
		glm::vec3(0.0f,  100.f, -53.0f)
	}
{
}

Application::~Application()
{
	delete archerSkeletalModel;
	delete archer;
	delete archerClipManager;
	delete archerAnimator;
	delete archerAnimState;

	
	delete environmentModel;
	delete environmentObject;

	delete elephantModel;
	delete elephantGameObject;
	delete elephantClipManager;
	delete elephantAnimator;


	delete wolfModel;
	delete wolfGameObject;
	delete wolfClipManager ;
	delete wolfAnimator;
	delete wolfAnimState;

}

void Application::Start()
{
    this->Init();
    this->Update();
}

void Application::Init()
{
    imguiHandler = std::make_unique<ImguiHandler>(*(graphicalWindow.GetWindow()));
    ImportShaders();
	ImportModels();
	glfwSetCursorPosCallback( graphicalWindow.GetWindow().get(), mouse_callback);
	glfwSetScrollCallback(graphicalWindow.GetWindow().get(), scroll_callback);
	glfwSetWindowSizeCallback(graphicalWindow.GetWindow().get(), WindowResizeHandler);

	glEnable(GL_DEPTH_TEST);

	// (optional) set browser properties
	fileDialog.SetTitle("Select an animation");
	fileDialog.SetTypeFilters({ ".fbx" });
	
}

void Application::Update()
{
	while (!glfwWindowShouldClose(graphicalWindow.GetWindow().get()))
	{

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		const double now = glfwGetTime();
		deltaTime = now - lastUpdateTime;

		
		ProcessInput();
		SetCameraPosition();
		UpdateGameLogic();
		UpdateAnimation();
		UpdateUI();


		if ((now - lastFrameTime) >= fpsLimit)
		{

			Render();
			
			// check and call events and swap buffers
			glfwSwapBuffers(graphicalWindow.GetWindow().get());

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
}

void Application::ProcessInput()
{
	glfwPollEvents();

	if(!wolfMode)
	{
		gameObjectPointer = archer;
		processInput(graphicalWindow.GetWindow().get(), archer);
	}
	else
	{
		gameObjectPointer = wolfGameObject;
		processInput(graphicalWindow.GetWindow().get(),wolfGameObject);
	}
}

void Application::SetCameraPosition()
{
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
}

void Application::UpdateGameLogic()
{
}

void Application::UpdateAnimation()
{
	archerAnimState->Update(deltaTime);
	wolfAnimState->Update(deltaTime);
	elephantAnimator->Update(deltaTime);
		
}

void Application::UpdateUI()
{
	imguiHandler->BeginFrame();

	// Draw the UI
	ImGui::ShowDemoWindow();

	// Create a window called "My First Tool", with a menu bar.
	ImGui::Begin("Animation Controller");

	if(archerSkeletalModel->GetSkeleton().GetRootBone())
		UIFunctions::DrawSkeletonTreeHelper(*(archerSkeletalModel->GetSkeleton().GetRootBone()));

	ImGui::End();

	// Getting new animation from user


	UIFunctions::AddNewAnimationUI(fileDialog); // dialogue so that the user can choose the animation

	if (fileDialog.HasSelected())
	{
		std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
		archerClipManager->AddNewAnimationClip(fileDialog.GetSelected().string());
		fileDialog.ClearSelected();
	}
		//TODO
	UIFunctions::ChangeRotationVector(rotationVector, envAngle);
		
	imguiHandler->EndFrame();
}

void Application::Render()
{
	glClearColor(0.7f, 0.7f,0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe
	UpdateSimpleShader();
	if (ShowModel)
	{
		archer->Draw(SimpleShader);
	}
	wolfGameObject->Draw(SimpleShader);
	elephantGameObject->Draw(SimpleShader);
	environmentObject->Draw(SimpleShader);

	UpdateLightShader();
	//Draw Skeleton Joints 
	//ourSkeletalModel->DrawSkeletonJoints(LightShader);
	/*for (int i = 0; i < POINT_LIGHTS_NUM; i++)
	{
		lights[i].Render(LightShader);
	}*/
	glDepthFunc(GL_LEQUAL);

	RenderCubeMap();
	glDepthFunc(GL_LESS);

	imguiHandler->Render();
}

void Application::UpdateSimpleShader()
{
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
}

void Application::UpdateLightShader()
{
	glm::mat4 view = cam.GetViewMatrix();
	glm::mat4 projection = glm::mat4(1);
	projection = glm::perspective(glm::radians(cam.GetFOV()), ((float)Width / (float)Height), 0.1f, 50000.0f);

	
	LightShader.use();
	LightShader.SetMat4("view", view);
	LightShader.SetMat4("projection", projection);
}

void Application::RenderCubeMap()
{
	
	glm::mat4 view = glm::mat4(glm::mat3(cam.GetViewMatrix()));
	glm::mat4 projection = glm::mat4(1);
	projection = glm::perspective(glm::radians(cam.GetFOV()), ((float)Width / (float)Height), 0.1f, 50000.0f);
			
	CubeMapShader.use();
	CubeMapShader.SetMat4("view", view);
	CubeMapShader.SetMat4("projection", projection);

	cubeMap.Draw(CubeMapShader);
	
}

void Application::ImportShaders()
{
    SimpleShader.SetShaderPath("E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Sources/Shaders/Vertex/VertexShader.vert",
                    "E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Sources/Shaders/Fragment/FragmentShader.frag");
	
    LightShader.SetShaderPath("E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Sources/Shaders/Vertex/SkeletonVertexShader.vert",
                       "E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Sources/Shaders/Fragment/LightFragmentShader.frag");

    CubeMapShader.SetShaderPath("E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Sources/Shaders/Vertex/SkyBoxVertexShader.vert",
        "E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Sources/Shaders/Fragment/SkyBoxFragmentShader.frag");

    cubeMap.SetCubeMapPath("E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Resources/CubeMap/skybox");
}

void Application::ImportModels()
{
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

	const std::string archerAnimationPath = "Resources/objects/Archer/Animations/idle.fbx";
	const std::string archerFBXResourcePath = "Resources/objects/Archer/Yelling While Standing.fbx";
	
	archerSkeletalModel = new SkeletalModel(archerFBXResourcePath);
	archer = new SkeletalModelGameObject(archerSkeletalModel);
	gameObjectPointer = archer;
	archerClipManager = new AnimationClipManager(archerAnimationPath, archerSkeletalModel->GetSkeleton());
	
	glm::vec3 currentArcherPosition = archer->GetPosition();
	archer->SetPosition(currentArcherPosition + glm::vec3(90, 93, 90));
	archer->SetRotation(0, glm::vec3(0.f, 1.f, 0.f));
	
	archerClipManager->AddNewAnimationClip("E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Resources/objects/Archer/animations/walking.fbx");
	archerClipManager->AddNewAnimationClip("E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Resources/objects/Archer/animations/Jog Forward.fbx");
	archerClipManager->AddNewAnimationClip("E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Resources/objects/Archer/animations/idle.fbx");
	archerClipManager->AddNewAnimationClip("E:/Graphics/OpenGL/GitRepo/OpenGL/OpenGL/Resources/objects/Archer/animations/Waving.fbx");

	// Get an animation from anim manager and pass it to animation
	// this way we could easily create state machines. Of course in its specific class

	//int animationSelector = 0;
	
	AnimationClip* anim = archerClipManager->GetLoadedAnimationClips()[0];


	//TODO animator constructor should be changed so it does not take any animations;

	
	archerAnimator = new Animator(archerClipManager->GetSkeleton(), *anim, glfwGetTime());  

	AnimationState* ARCHER_IDLE_STATE = new AnimationState("IDLE",archerClipManager->GetLoadedAnimationClips()[0]); 
	Transition* archerTranIdleToWalk = new Transition("WALK", [this]() { return (!wolfMode) && walking; }, 0.13);
	ARCHER_IDLE_STATE->AddNewTransition(archerTranIdleToWalk); 
	
	AnimationState* ARCHER_WALK_STATE = new AnimationState("WALK", archerClipManager->GetLoadedAnimationClips()[1]); 
	Transition* archerTranWalkToIdle = new Transition("IDLE", [this]() {return (!wolfMode) && !walking; }, 0.35);
	ARCHER_WALK_STATE->AddNewTransition(archerTranWalkToIdle);
	Transition* archerTranWalkToRun = new Transition("RUN", [this]() {return (!wolfMode) && running && walking; }, 0.13);
	ARCHER_WALK_STATE->AddNewTransition(archerTranWalkToRun);
	
	AnimationState* ARCHER_RUN_STATE = new AnimationState("RUN", archerClipManager->GetLoadedAnimationClips()[2]); 
	Transition* archerTranRunToWalk = new Transition("WALK", [this]() {return (!wolfMode) && ((!running) || (!walking)); }, 0.13);
	ARCHER_RUN_STATE->AddNewTransition(archerTranRunToWalk);
	
	archerAnimState = new AnimationStateMachine(archerAnimator, ARCHER_IDLE_STATE);
	archerAnimState->AddNewState(ARCHER_WALK_STATE);
	archerAnimState->AddNewState(ARCHER_RUN_STATE);



	/*
	* Importing the environment
	*/
	const std::string environmentPath = "Resources/objects/Castle/untitled.fbx_Scen3e.fbx_Scene.fbx";

	environmentModel = new Model(environmentPath);
	environmentObject = new ModelGameObject(environmentModel);
	environmentObject->SetRotation(0, glm::vec3(0.f, 1.f, 0.f));


	/*
	 * Importing Elephant
	 */
	const std::string elephantPath = "Resources/objects/Elephant/Elephant.fbx";
	elephantModel = new SkeletalModel(elephantPath);
	elephantGameObject = new SkeletalModelGameObject(elephantModel);

	elephantClipManager = new AnimationClipManager (elephantPath, elephantModel->GetSkeleton());

	elephantGameObject->SetScale(glm::vec3(3, 3, 3));
	elephantGameObject->SetPosition(elephantGameObject->GetPosition() + glm::vec3(500, 0, 500));

	elephantAnimator = new Animator(elephantClipManager->GetSkeleton(), *(elephantClipManager->GetLoadedAnimationClips()[0]), glfwGetTime());






	const std::string wolfPath = "Resources/objects/Wolf/wp.fbx";
	wolfModel = new SkeletalModel(wolfPath);
	wolfGameObject = new SkeletalModelGameObject(wolfModel);

	wolfClipManager = new AnimationClipManager(wolfPath, wolfModel->GetSkeleton());

	wolfGameObject->SetScale(glm::vec3(30, 30, 30));
	//wolfGameObject->SetRotation(90, glm::vec3(0.f, 0.f, 1.f));

	glm::vec3 currentWolfPosition = wolfGameObject->GetPosition();

	wolfGameObject->SetPosition(currentWolfPosition + glm::vec3(0, 55, 0));

	wolfAnimator = new Animator(wolfClipManager->GetSkeleton(), *(wolfClipManager->GetLoadedAnimationClips()[13]), glfwGetTime());

	AnimationState* wolfIDLE_STATE = new AnimationState("IDLE", wolfClipManager->GetLoadedAnimationClips()[13]);
	Transition* wolftranIdleToWalk = new Transition("WALK", [this]() { return wolfMode && walking; }, 0.13);
	wolfIDLE_STATE->AddNewTransition(wolftranIdleToWalk);

	AnimationState* wolfWalk_STATE = new AnimationState("WALK", wolfClipManager->GetLoadedAnimationClips()[12]);
	Transition* wolftranWalkToIdle = new Transition("IDLE", [this]() {return wolfMode && !walking; }, 0.35);
	wolfWalk_STATE->AddNewTransition(wolftranWalkToIdle);
	Transition* wolftranWalkToRun = new Transition("RUN", [this]() {return wolfMode &&  running && walking; }, 0.13);
	wolfWalk_STATE->AddNewTransition(wolftranWalkToRun);

	AnimationState* wolfRUN_STATE = new AnimationState("RUN", wolfClipManager->GetLoadedAnimationClips()[10]);
	Transition* wolftranRunToWalk = new Transition("WALK", [this]() {return wolfMode && ((!running) || (!walking)); }, 0.13);
	wolfRUN_STATE->AddNewTransition(wolftranRunToWalk);

	wolfAnimState = new AnimationStateMachine(wolfAnimator, wolfIDLE_STATE);
	wolfAnimState->AddNewState(wolfWalk_STATE);
	wolfAnimState->AddNewState(wolfRUN_STATE);
	wolfAnimState->Update(deltaTime);
	
}

void Application::processInput(GLFWwindow* window)
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

void Application::processInput(GLFWwindow* window, SkeletalModelGameObject* gameObject)
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
			archerSpeed = 2;
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

void Application::mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

void Application::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
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

void Application::WindowResizeHandler(GLFWwindow* window, int w, int h)
{
	std::cout << "Window Resized. W: " << w << "  H: " << h << std::endl;
	Width = w;
	Height = h;
}


Camera Application::cam(glm::vec3(0.0f, 0.0f, 500.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	45,
	0,
	-90,
	50
);