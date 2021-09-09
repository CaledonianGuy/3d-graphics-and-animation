// 3D Graphics and Animation - Main Template
// Visual Studio 2019
// Last Changed 01/10/2019 by Professor
// Last Chnaged 09/12/2020 by Student

#pragma comment(linker, "/NODEFAULTLIB:MSVCRT")
#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include <GL/glew.h>			// Add library to extend OpenGL to newest version
#include <GLFW/glfw3.h>			// Add library to launch a window
#include <GLM/glm.hpp>			// Add helper maths library
#include <GLM/gtx/transform.hpp>

#include <stb_image.h>			// Add library to load images for textures

#include "Mesh.h"				// Simplest mesh holder and OBJ loader - can update more - from https://github.com/BennyQBD/ModernOpenGLTutorial
#include "Shader.h"				// Allows multiple shader useage


// MAIN FUNCTIONS
void setupRender();
void startup();
void update(GLfloat currentTime);
void render(GLfloat currentTime);
void endProgram();
//void loadTexture(string name);

// HELPER FUNCTIONS OPENGL
void hintsGLFW();
void errorCallbackGLFW(int error, const char* description);
void debugGL();
static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam);

// CALLBACK FUNCTIONS FOR WINDOW
void onResizeCallback(GLFWwindow* window, int w, int h);
void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow* window, double x, double y);
void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);


// VARIABLES
GLFWwindow*		window;											// Keep track of the window
int				windowWidth = 640;				
int				windowHeight = 480;
bool			running = true;									// Are we still running?
glm::mat4		proj_matrix;									// Projection Matrix
glm::vec3		cameraPosition = glm::vec3(00.0f, 0.0f, 5.0f);	// Week 5 lecture
glm::vec3		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float           aspect = (float)windowWidth / (float)windowHeight;
float			fovy = 45.0f;
bool			keyStatus[1024];
GLfloat			deltaTime = 0.0f;
GLfloat			lastTime = 0.0f;
GLint			proj_location;

// Initialise Mesh, Shader, and Attributes (Position) for each object
// Sphere object
// --------------------------------
glm::vec3		modelSpherePosition1;
glm::vec3		modelSpherePosition2;
glm::vec3		modelSpherePosition3;
glm::vec3		modelSpherePosition4;
glm::vec3		modelSpherePosition1Initial;
Mesh			sphere;
Shader			sphereShader;
// Holder object
// --------------------------------
glm::vec3		modelHolderPosition1;
glm::vec3		modelHolderPosition2;
glm::vec3		modelHolderPosition3;
glm::vec3		modelHolderPosition4;
glm::vec3		modelHolderPositionInitial;
Mesh			holder;
Shader			holderShader;
// Pin object
// --------------------------------
glm::vec3		modelPinPosition1;
glm::vec3		modelPinPosition2;
glm::vec3		modelPinPosition3;
glm::vec3		modelPinPosition4;
glm::vec3		modelPinPositionInitial;
Mesh			pin;
Shader			pinShader;
// Light object
// --------------------------------
glm::vec3		lightPosition;
glm::vec3		lightRotation;
glm::vec3		lightScale;
Mesh			light;
Shader			lightShader;
// --------------------------------

// FPS camera variables
GLfloat			yaw = -90.0f;									// init pointing to inside
GLfloat			pitch = 0.0f;									// start centered
GLfloat			lastX = windowWidth / 2.0f;						// start middle screen
GLfloat			lastY = windowHeight / 2.0f;					// start middle screen
bool			firstMouse = true;

// Lighting variables
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);				// Sets the colour of the light
glm::vec3 materialColor = glm::vec3(0.1f, 0.1f, 0.1f);			// Sets the colour of the black material (used on Holders and Pins)

GLfloat			ka = 0.2f;										// Ambient constant
GLfloat			kd = 0.75f;										// Diffuse constant
GLfloat			ks = 0.6f;										// Specular constant
GLfloat			shine = 32.0f;									// Shine constant

