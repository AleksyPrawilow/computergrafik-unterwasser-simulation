#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"
#include "transform.h"

#include "Kiste.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>


float skyboxVertices[] = {
	// positions
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};


namespace texture {
	GLuint earth;
	GLuint clouds;
	GLuint moon;
	GLuint ship;
	GLuint shipNormal;

	GLuint grid;

	GLuint earthNormal;
	GLuint earthRough;
	GLuint shipRough;
	GLuint shipEmit;
	GLuint shipEmitIntensity;
	GLuint earthMetal;
	GLuint shipMetal;
	GLuint earthAO;
	GLuint shipAO;
	GLuint earthHeight;
	GLuint asteroidNormal;

	
}

GLuint skyboxCubemap;


GLuint program;
GLuint programSun;
GLuint programTex;
GLuint programShip;
GLuint programEarth;
GLuint programProcTex;
Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;

transform spaceshipTransform;
transform cameraTransform;
GLuint VAO, VBO;
GLuint skyboxVAO, skyboxVBO;

float aspectRatio = 1.f;


float roughness = 0.0f;
float roughnessShip = 0.0f;
float metallic = 0.0f;
float metallicShip = 0.0f;
float glowThreshold = 0.3f;



glm::mat4 createCameraMatrix()
{
	glm::mat4 R = glm::toMat4(glm::conjugate(cameraTransform.rotation));
	glm::mat4 T = glm::translate(glm::mat4(1.0f), -cameraTransform.position);
	return R * T;
}

glm::mat4 createPerspectiveMatrix()
{

	glm::mat4 perspectiveMatrix;
	float n = 0.05;
	float f = 80.;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		1,0.,0.,0.,
		0.,aspectRatio,0.,0.,
		0.,0.,(f + n) / (n - f),2 * f * n / (n - f),
		0.,0.,-1.,0.,
		});


	perspectiveMatrix = glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

void drawObjectColor(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color) {
	GLuint prog = program;
	glUseProgram(prog);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(prog, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(prog, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(prog, "color"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(prog, "lightPos"), -5, 3, 3);
	Core::DrawContext(context);
	glUseProgram(0);
}
void drawObjectTexture(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint textureID, GLuint textureID2, float a, float b) {
	GLuint prog = programTex;
	glUseProgram(prog);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(prog, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(prog, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(prog, "lightPos"), -5, 3, 3);
	glUniform3f(glGetUniformLocation(prog, "cameraPos"), cameraTransform.position.x, cameraTransform.position.y, cameraTransform.position.z);


	
	Core::SetActiveTexture(textureID, "colorTexture", prog, 0);
	Core::SetActiveTexture(textureID2, "normalMap", prog, 1);

	
	Core::DrawContext(context);
	glUseProgram(0);
}
void drawObjectShip(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint textureID, GLuint textureID2, float a, float b) {
	GLuint prog = programShip;
	glUseProgram(prog);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(prog, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(prog, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(prog, "lightPos"), -5, 3, 3);
	glUniform3f(glGetUniformLocation(prog, "cameraPos"), cameraTransform.position.x, cameraTransform.position.y, cameraTransform.position.z);
	
	Core::SetActiveTexture(textureID, "colorTexture", prog, 0);
	Core::SetActiveTexture(textureID2, "normalMap", prog, 1);

	Core::DrawContext(context);
	glUseProgram(0);
}


void renderScene(GLFWwindow* window)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 transformation;
	float time = glfwGetTime();


	glm::mat4 model = glm::rotate(2.5f + time * 0.4f, glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(9.0f) * 1.10f);
	drawObjectTexture(sphereContext, model, texture::earth, texture::earthNormal, roughness, metallic);

	glm::mat4 shipModel = spaceshipTransform.getModelMatrix() * glm::eulerAngleY(glm::pi<float>());

	drawObjectShip(
		shipContext,
		shipModel,
		texture::ship,
		texture::shipNormal,
		roughnessShip,
		metallicShip
	);

	glUseProgram(0);
	//glfwSwapBuffers(window);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	aspectRatio = width / float(height);
	glViewport(0, 0, width, height);
}
void loadModelToContext(std::string path, Core::RenderContext& context)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	context.initFromAssimpMesh(scene->mMeshes[0]);
}

void init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("assets/shaders/shader_5_1.vert", "assets/shaders/shader_5_1.frag");
	programTex = shaderLoader.CreateProgram("assets/shaders/shader_5_1_tex.vert", "assets/shaders/shader_5_1_tex.frag");
	programShip = shaderLoader.CreateProgram("assets/shaders/shader_5_1_ship.vert", "assets/shaders/shader_5_1_ship.frag");

	loadModelToContext("assets/models/spaceship.obj", shipContext);

	

	spaceshipTransform.position = glm::vec3(-12.f, -1.f, 0.f);
	spaceshipTransform.scale = glm::vec3(0.5f);
	texture::ship = Core::LoadTexture("assets/textures/rust.jpg");
	texture::shipNormal = Core::LoadTexture("assets/textures/rust_normal.jpg");

}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	float angleSpeed = 0.01f;
	float moveSpeed = 0.01f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		spaceshipTransform.position += spaceshipTransform.forward() * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		spaceshipTransform.position -= spaceshipTransform.forward() * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		spaceshipTransform.position += spaceshipTransform.right() * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		spaceshipTransform.position -= spaceshipTransform.right() * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		spaceshipTransform.position += spaceshipTransform.up() * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		spaceshipTransform.position -= spaceshipTransform.up() * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		spaceshipTransform.yaw(angleSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		spaceshipTransform.yaw(-angleSpeed);

	cameraTransform.position = spaceshipTransform.position - spaceshipTransform.forward() * 1.5f + glm::vec3(0,1,0) * 0.5f;
	cameraTransform.lookAt(spaceshipTransform.position + spaceshipTransform.forward());
}

// funkcja jest glowna petla
void renderLoop(GLFWwindow* window) {


	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// Start ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// UI Panel

		ImGui::SetNextWindowSize(ImVec2(120, 200), ImGuiCond_Always);
		ImGui::Begin("Asteroid Controls");

		ImGui::PushItemWidth(-1);
		// dodaj slajdery tutaj
		ImGui::PopItemWidth();

		ImGui::End();
		
		// Render
		renderScene(window);

		// Render ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

//}