#pragma once
#include "Camera.h"
#include "CubeMap.h"
#include "GameObject.h"
#include "GraphicalWindow.h"
#include "Shader.h"
#include "Animation/AnimationClipManager.h"
#include "Animation/Animator.h"
#include "Animation/StateMachine/StateMachine.h"
#include "imgui/Plugin/imfilebrowser.h"

class Application
{
public:
    Application();
    ~Application();
    void Start();


private:
    void Init();
    void Update();
    void ProcessInput();
    void SetCameraPosition();
    void UpdateGameLogic();
    void UpdateAnimation();
    void UpdateUI();
    void Render();
    void UpdateSimpleShader();
    void UpdateLightShader();
    void RenderCubeMap();

    void ImportShaders();
    void ImportModels();

    void processInput(GLFWwindow* window);
    void processInput(GLFWwindow* window, SkeletalModelGameObject* gameObject);


    // Callbacks
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void WindowResizeHandler(GLFWwindow* window, int w, int h);

private:


    GraphicalWindow graphicalWindow;
    std::unique_ptr<ImguiHandler> imguiHandler;


    /*Shaders*/

    Shader SimpleShader;
    Shader LightShader;
    Shader CubeMapShader;
    CubeMap cubeMap;

    static Camera cam;

    inline static SkeletalModelGameObject* gameObjectPointer = nullptr;


    /*Archer*/

    //TODO create character class that contains the character game object !? and the state machine.
    
    

    SkeletalModel* archerSkeletalModel;
    SkeletalModelGameObject* archer;
    AnimationClipManager* archerClipManager;
    Animator* archerAnimator;
    AnimationStateMachine* archerAnimState;


    Model* environmentModel;
    ModelGameObject* environmentObject;


    SkeletalModel* elephantModel;
    SkeletalModelGameObject* elephantGameObject;
    AnimationClipManager* elephantClipManager;
    Animator* elephantAnimator;


    SkeletalModel* wolfModel;
    SkeletalModelGameObject* wolfGameObject;
    AnimationClipManager* wolfClipManager ;
    Animator* wolfAnimator;
    AnimationStateMachine* wolfAnimState;





    /*Game Logic*/

    bool wolfMode = false;

    bool walking = false;

    bool running = false;

    float archerSpeed = 0;


    /* Application Logic */
    inline static int Width = 1080;
    inline static int Height = 1080;
    double deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    bool freeView = false;

    bool ShowModel = true;

    inline static float lastX = Width / 2;
    inline static float lastY = Height / 2;

    inline static bool firstMouse = true;
    
    const double fpsLimit = 1.0 / 60.0;
    double lastUpdateTime = 0;  // number of seconds since the last loop
    double lastFrameTime = 0;   // number of seconds since the last frame

    // create a file browser instance
    ImGui::FileBrowser fileDialog;

    // used for UI stuff 
    glm::vec3 rotationVector = glm::vec3(1.f, 0.f, 0.1f);
    float envAngle = glm::radians( 270.f);

    glm::vec3 pointLightPositions[];
};