unsigned int amount = 4;
glm::mat4* modelMatricesSphere;

int main()
{
	if (!glfwInit()) {							// Checking for GLFW
		cout << "Could not initialise GLFW...";
		return 0;
	}

	glfwSetErrorCallback(errorCallbackGLFW);	// Setup a function to catch and display all GLFW errors.

	hintsGLFW();								// Setup glfw with various hints.		

	// Start a window using GLFW
	string title = "My OpenGL Application";

	// Fullscreen
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	windowWidth = mode->width; windowHeight = mode->height; //fullscreen
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), glfwGetPrimaryMonitor(), NULL); // fullscreen

	// Window
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);
	if (!window) {								// Window or OpenGL context creation failed
		cout << "Could not initialise GLFW...";
		endProgram();
		return 0;
	}

	glfwMakeContextCurrent(window);				// making the OpenGL context current

	// Start GLEW (note: always initialise GLEW after creating your window context.)
	glewExperimental = GL_TRUE;					// hack: catching them all - forcing newest debug callback (glDebugMessageCallback)
	GLenum errGLEW = glewInit();
	if (GLEW_OK != errGLEW) {					// Problems starting GLEW?
		cout << "Could not initialise GLEW...";
		endProgram();
		return 0;
	}

	debugGL();									// Setup callback to catch openGL errors.	

	// Setup all the message loop callbacks.
	glfwSetWindowSizeCallback(window, onResizeCallback);			// Set callback for resize
	glfwSetKeyCallback(window, onKeyCallback);						// Set Callback for keys
	glfwSetMouseButtonCallback(window, onMouseButtonCallback);		// Set callback for mouse click
	glfwSetCursorPosCallback(window, onMouseMoveCallback);			// Set callback for mouse move
	glfwSetScrollCallback(window, onMouseWheelCallback);			// Set callback for mouse wheel.
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);		// Set mouse cursor. Fullscreen
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Set mouse cursor FPS fullscreen.

	setupRender();								// setup some render variables.
	startup();									// Setup all necessary information for startup (aka. load texture, shaders, models, etc).

	do {										// run until the window is closed
		GLfloat currentTime = (GLfloat)glfwGetTime();		// retrieve timelapse
		deltaTime = currentTime - lastTime;		// Calculate delta time
		lastTime = currentTime;					// Save for next frame calculations.
		glfwPollEvents();						// poll callbacks
		update(currentTime);					// update (physics, animation, structures, etc)
		render(currentTime);					// call render function.

		glfwSwapBuffers(window);				// swap buffers (avoid flickering and tearing)

		running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);	// exit if escape key pressed
		running &= (glfwWindowShouldClose(window) != GL_TRUE);
	} while (running);

	endProgram();			// Close and clean everything up...

	cout << "\nPress any key to continue...\n";
	cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}

void errorCallbackGLFW(int error, const char* description) {
	cout << "Error GLFW: " << description << "\n";
}

void hintsGLFW() {
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);			// Create context in debug mode - for debug message callback
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

void endProgram() {
	glfwMakeContextCurrent(window);		// destroys window handler
	glfwTerminate();	// destroys all windows and releases resources.
}

void setupRender() {
	glfwSwapInterval(1);	// Ony render when synced (V SYNC)

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 2);
	glfwWindowHint(GLFW_STEREO, GL_FALSE);
}

