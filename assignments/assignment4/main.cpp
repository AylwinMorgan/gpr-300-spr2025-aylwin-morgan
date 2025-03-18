#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/ext.hpp>

#include <ew/shader.h>
#include <ew/model.h>
#include <ew/camera.h>
#include <ew/transform.h>
#include <ew/cameraController.h>
#include <ew/texture.h>

#include "../assignment4/assets/animation.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();
void renderScene(const ew::Shader &shader, const glm::mat4 monkeyMatrix, ew::Model monkeyModel, float time);

void renderCube();

float lerp(float a, float b, float t);
glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t);
float inverseLerp(float a, float b, float x);

glm::vec3 getCurrentPosition(Animator* animator);
glm::vec3 getCurrentRotation(Animator* animator);
glm::vec3 getCurrentScale(Animator* animator);

glm::vec3 getCurrentVec3FromKeyframes(Animator* animator, std::vector<Vec3Keyframe*> keyframes);

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;

float minBias = 0.005f;

float maxBias = 0.05f;

bool doEmboss = false;
bool doGaussianBlur = false;

// animation related variables

AnimationClip* animationClip = new AnimationClip(1.0f);
Animator* animator = new Animator(animationClip);

float rectangleVertices[] = 
{  // coords    //texCoords
	 1.0, -1.0,   1.0, 0.0,
	-1.0, -1.0,   0.0, 0.0,
	-1.0,  1.0,   0.0, 1.0,
	
	 1.0,  1.0,   1.0, 1.0,
	 1.0, -1.0,   1.0, 0.0,
	-1.0,  1.0,   0.0, 1.0
};

float floorVertices[] = {
	// positions            // normals         // texcoords
	 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
	-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
	-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

	 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
	-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
	 25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
};

