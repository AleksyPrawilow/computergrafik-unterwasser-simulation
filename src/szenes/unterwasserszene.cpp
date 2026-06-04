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
Uboot uboot;
Wasser wasser;
Wesen earth;
Wesen rock;
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

	wasser.init();

	initSkybox();
	cubemapTexture = Kern::LoadCubemap(skyboxFaces);
	skyboxShader = ShaderManager::getInstance().loadShader("skybox", "assets/shaders/skybox.vert", "assets/shaders/skybox.frag");

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
	uboot.transform.scale = glm::vec3(0.04f);
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

		if (constexpr float WATER_HEIGHT = 0.0f; kamera.transform.position.y < WATER_HEIGHT) {
			glClearColor(0.0f, 0.05f, 0.15f, 1.0f);
		} else {
			glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (const Wesen * wesen: diewesen) {
			renderer.render(*wesen, viewProj, kamera.transform.position);
		}

		fadenkreuz.draw(uboot.transform, viewProj);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		renderer.render(wasser, viewProj, kamera.transform.position);
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		RenderSkybox(skyboxShader, cubemapTexture, skyboxVAO, kamera);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}