void startup() {

	// Load objects' model, shaders, and any textures
	// Sphere
	// -----------------------------
	sphere.LoadModel("Model_Sphere.obj");
	sphereShader.shaderLink("vs_sphere.glsl", "fs_sphere.glsl");
	// Texture
	sphereShader.loadTexture("Ball_Texture.bmp");
	// -----------------------------
	holder.LoadModel("Model_Holder.obj");
	holderShader.shaderLink("vs_holder.glsl", "fs_holder.glsl");
	// Pin
	// -----------------------------
	pin.LoadModel("Model_Pin.obj");
	pinShader.shaderLink("vs_pin.glsl", "fs_pin.glsl");
	// Light
	// -----------------------------
	light.LoadModel("light_sphere.obj");
	lightShader.shaderLink("vs_light.glsl", "fs_light.glsl");
	// -----------------------------

	// Set start locations for objects
	// Sphere
	// -----------------------------
	// Position
	modelSpherePosition1 = glm::vec3(0.0f, 0.0f, 0.0f);
	modelSpherePosition2 = glm::vec3(2.05f, -2.899f, 0.0f);
	modelSpherePosition3 = glm::vec3(-1.025f, -2.899f, 1.775f);
	modelSpherePosition4 = glm::vec3(-1.025f, -2.899f, -1.775f);
	modelSpherePosition1Initial = glm::vec3(0.0f);
	// Holder
	// -----------------------------
	modelHolderPosition1 = glm::vec3(0.0f);
	modelHolderPosition2 = glm::vec3(0.0f);
	modelHolderPosition3 = glm::vec3(0.0f);
	modelHolderPosition4 = glm::vec3(0.0f);
	modelHolderPositionInitial = glm::vec3(0.0f);
	// Pin
	// -----------------------------
	modelPinPosition1 = glm::vec3(0.0f);
	modelPinPosition2 = glm::vec3(0.0f);
	modelPinPosition3 = glm::vec3(0.0f);
	modelPinPosition4 = glm::vec3(0.0f);
	modelPinPositionInitial = glm::vec3(0.0f);
	// Light
	// -----------------------------
	lightPosition = glm::vec3(5.0f, 5.0f, 5.0f);
	lightRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	lightScale = glm::vec3(0.2f);

	// A few optimizations.
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Calculate proj_matrix for the first time.
	aspect = (float)windowWidth / (float)windowHeight;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void update(GLfloat currentTime) {

	// Calculate camera movement
	GLfloat cameraSpeed = 3.0f * deltaTime;
	if (keyStatus[GLFW_KEY_W]) cameraPosition += cameraSpeed * cameraFront;
	if (keyStatus[GLFW_KEY_S]) cameraPosition -= cameraSpeed * cameraFront;
	if (keyStatus[GLFW_KEY_A]) cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keyStatus[GLFW_KEY_D]) cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	// Calculate objects movements
	// Speeds
	// -----------------------------
	GLfloat sphereSpeed = 0.5f * deltaTime;
	GLfloat holderSpeed = 1.5f * deltaTime;
	GLfloat pinSpeed = 2.5f * deltaTime;
	// Outward movement
	// -----------------------------
	if (keyStatus[GLFW_KEY_KP_ADD])
	{
		// Sphere
		// -----------------------------
		modelSpherePosition1 += normalize(glm::vec3(0.0f, 1.0f, 0.0f)) * sphereSpeed;
		// Holder
		// -----------------------------
		modelHolderPosition2 += normalize(glm::vec3(-1.0f, 0.0f, 0.0f)) * holderSpeed;
		modelHolderPosition3 += normalize(glm::vec3(0.5f, 0.0f, -0.866f)) * holderSpeed;
		modelHolderPosition4 += normalize(glm::vec3(0.5f, 0.0f, 0.866f)) * holderSpeed;
		// Pin
		// -----------------------------
		modelPinPosition1 += normalize(glm::vec3(0.0f, 1.0f, 0.0f)) * pinSpeed;
		modelPinPosition2 += normalize(glm::vec3(2.05f, -0.755f, 0.0f)) * pinSpeed;
		modelPinPosition3 += normalize(glm::vec3(-1.025f, -0.755f, 1.775f)) * pinSpeed;
		modelPinPosition4 += normalize(glm::vec3(-1.025f, -0.755f, -1.775f)) * pinSpeed;
	}
	// Inward movement
	// -----------------------------
	if (keyStatus[GLFW_KEY_KP_SUBTRACT])
	{
		// Epsilon is used to check the distance between two objects 
		GLfloat EPSILON = 0.01f;
		// Sphere
		// -----------------------------
		if ((glm::length(modelSpherePosition1 - modelSpherePosition1Initial) > EPSILON)) modelSpherePosition1 -= normalize(glm::vec3(0.0f, 1.0f, 0.0f)) * sphereSpeed;
		// Holder
		// -----------------------------
		if ((glm::length(modelHolderPosition2 - modelHolderPositionInitial) > EPSILON)) modelHolderPosition2 -= normalize(glm::vec3(-1.0f, 0.0f, 0.0f)) * holderSpeed;
		if ((glm::length(modelHolderPosition3 - modelHolderPositionInitial) > EPSILON)) modelHolderPosition3 -= normalize(glm::vec3(0.5f, 0.0f, -0.866f)) * holderSpeed;
		if ((glm::length(modelHolderPosition4 - modelHolderPositionInitial) > EPSILON)) modelHolderPosition4 -= normalize(glm::vec3(0.5f, 0.0f, 0.866f)) * holderSpeed;
		// Pin
		// -----------------------------
		if ((glm::length(modelPinPosition1 - modelPinPositionInitial) > EPSILON)) modelPinPosition1 -= normalize(glm::vec3(0.0f, 1.0f, 0.0f)) * pinSpeed;
		if ((glm::length(modelPinPosition2 - modelPinPositionInitial) > EPSILON)) modelPinPosition2 -= normalize(glm::vec3(2.05f, -0.755f, 0.0f)) * pinSpeed;
		if ((glm::length(modelPinPosition3 - modelPinPositionInitial) > EPSILON)) modelPinPosition3 -= normalize(glm::vec3(-1.025f, -0.755f, 1.775f)) * pinSpeed;
		if ((glm::length(modelPinPosition4 - modelPinPositionInitial) > EPSILON)) modelPinPosition4 -= normalize(glm::vec3(-1.025f, -0.755f, -1.775f)) * pinSpeed;
	}
	// Reset all movements
	// -----------------------------
	if (keyStatus[GLFW_KEY_R])
	{
		// Sphere
		// -----------------------------
		// Position
		modelSpherePosition1 = modelSpherePosition1Initial;
		// Holder
		// -----------------------------
		modelHolderPosition2 = modelHolderPositionInitial;
		modelHolderPosition3 = modelHolderPositionInitial;
		modelHolderPosition4 = modelHolderPositionInitial;
		// Pin
		// -----------------------------
		modelPinPosition1 = modelPinPositionInitial;
		modelPinPosition2 = modelPinPositionInitial;
		modelPinPosition3 = modelPinPositionInitial;
		modelPinPosition4 = modelPinPositionInitial;
	}
}