struct Material {
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

ew::Camera camera;
ew::Transform monkeyTransform;
ew::CameraController cameraController;

unsigned int planeVAO;
unsigned int depthMap;
float lightPosition[3] = {-2.0,4.0,-1.0};
float nearPlane = 1.0f;
float farPlane = 7.5f;

int main() {
	GLFWwindow* window = initWindow("Assignment 1", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	GLuint brickTexture = ew::loadTexture("assets/brick_color.jpg");

	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f); // point camera at the center of the scene
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f; // vertical field of view in degrees


    float planeVertices[] = {
        // positions            // normals         // texcoords
         5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  5.0f,  0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,   0.0f, 5.0f,

         5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  5.0f,  0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,   0.0f, 5.0f,
         5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,  5.0f, 5.0f
    };
    // plane VAO
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

	unsigned int rectVAO; // vertex array object
	unsigned int rectVBO; // vertex buffer object
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float), (void*)(2*sizeof(float)));


	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Shader emboss = ew::Shader("assets/emboss.vert","assets/emboss.frag");
	//ew::Shader blur = ew::Shader("assets/blur.vert","assets/blur.frag");
	ew::Shader identity = ew::Shader("assets/identity.vert","assets/identity.frag");
	ew::Shader simpleDepth = ew::Shader("assets/simpleDepthShader.vert","assets/simpleDepthShader.frag");
	ew::Shader shadowMap = ew::Shader("assets/shadowMapping.vert","assets/shadowMapping.frag");
	ew::Model monkeyModel = ew::Model("assets/suzanne.obj");

	glEnable(GL_DEPTH_TEST); // depth testing

	unsigned int RBO; // render buffer object
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	unsigned int depthMapFBO;  // create frame buffer for shadows
	glGenFramebuffers(1,&depthMapFBO);

	const unsigned int SHADOW_WIDTH = 1024; // define width and height of shadow map
	const unsigned int SHADOW_HEIGHT = 1024;

	// depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER,0);


	shadowMap.use();
	shadowMap.setInt("diffuseTexture", 0);
	shadowMap.setInt("shadowMap", 1);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;
		cameraController.move(window, &camera, deltaTime);
		
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 lightPos(lightPosition[0], lightPosition[1], lightPosition[2]);
		glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, nearPlane, farPlane);
		glm::mat4 lightView = glm::lookAt(lightPos,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		simpleDepth.use();
		simpleDepth.setMat4("lightSpaceMatrix",lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    	glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, brickTexture);

		glm::mat4 model = monkeyTransform.modelMatrix();

		if (animator->isPlaying) {
			animator->playbackTime += deltaTime * animator->playbackSpeed;
			if (animator->isLooping && animator->playbackTime > animator->clip->duration) {
				animator->playbackTime = 0;
			}
			if (animator->clip->positionFrames.size() > 0){
				model = glm::translate(model, getCurrentPosition(animator));
			}
			if (animator->clip->rotationFrames.size() > 0){
				model = glm::rotate(model, getCurrentRotation(animator)[0], glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, getCurrentRotation(animator)[1], glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, getCurrentRotation(animator)[2], glm::vec3(0.0f, 0.0f, 1.0f));
			}
			if (animator->clip->scaleFrames.size() > 0){
				//model = glm::scale(model, getCurrentScale(animator));
				glm::vec3 scale = getCurrentScale(animator);
				//model = glm::scale(model,glm::vec3(2.0f,1.0f,1.0f));
				//glm::vec4 scalar = glm::vec4(getCurrentScale(animator),1.0f);
				model = glm::scale(model, scale);
			}
		}

		simpleDepth.setMat4("model", model);
		monkeyModel.draw();

		//renderScene(simpleDepth, model, monkeyModel, time);

		glBindFramebuffer(GL_FRAMEBUFFER,0);

		// reset viewport
		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render normal scene
		shadowMap.use();
		glm::mat4 projection = camera.projectionMatrix();
		glm::mat4 view = camera.viewMatrix();
		shadowMap.setMat4("projection",projection);
		shadowMap.setMat4("view",view);
		shadowMap.setVec3("viewPos",camera.position);
		shadowMap.setVec3("lightPos",lightPos);
		shadowMap.setMat4("lightSpaceMatrix",lightSpaceMatrix);
		shadowMap.setFloat("maxBias",maxBias);
		shadowMap.setFloat("minBias",minBias);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,brickTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,depthMap);



		shadowMap.setMat4("model", model);
		monkeyModel.draw();

		//renderScene(shadowMap, model, monkeyModel, time);

		drawUI();

		glfwSwapBuffers(window);
	}
	delete animator;
	printf("Shutting down...");
}

void resetCamera(ew::Camera* camera, ew::CameraController* controller) {
	camera->position = glm::vec3(0, 0, 5.0f);
	camera->target = glm::vec3(0);
	controller->yaw = 0;
	controller->pitch = 0;
}

