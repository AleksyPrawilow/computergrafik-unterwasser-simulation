//
// Created by Alexey Pravilov on 02/06/2026.
//

#include "unterwasserszene.h"
#include "glew.h"
#include <GLFW/glfw3.h>
#include "ext.hpp"
#include "../werkzeuge/shaderLoader.h"
#include "../werkzeuge/textur.h"
#include "../werkzeuge/transform.h"
#include "../werkzeuge/renderer.h"
#include "../werkzeuge/daswesen.h"
#include "../dieWesen/fadenkreuz.h"
#include "../dieWesen/uboot.h"
#include "../werkzeuge/kamera.h"

GLuint program;
GLuint programTex;
GLuint programShip;
Core::shaderLoader shaderLoader;

Renderer renderer;
Uboot uboot;
Daswesen earth;
Fadenkreuz crosshair;
Kamera kamera;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	kamera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
	glViewport(0, 0, width, height);
}

void init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glEnable(GL_DEPTH_TEST);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	program = shaderLoader.CreateProgram(
		"assets/shaders/shader_5_1.vert",
		"assets/shaders/shader_5_1.frag"
	);

	programTex = shaderLoader.CreateProgram(
		"assets/shaders/shader_5_1_tex.vert",
		"assets/shaders/shader_5_1_tex.frag"
	);

	programShip = shaderLoader.CreateProgram(
		"assets/shaders/shader_5_1_ship.vert",
		"assets/shaders/shader_5_1_ship.frag"
	);

	earth.loadModel("assets/models/Rock001.obj");

	uboot.transform.position = glm::vec3(-12.f, -1.f, 0.f);
	uboot.transform.scale = glm::vec3(0.5f);
	uboot.material.shader = programShip;
	uboot.init();
	crosshair.init(kamera.getAspectRatio());

	earth.material.shader = programTex;
	earth.material.albedo = Core::LoadTexture("assets/textures/RockTexture001_ao.png");
	earth.material.normal = Core::LoadTexture("assets/textures/RockTexture001_normal.png");
	earth.material.metallic = Core::LoadTexture("assets/textures/RockTexture001_metallic.png");
	earth.transform.position = glm::vec3(0);
	earth.transform.scale = glm::vec3(18.0f);
}

void shutdown(GLFWwindow* window) {
	shaderLoader.DeleteProgram(program);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void renderLoop(GLFWwindow* window) {
	float lastFrame = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		const auto currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if (deltaTime > 0.1f) deltaTime = 0.1f;

		processInput(window);
		uboot.update(window, deltaTime, kamera.transform);

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		const auto time = static_cast<float>(glfwGetTime());
		earth.transform.rotation = glm::angleAxis(time * 0.4f, glm::vec3(0, 1, 0));
		glm::mat4 viewProj = kamera.getProjectionMatrix() * kamera.getViewMatrix();

		renderer.render(earth, viewProj, kamera.transform.position);
		renderer.render(uboot, viewProj, kamera.transform.position);
		crosshair.draw(uboot.transform, viewProj);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}