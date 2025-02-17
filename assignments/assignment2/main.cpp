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

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;


bool doEmboss = false;
bool doGaussianBlur = false;

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
unsigned int depthMap;
float lightPosition[3] = {2.0,0,0};
float nearPlane = 0.1f;
float farPlane = 7.5f;

int main() {
	GLFWwindow* window = initWindow("Assignment 1", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	GLuint brickTexture = ew::loadTexture("assets/brick_color.jpg");

	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f); // point camera at the center of the scene
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f; // vertical field of view in degrees


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
	ew::Model monkeyModel = ew::Model("assets/suzanne.obj");
	ew::Shader simpleDepth = ew::Shader("assets/simpleDepthShader.vert","assets/simpleDepthShader.frag");
	ew::Shader shadowMap = ew::Shader("assets/shadowMapping.vert","assets/shadowMapping.frag");


	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK); // Back face culling
	glEnable(GL_DEPTH_TEST); // depth testing
	/*
	unsigned int FBO; // frame buffer object
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER,FBO);
	
	unsigned int framebufferTexture; // texture attachments
	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture,0);
	*/
	unsigned int RBO; // render buffer object
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	unsigned int depthMapFBO;  // create frame buffer for shadows
	glGenFramebuffers(1,&depthMapFBO);

	const unsigned int SHADOW_WIDTH = 1024; // define width and height of shadow map
	const unsigned int SHADOW_HEIGHT = 1024;

	// depth map
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec3 lightPos(lightPosition[0], lightPosition[1], lightPosition[2]);
		glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, nearPlane, farPlane);
		glm::mat4 lightView = glm::lookAt(lightPos,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		simpleDepth.use();
		simpleDepth.setMat4("lightSpaceMatrix",lightSpaceMatrix);
		simpleDepth.setMat4("model",monkeyTransform.modelMatrix());

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    	glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, brickTexture);


		monkeyModel.draw();

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
		shadowMap.setMat4("model", monkeyTransform.modelMatrix());
		shadowMap.setVec3("viewPos",camera.position);
		shadowMap.setVec3("lightPos",lightPos);
		shadowMap.setMat4("lightSpaceMatrix",lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,brickTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,depthMap);


		monkeyModel.draw();

		drawUI();

		glfwSwapBuffers(window);
	}
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
		ImGui::SliderFloat3("Light Position", lightPosition,-4.0,4.0);
		// near plane
		ImGui::SliderFloat("Near Plane",&nearPlane,0.1,10.0);
		// far plane
		ImGui::SliderFloat("Far Plane", &farPlane, 0.1, 10.0);
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