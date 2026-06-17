#include "himmelsboxWesen.h"
#include "textur.h"
#include "shaderManager.h"
#include "renderWerkzeuge.h"
#include "visual/worldEnvironment.h"
#include <GLFW/glfw3.h>

GLuint HimmelsboxWesen::aktiveCubemap = 0;

static const float himmelsboxVertices[] = {
    -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f
};

HimmelsboxWesen::HimmelsboxWesen(const std::vector<std::string>& gesichter)
    : gesichterPfade(gesichter) {
    name = "himmelsbox";
}

void HimmelsboxWesen::init() {
    cubemapTextur = Kern::LoadCubemap(gesichterPfade);
    aktiveCubemap = cubemapTextur;

    shaderProgramm = ShaderManager::getInstance().loadShader(
        "skybox", "assets/shaders/skybox.vert", "assets/shaders/skybox.frag"
    );

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(himmelsboxVertices), himmelsboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void HimmelsboxWesen::customRender(const glm::mat4& view, const glm::mat4& projection) const {
    const float zeit = static_cast<float>(glfwGetTime());
    const glm::vec3 kameraPos = glm::vec3(glm::inverse(view)[3]);

    glDepthFunc(GL_LEQUAL);
    glUseProgram(shaderProgramm);

    Kern::setUniform(shaderProgramm, "projection", projection);
    Kern::setUniform(shaderProgramm, "view", view);
    Kern::setUniform(shaderProgramm, "cameraPos", kameraPos);
    Kern::setUniform(shaderProgramm, "time", zeit);

    const EnvParameters env = (WorldEnvironment::activeEnv != nullptr)
                        ? WorldEnvironment::activeEnv->params
                        : EnvParameters();

    Kern::setUniform(shaderProgramm, "u_fogColor", env.fogColor);
    Kern::setUniform(shaderProgramm, "u_sunDirection", env.sunDirection);
    Kern::setUniform(shaderProgramm, "u_heightFogEnabled", env.heightFogEnabled ? 1.0f : 0.0f);
    Kern::setUniform(shaderProgramm, "u_heightFogColor", env.heightFogColor);
    Kern::setUniform(shaderProgramm, "u_heightFogMin", env.heightFogMin);
    Kern::setUniform(shaderProgramm, "u_heightFogMax", env.heightFogMax);
    Kern::setUniform(shaderProgramm, "u_baseFogDensity", env.fogDensity);
    Kern::setUniform(shaderProgramm, "u_depthDimmingEnabled", env.depthDimmingEnabled);
    Kern::setUniform(shaderProgramm, "u_depthDimmingCoefficient", env.depthDimmingCoefficient);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextur);
    Kern::setUniform(shaderProgramm, "skybox", 0);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);
}
