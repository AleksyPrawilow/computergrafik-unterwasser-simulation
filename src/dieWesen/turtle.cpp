#include "turtle.h"
#include "werkzeuge/shaderManager.h"
#include "werkzeuge/textur.h"

void Turtle::init() {
    material.shader = ShaderManager::getInstance().getShader("default");

    if (type == 1) {
        loadModel("assets/models/turtle1.obj");
        material.albedo = Kern::LoadTexture("assets/textures/turtle1.png");
        material.normal = Kern::LoadTexture("assets/textures/turtle1_normal.png");
        material.metallic = Kern::LoadTexture("assets/textures/turtle1_specular.png");
        material.roughness = material.metallic; 

        // Wesen* eyes = new Wesen();
        // eyes->loadModel("assets/models/turtle1_eyes.obj");
        // eyes->material.shader = material.shader;
        // eyes->material.albedo = Kern::LoadTexture("assets/textures/turtle1eyes.png");
        // addChild(eyes);
        transform.scale = glm::vec3(10.0f);
    } else {
        loadModel("assets/models/turtle2.obj");
        material.albedo = Kern::LoadTexture("assets/textures/turtle2.png");
        material.normal = Kern::LoadTexture("assets/textures/turtle2_normal.jpg");
        material.metallic = Kern::LoadTexture("assets/textures/turtle2_specular.png");
        material.roughness = material.metallic;
        transform.scale = glm::vec3(0.05f);
        transform.pitch(glm::radians(90.0f));
    }
}

void Turtle::onUpdate(GLFWwindow* window, float deltaTime, Transform& cameraTransform) {

}