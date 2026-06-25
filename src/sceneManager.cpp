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
#include "dieWesen/leviathan.h"
#include "szenes/labsWesen.h"
#include "szenes/unterwasserszeneWesen.h"
#include "szenes/weltraumszeneWesen.h"
#include "werkzeuge/textur.h"
#include "werkzeuge/transform.h"
#include "werkzeuge/renderer.h"
#include "werkzeuge/visual/questManager.h"
#include "werkzeuge/wesen.h"
#include "werkzeuge/kamera.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/visual/lightManager.h"
#include "werkzeuge/visual/tween.h"
#include "werkzeuge/input.h"
#include "werkzeuge/textureManager.h"
#include "werkzeuge/audio/audioManager.h"
#include "werkzeuge/ui/worldspaceUI.h"
#include "werkzeuge/visual/worldEnvironment.h"
#include "werkzeuge/groupManager.h"
#include "werkzeuge/bloom.h"
#include "werkzeuge/shadowMap.h"
#include "werkzeuge/modelManager.h"
#include "werkzeuge/audio/musicManager.h"
#include "werkzeuge/gegenstandDaten.h"
#include "werkzeuge/inventar.h"

Renderer renderer;
Kamera kamera;
Bloom bloom;
ShadowMap shadowMap;
WorldEnvironment * WorldEnvironment::activeEnv = nullptr;
Wesen * scene = nullptr;
Leviathan * leviathan = nullptr;
int aktuelleSzene = 0;
bool cursorDisabled = true;
bool inventarOffen = false;

void Scene::framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
	kamera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
	glViewport(0, 0, width, height);
}

void Scene::szeneWechseln(int index) {
	if (scene != nullptr) {
		AudioManager::getInstance().allesStoppen();
		MusicManager::getInstance().stopAll();
		TweenManager::getInstance().cleanup();
		QuestManager::getInstance().cleanup();
		Inventar::getInstance().clearCallbacks();
		GroupManager::getInstance().cleanup();
		delete scene;
		scene = nullptr;
		WorldEnvironment::activeEnv = nullptr;
		LightManager::getInstance().cleanup();
	}

	aktuelleSzene = index;
	if (index == 0) {
		scene = new UnterwasserszeneWesen();
	} else {
		scene = new LabsWesen();
	}
	scene->init();
	const auto& leviathanGroup = GroupManager::getInstance().getEntitiesInGroup("Leviathan");
	leviathan = leviathanGroup.empty() ? nullptr : dynamic_cast<Leviathan*>(leviathanGroup[0]);
}

void Scene::init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glEnable(GL_DEPTH_TEST);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported()) {
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}

	int fbW, fbH;
	glfwGetFramebufferSize(window, &fbW, &fbH);

	bool apple = false;

#ifdef __APPLE__
	apple = true;
#endif

	if (apple) {
		int winW, winH;
		glfwGetWindowSize(window, &winW, &winH);

		// Store globally in the UI system
		UIElement::dpiScale = static_cast<float>(fbW) / static_cast<float>(winW) * 0.5f;
	} else {
		float xscale, yscale;
		glfwGetWindowContentScale(window, &xscale, &yscale);
		UIElement::dpiScale = xscale;
	}

	renderer.init();
	UIElement::initUISystem();
	AudioManager::getInstance().init();
	GegenstandDaten::getInstance().init();
	Input::init(window);

	bloom.init(fbW, fbH);
	shadowMap.init(1024);

	szeneWechseln(0);

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

	bloom.cleanup();
	shadowMap.cleanup();
	ShaderManager::getInstance().cleanup();
	LightManager::getInstance().cleanup();
	TweenManager::getInstance().cleanup();
	GroupManager::getInstance().cleanup();
	TextureManager::getInstance().cleanup();
	ModelManager::getInstance().cleanup();
	UIElement::cleanupUISystem();
	Kern::clearUniformCache();
}