void render(GLfloat currentTime) {
	glViewport(0, 0, windowWidth, windowHeight);

	// Clear colour buffer
	glm::vec4 backgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f); glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);

	// Clear deep buffer
	static const GLfloat one = 1.0f; glClearBufferfv(GL_DEPTH, 0, &one);

	// Enable blend
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Window and full screen camera setup
	glm::mat4 viewMatrix = glm::lookAt(
		cameraPosition,					// eye
		cameraPosition + cameraFront,	// centre
		cameraUp);						// up

	// States which program and passes variables into the relevant shaders
	// Spheres
	// -----------------------------
	// Use program, and send shared matices to relevant shader
	sphereShader.use();
	sphereShader.setMat4("proj_matrix", proj_matrix);
	sphereShader.setMat4("view_matrix", viewMatrix);
	// Sphere One, Model Matrix and Draw
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, modelSpherePosition1);
	sphereShader.setMat4("model_matrix", modelMatrix);
	sphere.Draw();
	// Shpere Two, Model Matrix and Draw
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, modelSpherePosition2);
	sphereShader.setMat4("model_matrix", modelMatrix);
	sphere.Draw();
	// Shpere Three, Model Matrix and Draw
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, modelSpherePosition3);
	sphereShader.setMat4("model_matrix", modelMatrix);
	sphere.Draw();
	// Shpere Four, Model Matrix and Draw
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, modelSpherePosition4);
	sphereShader.setMat4("model_matrix", modelMatrix);
	sphere.Draw();
	// Pass values used for calculating light to relevant shader
	sphereShader.setVec3("materialColor", materialColor);
	sphereShader.setVec3("lightColor", lightColor);
	sphereShader.setVec3("lightPos", lightPosition);
	sphereShader.setVec3("viewPos", cameraPosition);
	// Pass constant values for calculating light to relevant shader
	sphereShader.setFloat("ka", ka);
	sphereShader.setFloat("kd", 0.8f);
	sphereShader.setFloat("ks", 0.15f);
	sphereShader.setFloat("shine", 64.0f);
	// -----------------------------

	// Holders
	// -----------------------------
	// Use program, and send shared matices to relevant shader
	holderShader.use();										
	holderShader.setMat4("proj_matrix", proj_matrix);
	holderShader.setMat4("view_matrix", viewMatrix);
	// Holder One, Model Matrix and Draw
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, modelHolderPosition1);
	holderShader.setMat4("model_matrix", modelMatrix);
	holder.Draw();
	// Holder Two, Model Matrix and Draw
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, modelHolderPosition2);
	modelMatrix = glm::rotate(modelMatrix, 4.3738f, glm::vec3(0.0f, 0.0f, 1.0f)); //0.0f, 1.0472f, 4.3738f
	modelMatrix = glm::rotate(modelMatrix, 1.0472f, glm::vec3(0.0f, 1.0f, 0.0f));
	holderShader.setMat4("model_matrix", modelMatrix);
	holder.Draw();
	// Holder Three, Model Matrix and Draw
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, modelHolderPosition3);
	modelMatrix = glm::rotate(modelMatrix, 5.777f, glm::vec3(0.0f, 0.0f, 1.0f)); //2.1206f, 5.9952f, 5.777f
	modelMatrix = glm::rotate(modelMatrix, 5.9952f, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, 2.1206f, glm::vec3(1.0f, 0.0f, 0.0f));
	holderShader.setMat4("model_matrix", modelMatrix);
	holder.Draw();
	// Holder Four, Model Matrix and Draw
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, modelHolderPosition4);
	modelMatrix = glm::rotate(modelMatrix, 3.7525f, glm::vec3(0.0f, 0.0f, 1.0f)); //4.7124f, 5.6723f, 3.7525f
	modelMatrix = glm::rotate(modelMatrix, 5.6723f, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, 4.7124f, glm::vec3(1.0f, 0.0f, 0.0f));
	holderShader.setMat4("model_matrix", modelMatrix);
	holder.Draw();
	// Pass values used for calculating light to relevant shader
	holderShader.setVec3("materialColor", materialColor);
	holderShader.setVec3("lightColor", lightColor);
	holderShader.setVec3("lightPos", lightPosition);
	holderShader.setVec3("viewPos", cameraPosition);
	// Pass constant values for calculating light to relevant shader
	holderShader.setFloat("ka", ka);
	holderShader.setFloat("kd", kd);
	holderShader.setFloat("ks", ks);
	holderShader.setFloat("shine", shine);
	// -----------------------------

	// Pins
	// -----------------------------
	// Use program, and send shared matices to relevant shader
	pinShader.use();
	pinShader.setMat4("proj_matrix", proj_matrix);
	pinShader.setMat4("view_matrix", viewMatrix);
	// Pin One, Model Matrix and draw
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, modelPinPosition1);
	pinShader.setMat4("model_matrix", modelMatrix);
	pin.Draw();
	// Pin Two, Model Matrix and draw
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, modelPinPosition2);
	modelMatrix = glm::rotate(modelMatrix, 4.3738f, glm::vec3(0.0f, 0.0f, 1.0f)); //0.0f, 1.0472f, 4.3738f
	modelMatrix = glm::rotate(modelMatrix, 1.0472f, glm::vec3(0.0f, 1.0f, 0.0f));
	pinShader.setMat4("model_matrix", modelMatrix);
	pin.Draw();
	// Pin Three, Model Matrix and draw
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, modelPinPosition3);
	modelMatrix = glm::rotate(modelMatrix, 5.777f, glm::vec3(0.0f, 0.0f, 1.0f)); //2.1206f, 5.9952f, 5.777f
	modelMatrix = glm::rotate(modelMatrix, 5.9952f, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, 2.1206f, glm::vec3(1.0f, 0.0f, 0.0f));
	pinShader.setMat4("model_matrix", modelMatrix);
	pin.Draw();
	// Pin Four, Model Matrix and draw
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, modelPinPosition4);
	modelMatrix = glm::rotate(modelMatrix, 3.7525f, glm::vec3(0.0f, 0.0f, 1.0f)); //4.7124f, 5.6723f, 3.7525f
	modelMatrix = glm::rotate(modelMatrix, 5.6723f, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, 4.7124f, glm::vec3(1.0f, 0.0f, 0.0f));
	pinShader.setMat4("model_matrix", modelMatrix);
	pin.Draw();
	// Pass values used for calculating light to relevant shader
	pinShader.setVec3("materialColor", materialColor);
	pinShader.setVec3("lightColor", lightColor);
	pinShader.setVec3("lightPos", lightPosition);
	pinShader.setVec3("viewPos", cameraPosition);
	// Pass constant values for calculating light to relevant shader
	pinShader.setFloat("ka", ka);
	pinShader.setFloat("kd", kd);
	pinShader.setFloat("ks", ks);
	pinShader.setFloat("shine", shine);
	// -----------------------------

	// Light
	// -----------------------------
	// Use program, and send shared matices to relevant shader
	lightShader.use();
	lightShader.setMat4("proj_matrix", proj_matrix);
	lightShader.setMat4("view_matrix", viewMatrix);
	// Light, Model Matrix, light colour, and draw
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, lightPosition);
	modelMatrix = glm::scale(modelMatrix, lightScale);
	lightShader.setMat4("model_matrix", modelMatrix);
	lightShader.setVec3("lightColor", lightColor);
	light.Draw();
	// -----------------------------
}

