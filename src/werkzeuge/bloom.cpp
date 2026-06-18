#include "bloom.h"
#include "shaderManager.h"
#include "renderWerkzeuge.h"

void Bloom::init(int width, int height) {
    extractShader = ShaderManager::getInstance().loadShader(
        "bloom_extract",
        "assets/shaders/postprocess.vert",
        "assets/shaders/bloom_extract.frag"
    );
    blurShader = ShaderManager::getInstance().loadShader(
        "bloom_blur",
        "assets/shaders/postprocess.vert",
        "assets/shaders/bloom_blur.frag"
    );
    compositeShader = ShaderManager::getInstance().loadShader(
        "bloom_composite",
        "assets/shaders/postprocess.vert",
        "assets/shaders/bloom_composite.frag"
    );

    initQuad();
    createFBOs(width, height);
}

void Bloom::initQuad() {
    float quadVertices[] = {
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void Bloom::createFBOs(int width, int height) {
    fbWidth = width;
    fbHeight = height;

    // Scene FBO (full resolution)
    glGenFramebuffers(1, &sceneFBO);
    glGenTextures(1, &sceneColorTex);
    glGenRenderbuffers(1, &sceneDepthRBO);

    glBindTexture(GL_TEXTURE_2D, sceneColorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &sceneBloomTex);
    glBindTexture(GL_TEXTURE_2D, sceneBloomTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindRenderbuffer(GL_RENDERBUFFER, sceneDepthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneColorTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, sceneBloomTex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sceneDepthRBO);

    GLenum drawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Ping-pong FBOs (half resolution for blur)
    int halfW = width / 2;
    int halfH = height / 2;
    if (halfW < 1) halfW = 1;
    if (halfH < 1) halfH = 1;

    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongTex);

    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, pingpongTex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, halfW, halfH, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongTex[i], 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Bloom::destroyFBOs() {
    if (sceneFBO) { glDeleteFramebuffers(1, &sceneFBO); sceneFBO = 0; }
    if (sceneColorTex) { glDeleteTextures(1, &sceneColorTex); sceneColorTex = 0; }
    if (sceneBloomTex) { glDeleteTextures(1, &sceneBloomTex); sceneBloomTex = 0; }
    if (sceneDepthRBO) { glDeleteRenderbuffers(1, &sceneDepthRBO); sceneDepthRBO = 0; }
    if (pingpongFBO[0]) { glDeleteFramebuffers(2, pingpongFBO); pingpongFBO[0] = pingpongFBO[1] = 0; }
    if (pingpongTex[0]) { glDeleteTextures(2, pingpongTex); pingpongTex[0] = pingpongTex[1] = 0; }
}

void Bloom::resize(int width, int height) {
    if (width == fbWidth && height == fbHeight) return;
    destroyFBOs();
    createFBOs(width, height);
}

void Bloom::cleanup() {
    destroyFBOs();
    if (quadVAO) { glDeleteVertexArrays(1, &quadVAO); quadVAO = 0; }
    if (quadVBO) { glDeleteBuffers(1, &quadVBO); quadVBO = 0; }
}

void Bloom::beginCapture() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    resize(viewport[2], viewport[3]);

    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Bloom::endCaptureAndProcess(float threshold, float intensity, bool enabled) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST);

    if (enabled) {
        int halfW = fbWidth / 2;
        int halfH = fbHeight / 2;
        if (halfW < 1) halfW = 1;
        if (halfH < 1) halfH = 1;

        // 1. Extract bright pixels → pingpong[0]
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
        glViewport(0, 0, halfW, halfH);
        glUseProgram(extractShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneBloomTex);
        Kern::setUniform(extractShader, "u_bloomTexture", 0);
        Kern::setUniform(extractShader, "u_threshold", threshold);
        Kern::DrawQuad(quadVAO);

        // 2. Ping-pong Gaussian blur
        glUseProgram(blurShader);
        Kern::setUniform(blurShader, "u_image", 0);
        bool horizontal = true;
        for (int i = 0; i < BLUR_PASSES * 2; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal ? 1 : 0]);
            Kern::setUniform(blurShader, "u_horizontal", horizontal);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pingpongTex[horizontal ? 0 : 1]);
            Kern::DrawQuad(quadVAO);
            horizontal = !horizontal;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // 3. Composite to screen
    glViewport(0, 0, fbWidth, fbHeight);
    glUseProgram(compositeShader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneColorTex);
    Kern::setUniform(compositeShader, "u_sceneTexture", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongTex[0]);
    Kern::setUniform(compositeShader, "u_bloomTexture", 1);

    Kern::setUniform(compositeShader, "u_bloomIntensity", intensity);
    Kern::setUniform(compositeShader, "u_bloomEnabled", enabled);
    Kern::DrawQuad(quadVAO);

    glUseProgram(0);
    glEnable(GL_DEPTH_TEST);
}
