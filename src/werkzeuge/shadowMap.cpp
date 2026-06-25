#include "shadowMap.h"
#include <gtc/matrix_transform.hpp>

void ShadowMap::init(int res) {
    resolution = res;

    glGenFramebuffers(1, &fbo);

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::cleanup() {
    if (fbo != 0) glDeleteFramebuffers(1, &fbo);
    if (depthTexture != 0) glDeleteTextures(1, &depthTexture);
    fbo = 0;
    depthTexture = 0;
}

void ShadowMap::updateDirectional(const glm::vec3& lightDir, const glm::vec3& sceneCenter, float sceneRadius) {
    glm::vec3 dir = glm::normalize(lightDir);
    glm::vec3 lightPos = sceneCenter + dir * sceneRadius;

    glm::mat4 lightView = glm::lookAt(lightPos, sceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProj = glm::ortho(-sceneRadius, sceneRadius, -sceneRadius, sceneRadius, 0.1f, sceneRadius * 2.0f);

    lightSpaceMatrix = lightProj * lightView;
}

void ShadowMap::beginPass() {
    glGetIntegerv(GL_VIEWPORT, savedViewport);
    glViewport(0, 0, resolution, resolution);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
}

void ShadowMap::endPass() {
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(savedViewport[0], savedViewport[1], savedViewport[2], savedViewport[3]);
}