void drawUI() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");
	if (ImGui::Button("Reset Camera")) {
		resetCamera(&camera, &cameraController);
	}
	if (ImGui::CollapsingHeader("Material")) {
		ImGui::SliderFloat("AmbientK", &material.Ka, 0.0f, 1.0f);
		ImGui::SliderFloat("DiffuseK", &material.Kd, 0.0f, 1.0f);
		ImGui::SliderFloat("SpecularK", &material.Ks, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &material.Shininess, 2.0f, 1024.0f);
	}
	if (ImGui::CollapsingHeader("Effects")) {
		ImGui::Checkbox("Emboss",&doEmboss);
		ImGui::Checkbox("Gaussian Blur",&doGaussianBlur);
	}
	if (ImGui::CollapsingHeader("Light Source")) {
		// light position
		ImGui::SliderFloat3("Light Position", lightPosition,-8.0,8.0);
		// near plane
		ImGui::SliderFloat("Near Plane",&nearPlane,0.1,10.0);
		// far plane
		ImGui::SliderFloat("Far Plane", &farPlane, 0.1, 10.0);
		// min bias
		ImGui::SliderFloat("Min Bias",&minBias, 0.0f,1.0f);
		// max bias 
		ImGui::SliderFloat("Max Bias", &maxBias, 0.0f, 1.0f);
	}

	if (ImGui::CollapsingHeader("Animation")) {
		// toggle whether the animation is playing
		ImGui::Checkbox("Playing",&animator->isPlaying);
		// toggle whether the animation will loop
		ImGui::Checkbox("Loop",&animator->isLooping);
		// set animation playback speed
		ImGui::SliderFloat("Playback Speed", &animator->playbackSpeed, 0.0f, 2.0f);
		// animation playback time
		ImGui::SliderFloat("Playback Time", &animator->playbackTime, 0.0f, animator->clip->duration);
		// animation duration
		ImGui::SliderFloat("Duration", &animator->clip->duration, 0.01f, 10.0f);
	}

	// --manage position keyframes--
	if (ImGui::CollapsingHeader("Position Keyframes")) {
		for (int i = 0; i < animator->clip->positionFrames.size(); i++) {
			ImGui::PushID(i);
			ImGui::SliderFloat("Time", &animator->clip->positionFrames[i]->time, 0.0f, animator->clip->duration);
			ImGui::SliderFloat3("Values", animator->clip->positionFrames[i]->value, 0.0f, 10.0f);
			ImGui::PopID();
		}
		if (ImGui::Button("Add Keyframe")) {
			animator->clip->positionFrames.push_back(new Vec3Keyframe);
		}
		if (ImGui::Button("Remove Keyframe")) {
			if (animator->clip->positionFrames.size() > 0){
				animator->clip->positionFrames.pop_back();
			}
		}
	}
	//--manage rotation keyframes--
	if (ImGui::CollapsingHeader("Rotation Keyframes")) {
		for (int i = 0; i < animator->clip->rotationFrames.size(); i++) {
			ImGui::PushID(i);
			ImGui::SliderFloat("Time", &animator->clip->rotationFrames[i]->time, 0.0f, animator->clip->duration);
			ImGui::SliderFloat3("Values", animator->clip->rotationFrames[i]->value, 0.0f, 10.0f);
			ImGui::PopID();
		}
		if (ImGui::Button("Add Keyframe")) {
			animator->clip->rotationFrames.push_back(new Vec3Keyframe);
		}
		if (ImGui::Button("Remove Keyframe")) {
			if (animator->clip->rotationFrames.size() > 0) {
				animator->clip->rotationFrames.pop_back();
			}
		}
	}
	//--manage scale keyframes--
	if (ImGui::CollapsingHeader("Scale Keyframes")) {
		for (int i = 0; i < animator->clip->scaleFrames.size(); i++) {
			ImGui::PushID(i);
			ImGui::SliderFloat("Time", &animator->clip->scaleFrames[i]->time, 0.0f, animator->clip->duration);
			ImGui::SliderFloat3("Values", animator->clip->scaleFrames[i]->value, 0.0f, 10.0f);
			ImGui::PopID();
		}
		if (ImGui::Button("Add Keyframe")) {
			animator->clip->scaleFrames.push_back(new Vec3Keyframe(1.0f,1.0f,1.0f));
		}
		if (ImGui::Button("Remove Keyframe")) {
			if (animator->clip->scaleFrames.size() > 0) {
				animator->clip->scaleFrames.pop_back();
			}
		}
	}

	ImGui::End();
	ImGui::Begin("Shadow Map");
	//Using a Child allow to fill all the space of the window.
	ImGui::BeginChild("Shadow Map");
	//Stretch image to be window size
	ImVec2 windowSize = ImGui::GetWindowSize();
	//Invert 0-1 V to flip vertically for ImGui display
	//shadowMap is the texture2D handle
	ImGui::Image((ImTextureID)depthMap, windowSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}