void onResizeCallback(GLFWwindow* window, int w, int h) {
	windowWidth = w;
	windowHeight = h;

	aspect = (float)w / (float)h;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) keyStatus[key] = true;
	else if (action == GLFW_RELEASE) keyStatus[key] = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

}

void onMouseMoveCallback(GLFWwindow* window, double x, double y) {
	int mouseX = static_cast<int>(x);
	int mouseY = static_cast<int>(y);

	if (firstMouse) {
		lastX = (float)mouseX; lastY = (float)mouseY; firstMouse = false;
	}

	GLfloat xoffset = mouseX - lastX;
	GLfloat yoffset = lastY - mouseY; // Reversed
	lastX = (float)mouseX; lastY = (float)mouseY;

	GLfloat sensitivity = 0.05f;
	xoffset *= sensitivity; yoffset *= sensitivity;

	yaw += xoffset; pitch += yoffset;

	// check for pitch out of bounds otherwise screen gets flipped
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	cameraFront = glm::normalize(front);
}

static void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset) {
	int yoffsetInt = static_cast<int>(yoffset);
}

void debugGL() {
	//Output some debugging information
	cout << "VENDOR: " << (char *)glGetString(GL_VENDOR) << endl;
	cout << "VERSION: " << (char *)glGetString(GL_VERSION) << endl;
	cout << "RENDERER: " << (char *)glGetString(GL_RENDERER) << endl;

	// Enable Opengl Debug
	//glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}

static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam) {

	cout << "---------------------opengl-callback------------" << endl;
	cout << "Message: " << message << endl;
	cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		cout << "OTHER";
		break;
	}
	cout << " --- ";

	cout << "id: " << id << " --- ";
	cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		cout << "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		cout << "NOTIFICATION";
	}
	cout << endl;
	cout << "-----------------------------------------" << endl;
}