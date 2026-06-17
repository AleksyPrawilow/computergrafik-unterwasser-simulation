#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_HIMMELSBOXWESEN_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_HIMMELSBOXWESEN_H

#include "wesen.h"
#include "kamera.h"
#include <vector>
#include <string>

class HimmelsboxWesen : public Wesen {
public:
    static GLuint aktiveCubemap;

    explicit HimmelsboxWesen(const std::vector<std::string>& gesichter);

    void init() override;
    [[nodiscard]] bool hasCustomRender() const override { return true; }
    void customRender(const glm::mat4& view, const glm::mat4& projection) const override;

private:
    std::vector<std::string> gesichterPfade;
    GLuint cubemapTextur = 0;
    GLuint shaderProgramm = 0;
    GLuint vao = 0;
    GLuint vbo = 0;
};

#endif