/// <summary>
/// Initializes GLFW, GLAD, and IMGUI
/// </summary>
/// <param name="title">Window title</param>
/// <param name="width">Window width</param>
/// <param name="height">Window height</param>
/// <returns>Returns window handle on success or null on fail</returns>
GLFWwindow* initWindow(const char* title, int width, int height) {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return nullptr;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return nullptr;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}

void renderScene(const ew::Shader& shader, glm::mat4 monkeyMatrix, ew::Model monkeyModel, float time) {
	glm::mat4 model = glm::mat4(1.0f);
	shader.setMat4("model", model);
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES,0,6);
	
	shader.setMat4("model", monkeyMatrix);
	
	monkeyModel.draw();
}

// I copied this functin from learnopengl.com to make testing easier
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			 // bottom face
			 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 // top face
			 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}


float lerp(float a, float b, float t) {
	return (1.0-t)*a + t*b;
}

glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t) {
	glm::vec3 result = glm::vec3(0);
	result.x = (1.0-t)*a.x + t*b.x;
	result.y = (1.0-t)*a.y + t*b.y;
	result.z = (1.0-t)*a.z + t*b.z;
	return result;
}

// returns how far between a and b x is.
float inverseLerp(float a, float b, float x) {
	if (b == a) {
		return 0;
	}
	return (x-a)/(b-a);
}

int getPreviousKeyframe(std::vector<Vec3Keyframe*> vector, float time) {
	for (int i = 0; i < vector.size(); i++) {
		if (vector[i]->time > time) {
			return i-1;
		}
	}
	return vector.size() - 1;
}

// current model position is determined by getting previous and next position keyframes from time
// --if next position is index 0, count previous and next both as 0
// --if previous is the final keyframe, count previous and next both as the final keyframe
// and then doing an inverse lerp of previous and next keyframe times with current time to get value T
// then do a lerp of previous position and next position with value T to get current position
glm::vec3 getCurrentPosition(Animator* animator) {
	std::vector<Vec3Keyframe*> keyFrames = animator->clip->positionFrames;
	return getCurrentVec3FromKeyframes(animator, keyFrames);
}

glm::vec3 getCurrentRotation(Animator* animator) {
	std::vector<Vec3Keyframe*> keyFrames = animator->clip->rotationFrames;
	return getCurrentVec3FromKeyframes(animator, keyFrames);
}

glm::vec3 getCurrentScale(Animator* animator) {
	std::vector<Vec3Keyframe*> keyFrames = animator->clip->scaleFrames;
	return getCurrentVec3FromKeyframes(animator, keyFrames);
}

glm::vec3 getCurrentVec3FromKeyframes(Animator* animator, std::vector<Vec3Keyframe*> keyFrames) {
	int previousKeyframeIndex = getPreviousKeyframe(keyFrames, animator->playbackTime);
	int nextKeyframeIndex;
	// if the previous index is -1, treat previous and next as 0
	if (previousKeyframeIndex == -1) {
		previousKeyframeIndex = 0;
		nextKeyframeIndex = 0;
	}
	// if previous index is the end of the vector, treat previous and next as the same
	else if (previousKeyframeIndex == keyFrames.size() - 1) {
		nextKeyframeIndex = previousKeyframeIndex;
	}
	// otherwise next index is just the next sequential index
	else {
		nextKeyframeIndex = previousKeyframeIndex + 1;
	}

	float T = inverseLerp(keyFrames[previousKeyframeIndex]->time, keyFrames[nextKeyframeIndex]->time, animator->playbackTime);

	glm::vec3 firstValue = glm::vec3(keyFrames[previousKeyframeIndex]->value[0], keyFrames[previousKeyframeIndex]->value[1], keyFrames[previousKeyframeIndex]->value[2]);
	glm::vec3 secondValue = glm::vec3(keyFrames[nextKeyframeIndex]->value[0], keyFrames[nextKeyframeIndex]->value[1], keyFrames[nextKeyframeIndex]->value[2]);

	glm::vec3 currentValue = lerp(firstValue, secondValue, T);

	return currentValue;
}