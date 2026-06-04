//
// Created by Alexey Pravilov on 02/06/2026.
//

#include "shaderManager.h"

GLuint ShaderManager::getShader(const std::string& name) {
    if (const auto it = shaderMap.find(name); it != shaderMap.end()) {
        return it->second;
    }
    return 0;
}

GLuint ShaderManager::loadShader(const std::string& name, const char* vertPath, const char* fragPath) {
    if (auto it = shaderMap.find(name); it != shaderMap.end()) {
        return it->second;
    }

    const GLuint program = shaderLoader.CreateProgram(vertPath, fragPath);
    shaderMap[name] = program;
    return program;
}

void ShaderManager::cleanup() {
    for (auto& pair : shaderMap) {
        shaderLoader.DeleteProgram(pair.second);
    }
    shaderMap.clear();
}
