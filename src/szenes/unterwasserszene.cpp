//
// Created by Alexey Pravilov on 02/06/2026.
//

#include "unterwasserszene.h"
#include "glew.h"
#include <GLFW/glfw3.h>
#include "ext.hpp"
#include <vector>
#include "../werkzeuge/textur.h"
#include "../werkzeuge/transform.h"
#include "../werkzeuge/renderer.h"
#include "../werkzeuge/wesen.h"
#include "../dieWesen/fadenkreuz.h"
#include "../dieWesen/uboot.h"
#include "../werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"

GLuint program;
GLuint programTex;
GLuint programShip;

Renderer renderer;
Uboot uboot;
Wesen earth;
Wesen rock;
Fadenkreuz fadenkreuz;
Kamera kamera;
std::vector<Wesen *> diewesen;

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

	diewesen.push_back(&uboot);
	diewesen.push_back(&rock);
	diewesen.push_back(&earth);

	programTex = ShaderManager::getInstance().loadShader(
		"default",
		"assets/shaders/shader_5_1_tex.vert",
		"assets/shaders/shader_5_1_tex.frag"
	);

	for (Wesen * wesen: diewesen) {
		wesen->init();
	}

	earth.loadModel("assets/models/sphere.obj");
	rock.loadModel("assets/models/Rock001.obj");

	uboot.transform.position = glm::vec3(-12.f, -1.f, 0.f);
	uboot.transform.scale = glm::vec3(0.5f);
	fadenkreuz.init(kamera.getAspectRatio());

	rock.material.shader = programTex;
	rock.material.albedo = Kern::LoadTexture("assets/textures/RockTexture001_ao.png");
	rock.material.normal = Kern::LoadTexture("assets/textures/RockTexture001_normal.png");
	rock.material.metallic = Kern::LoadTexture("assets/textures/RockTexture001_metallic.png");
	rock.transform.position = glm::vec3(0);
	rock.transform.scale = glm::vec3(36.0f);

	earth.material.shader = programTex;
	earth.material.albedo = Kern::LoadTexture("assets/textures/earth.png");
	earth.material.normal = Kern::LoadTexture("assets/textures/earth_normal.png");
	earth.transform.position = glm::vec3(40.0f, 50.0f, 30.0f);
	earth.transform.scale = glm::vec3(28.0f);
}

void shutdown(GLFWwindow* window) {
	ShaderManager::getInstance().cleanup();
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

		const auto time = static_cast<float>(glfwGetTime());
		earth.transform.rotation = glm::angleAxis(time * 0.4f, glm::vec3(0, 1, 0));
		rock.transform.rotation = glm::angleAxis(time * 0.6f, glm::vec3(0, 1, 0));
		rock.transform.position.x = cos(time * 0.5f) * 100 + earth.transform.position.x;
		rock.transform.position.y = sin(time * 0.5f) * 100 + earth.transform.position.y;

		for (Wesen * wesen: diewesen) {
			wesen->update(window, deltaTime, kamera.transform);
		}

		glm::mat4 viewProj = kamera.getProjectionMatrix() * kamera.getViewMatrix();

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (const Wesen * wesen: diewesen) {
			renderer.render(*wesen, viewProj, kamera.transform.position);
		}

		fadenkreuz.draw(uboot.transform, viewProj);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}