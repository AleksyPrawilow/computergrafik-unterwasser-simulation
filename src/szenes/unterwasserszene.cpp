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
#include "../dieWesen/ui/fadenkreuz.h"
#include "../dieWesen/uboot.h"
#include "../werkzeuge/kamera.h"
#include "dieWesen/earth.h"
#include "dieWesen/wasser.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/skyboxHelper.h"
#include "../dieWesen/jellyfish.h"
#include "dieWesen/oceanFloor.h"
#include "../werkzeuge/visual/lightManager.h"
#include "../werkzeuge/visual/tween.h"
#include "dieWesen/ui/hudPanel.h"

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
auto * oceanFloor = new OceanFloor();
auto * hudPanel = new HudPanel();
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

	renderer.init();

	initSkybox();
	cubemapTexture = Kern::LoadCubemap(skyboxFaces);
	skyboxShader = ShaderManager::getInstance().loadShader("skybox", "assets/shaders/skybox.vert", "assets/shaders/skybox.frag");

	diewesen.push_back(uboot);
	diewesen.push_back(earth);
	diewesen.push_back(oceanFloor);
	diewesen.push_back(wasser);
	diewesen.push_back(hudPanel);
	diewesen.push_back(new Jellyfish());

	for (Wesen * wesen: diewesen) {
		wesen->init();
	}

	uboot->transform.position = glm::vec3(-12.f, -1.f, 0.f);
	fadenkreuz.init(kamera.getAspectRatio());
}

void shutdown(GLFWwindow* window) {
	ShaderManager::getInstance().cleanup();
	LightManager::getInstance().cleanup();
	TweenManager::getInstance().cleanup();
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

		if (hudPanel != nullptr) {
			float depthVal = -uboot->transform.position.y;
			float speedVal = 3 * 3.6f;

			if (depthVal < 0.0f) depthVal = 0.0f;

			hudPanel->setDepth(depthVal);
			hudPanel->setSpeed(speedVal);
		}

	    glm::mat4 view = kamera.getViewMatrix();
	    glm::mat4 projection = kamera.getProjectionMatrix();
	    glm::mat4 viewProj = projection * view;

	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	    for (const Wesen * wesen: diewesen) {
	        renderer.render(*wesen, view, projection, kamera.transform.position);
	    }

	    renderer.drawOpaque(view, projection, kamera.transform.position);
	    RenderSkybox(skyboxShader, cubemapTexture, skyboxVAO, kamera, currentFrame);
	    renderer.drawTransparent(view, projection, kamera.transform.position);
		fadenkreuz.draw(uboot->transform, viewProj);
		renderer.drawUI(view, projection);
	    renderer.clearQueues();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
