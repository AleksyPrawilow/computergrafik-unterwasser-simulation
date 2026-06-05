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
#include "dieWesen/earth.h"
#include "dieWesen/wasser.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/skyboxHelper.h"

GLuint programTex;
GLuint cubemapTexture;
GLuint skyboxShader;

std::vector<std::string> skyboxFaces {
	"assets/textures/skybox/px.png",
	"assets/textures/skybox/nx.png",
	"assets/textures/skybox/py.png",
	"assets/textures/skybox/ny.png",
	"assets/textures/skybox/pz.png",
	"assets/textures/skybox/nz.png"
};

Renderer renderer;
auto * uboot = new Uboot();
auto * wasser = new Wasser();
auto * earth = new Earth();
Fadenkreuz fadenkreuz;
Kamera kamera;
std::vector<Wesen *> diewesen;

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
	kamera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
	glViewport(0, 0, width, height);
}

void init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glEnable(GL_DEPTH_TEST);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	wasser->init();

	initSkybox();
	cubemapTexture = Kern::LoadCubemap(skyboxFaces);
	skyboxShader = ShaderManager::getInstance().loadShader("skybox", "assets/shaders/skybox.vert", "assets/shaders/skybox.frag");

	diewesen.push_back(uboot);
	diewesen.push_back(earth);

	for (Wesen * wesen: diewesen) {
		wesen->init();
	}

	uboot->transform.position = glm::vec3(-12.f, -1.f, 0.f);
	fadenkreuz.init(kamera.getAspectRatio());
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

		for (Wesen * wesen: diewesen) {
			wesen->update(window, deltaTime, kamera.transform);
		}

		glm::mat4 viewProj = kamera.getProjectionMatrix() * kamera.getViewMatrix();

		if (constexpr float WATER_HEIGHT = 0.0f; kamera.transform.position.y < WATER_HEIGHT) {
			glClearColor(0.0f, 0.05f, 0.15f, 1.0f);
		} else {
			glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (const Wesen * wesen: diewesen) {
			renderer.render(*wesen, viewProj, kamera.transform.position);
		}

		fadenkreuz.draw(uboot->transform, viewProj);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		renderer.render(*wasser, viewProj, kamera.transform.position);
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		RenderSkybox(skyboxShader, cubemapTexture, skyboxVAO, kamera);

		diewesen.erase(std::remove_if(diewesen.begin(), diewesen.end(), [](const Wesen* wesen) {
			if (wesen->isQueuedDestroyed) {
					delete wesen;
					return true;
				}
				return false;
			}), diewesen.end()
		);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}