//
// Created by Alexey Pravilov on 02/06/2026.
//

#include "sceneManager.h"
#include "glew.h"
#include <GLFW/glfw3.h>
#include "ext.hpp"
#include <vector>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
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
#include "werkzeuge/audio/audioManager.h"
#include "werkzeuge/ui/worldspaceUI.h"
#include "werkzeuge/visual/worldEnvironment.h"

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
WorldEnvironment * WorldEnvironment::activeEnv = nullptr;
auto * scene = new UnterwasserszeneWesen();
bool cursorDisabled = true;

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
	AudioManager::getInstance().init();
	Input::init(window);

	initSkybox();
	cubemapTexture = Kern::LoadCubemap(skyboxFaces);
	skyboxShader = ShaderManager::getInstance().loadShader("skybox", "assets/shaders/skybox.vert", "assets/shaders/skybox.frag");

	scene->init();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	const ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410");
}

void Scene::shutdown(GLFWwindow* window) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

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

	if (Input::isKeyJustPressed(GLFW_KEY_TAB)) {
		std::cout << "Tab" << std::endl;
		cursorDisabled = !cursorDisabled;
		glfwSetInputMode(window, GLFW_CURSOR, cursorDisabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
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

		if (cursorDisabled) {
			scene->update(window, deltaTime, kamera.transform);
		} else {
			scene->update(window, 0.0f, kamera.transform);
		}

		AudioManager::getInstance().updateListener(kamera.transform.position, kamera.transform.forward(), kamera.transform.up());
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

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (WorldEnvironment::activeEnv != nullptr && !cursorDisabled) {
            ImGui::Begin("World Environment Tweaker");

            EnvParameters& params = WorldEnvironment::activeEnv->params;

            ImGui::Separator();

            if (ImGui::CollapsingHeader("Sun / Star Light", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderFloat3("Sun Direction", &params.sunDirection[0], -1.0f, 1.0f);
                if (glm::length(params.sunDirection) > 0.001f) {
                    params.sunDirection = glm::normalize(params.sunDirection);
                }
                ImGui::ColorEdit3("Sun Color", &params.sunColor[0]);
                ImGui::SliderFloat("Sun Energy", &params.sunEnergy, 0.0f, 5.0f);
            }

            if (ImGui::CollapsingHeader("Ambient Light", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::ColorEdit3("Ambient Color", &params.ambientColor[0]);
                ImGui::SliderFloat("Ambient Energy", &params.ambientEnergy, 0.0f, 1.0f);
            }

            if (ImGui::CollapsingHeader("Distance Fog", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Distance Fog Active", &params.fogEnabled);
                ImGui::ColorEdit3("Fog Color", &params.fogColor[0]);
                ImGui::SliderFloat("Fog Density", &params.fogDensity, 0.0f, 0.1f, "%.4f");
            }

            if (ImGui::CollapsingHeader("Height / Depth Fog", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Height Fog Active", &params.heightFogEnabled);
                ImGui::ColorEdit3("Height Fog Color", &params.heightFogColor[0]);
                ImGui::SliderFloat("Ceiling height (Max Y)", &params.heightFogMax, -50.0f, 50.0f);
                ImGui::SliderFloat("Floor height (Min Y)", &params.heightFogMin, -150.0f, 0.0f);
            }

            if (ImGui::CollapsingHeader("Water Caustics")) {
                ImGui::Checkbox("Caustics Active", &params.causticsEnabled);
                ImGui::ColorEdit3("Caustics Color", &params.causticsColor[0]);
                ImGui::SliderFloat("Caustics Scale", &params.causticsScale, 0.01f, 0.3f, "%.3f");
                ImGui::SliderFloat("Caustics Intensity", &params.causticsIntensity, 0.0f, 5.0f);
            }

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
}
