//
// Created by Alexey Pravilov on 02/06/2026.
//

#include "fadenkreuz.h"
#include "werkzeuge/transform.h"

Fadenkreuz::Fadenkreuz() = default;

Fadenkreuz::~Fadenkreuz() {
    if (shaderProgram != 0) glDeleteProgram(shaderProgram);
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
}

void Fadenkreuz::init(float aspectRatio) {
    const char* vertexSrc = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        uniform vec2 u_Offset;
        void main() {
            gl_Position = vec4(aPos + u_Offset, 0.0, 1.0);
        }
    )";

    const char* fragmentSrc = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
    )";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexSrc, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentSrc, NULL);
    glCompileShader(fs);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    glDeleteShader(vs);
    glDeleteShader(fs);

    float wSize = 0.020f;
    float hGap = 0.006f;
    float dotSize = 0.002f;

    float vertices[] = {
        // Left Wing Line
        -wSize - hGap, 0.0f,    -hGap, 0.0f,
        // Right Wing Line
         hGap, 0.0f,             wSize + hGap, 0.0f,

        // Center Dot
        -dotSize, 0.0f,          dotSize, 0.0f,
         0.0f, -dotSize * aspectRatio,  0.0f, dotSize * aspectRatio
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Fadenkreuz::draw(const Transform& target, const glm::mat4& viewProj) const {
    constexpr float aimDistance = 20.0f;
    const glm::vec4 clipSpace = viewProj * glm::vec4(target.position + target.forward() * aimDistance, 1.0f);
    glm::vec2 ndcOffset(0.0f, 0.0f);

    if (clipSpace.w > 0.0f) {
        ndcOffset.x = clipSpace.x / clipSpace.w;
        ndcOffset.y = clipSpace.y / clipSpace.w;
    } else {
        return;
    }

    glUseProgram(shaderProgram);
    glUniform2f(glGetUniformLocation(shaderProgram, "u_Offset"), ndcOffset.x, ndcOffset.y);

    glBindVertexArray(VAO);
    glDisable(GL_DEPTH_TEST);
    glLineWidth(2.0f);

    glDrawArrays(GL_LINES, 0, 8);

    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
    glUseProgram(0);
}
