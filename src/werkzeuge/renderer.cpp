//
// Created by Alexey Pravilov on 01/06/2026.
//

#include "renderer.h"

#include "textur.h"
#include "gtc/type_ptr.inl"

void Renderer::render (
    const Daswesen& e,
    const glm::mat4& viewProj,
    const glm::vec3& cameraPos)
{
    const Material& m = e.material;

    glUseProgram(m.shader);

    glm::mat4 model = e.transform.getModelMatrix();
    glm::mat4 mvp = viewProj * model;

    glUniformMatrix4fv(
        glGetUniformLocation(m.shader, "transformation"),
        1,
        GL_FALSE,
        &mvp[0][0]
    );

    glUniformMatrix4fv(
        glGetUniformLocation(m.shader, "modelMatrix"),
        1,
        GL_FALSE,
        &model[0][0]
    );

    glUniform3fv(
        glGetUniformLocation(m.shader, "cameraPos"),
        1,
        glm::value_ptr(cameraPos)
    );

    Core::SetActiveTexture(m.albedo, "colorTexture", m.shader, 0);
    Core::SetActiveTexture(m.normal, "normalMap", m.shader, 1);

    if (m.roughness)
        Core::SetActiveTexture(m.roughness, "roughnessMap", m.shader, 2);

    if (m.metallic)
        Core::SetActiveTexture(m.metallic, "metallicMap", m.shader, 3);

    Core::DrawContext(e.mesh);

    glUseProgram(0);
}