void Scene::processInput(GLFWwindow* window) {
	if (Input::isKeyJustPressed(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
	}

	if (Input::isKeyJustPressed(GLFW_KEY_TAB) && !inventarOffen) {
		cursorDisabled = !cursorDisabled;
		glfwSetInputMode(window, GLFW_CURSOR, cursorDisabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}

	if (Input::isKeyJustPressed(GLFW_KEY_F1) && aktuelleSzene != 0) {
		szeneWechseln(0);
	}
	if (Input::isKeyJustPressed(GLFW_KEY_F4) && aktuelleSzene != 1) {
		szeneWechseln(1);
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
		kamera.updateShake(deltaTime);
		processInput(window);

		if (cursorDisabled) {
			scene->update(window, deltaTime, kamera.transform);
		} else {
			scene->update(window, 0.0f, kamera.transform);
		}

		AudioManager::getInstance().updateListener(kamera.transform.position, kamera.transform.forward(), kamera.transform.up());
		TweenManager::getInstance().update(deltaTime);
		MusicManager::getInstance().onUpdate(window, deltaTime, kamera.transform);

	    glm::mat4 view = kamera.getViewMatrix();
	    glm::mat4 projection = kamera.getProjectionMatrix();
		renderer.updateFrustum(view, projection);

		EnvParameters bloomParams;
		if (WorldEnvironment::activeEnv != nullptr) {
			bloomParams = WorldEnvironment::activeEnv->params;
			glm::vec3 shadowSunDir = glm::normalize(glm::vec3(0.3f, 0.8f, 0.5f));
			shadowMap.updateDirectional(shadowSunDir, glm::vec3(-700.0f, 10.0f, -220.0f), 120.0f);
			renderer.lightSpaceMatrix = shadowMap.getLightSpaceMatrix();
		}

		renderer.sendEnvironment(kamera.transform.position);
		renderer.render(*scene, view, projection, kamera.transform.position);

		shadowMap.beginPass();
		renderer.shadowPass(shadowMap);
		shadowMap.endPass();

		bloom.beginCapture();
	    renderer.drawOpaque(view, projection, kamera.transform.position);
	    renderer.drawHimmelsbox(view, projection);
	    renderer.drawTransparent(view, projection, kamera.transform.position);
		renderer.drawDebugAABBs(view, projection);
		bloom.endCaptureAndProcess(bloomParams.bloomThreshold, bloomParams.bloomIntensity, bloomParams.bloomEnabled);

		renderer.drawUI(view, projection);
	    renderer.clearQueues();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (!cursorDisabled) {
			ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
			ImGui::SetNextWindowBgAlpha(0.35f);

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

			ImGui::Begin("Player Coordinates", nullptr, window_flags);
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Player Position");
			ImGui::Separator();
			ImGui::Text("X: %.2f", kamera.transform.position.x);
			ImGui::Text("Y: %.2f", kamera.transform.position.y);
			ImGui::Text("Z: %.2f", kamera.transform.position.z);
			ImGui::End();
		}

		if (WorldEnvironment::activeEnv != nullptr && !cursorDisabled && !inventarOffen) {
            ImGui::Begin("World Environment Tweaker");

            EnvParameters& params = WorldEnvironment::activeEnv->params;

            ImGui::Separator();

			ImGui::Text("Debug Tools");
			ImGui::Checkbox("Show Mesh AABBs", &renderer.showDebugAABBs);
			ImGui::Separator();

			if (leviathan) {
				ImGui::SliderFloat("offset X", &leviathan->clawXOffset, 0.0f, 100.0f);
				ImGui::SliderFloat("offset Y", &leviathan->clawYOffset, 0.0f, 100.0f);
				ImGui::SliderFloat("offset Z", &leviathan->clawZOffset, 0.0f, 100.0f);
				ImGui::SliderFloat("offset Y general", &leviathan->lightsabersYOffset, -100.0f, 100.0f);
			}

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

			if (ImGui::CollapsingHeader("Depth dimming", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Checkbox("Dimming enabled", &params.depthDimmingEnabled);
				ImGui::SliderFloat("Dimming coefficient", &params.depthDimmingCoefficient, 0.0f, 1.0f);
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

            if (ImGui::CollapsingHeader("Bloom", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Bloom Enabled", &params.bloomEnabled);
                ImGui::SliderFloat("Bloom Threshold", &params.bloomThreshold, 0.0f, 0.5f);
                ImGui::SliderFloat("Bloom Intensity", &params.bloomIntensity, 0.0f, 3.0f);
            }

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
}
