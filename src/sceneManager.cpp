//
// Created by Alexey Pravilov on 02/06/2026.
//

#include "sceneManager.h"
#include "glew.h"
#include <GLFW/glfw3.h>
#include "ext.hpp"
#include <vector>

#include "szenes/unterwasserszeneWesen.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/transform.h"
#include "werkzeuge/renderer.h"
#include "werkzeuge/wesen.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/skyboxHelper.h"
#include "werkzeuge/visual/lightManager.h"
#include "werkzeuge/visual/tween.h"
#include "werkzeuge/input.h"
#include "werkzeuge/textureManager.h"
#include "werkzeuge/ui/worldspaceUI.h"

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
Kamera kamera;
auto * scene = new UnterwasserszeneWesen();

void Scene::framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
	kamera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
	glViewport(0, 0, width, height);
}

void Scene::init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glEnable(GL_DEPTH_TEST);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	renderer.init();
	UIElement::initUISystem();
	Input::init(window);

	initSkybox();
	cubemapTexture = Kern::LoadCubemap(skyboxFaces);
	skyboxShader = ShaderManager::getInstance().loadShader("skybox", "assets/shaders/skybox.vert", "assets/shaders/skybox.frag");

	scene->init();
}

void Scene::shutdown(GLFWwindow* window) {
	ShaderManager::getInstance().cleanup();
	LightManager::getInstance().cleanup();
	TweenManager::getInstance().cleanup();
	GroupManager::getInstance().cleanup();
	TextureManager::getInstance().cleanup();
	UIElement::cleanupUISystem();
}

void Scene::processInput(GLFWwindow* window) {
	if (Input::isKeyJustPressed(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
	}
}

void Scene::renderLoop(GLFWwindow* window) {
	float lastFrame = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		const auto currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if (deltaTime > 0.1f) deltaTime = 0.1f;

		Input::update();
		processInput(window);

		scene->update(window, deltaTime, kamera.transform);

		TweenManager::getInstance().update(deltaTime);

	    glm::mat4 view = kamera.getViewMatrix();
	    glm::mat4 projection = kamera.getProjectionMatrix();

	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer.render(*scene, view, projection, kamera.transform.position);
	    renderer.drawOpaque(view, projection, kamera.transform.position);
	    RenderSkybox(skyboxShader, cubemapTexture, skyboxVAO, kamera, currentFrame);
	    renderer.drawTransparent(view, projection, kamera.transform.position);
		renderer.drawUI(view, projection);
	    renderer.clearQueues();

		glfwSwapBuffers(window);
	}
}
