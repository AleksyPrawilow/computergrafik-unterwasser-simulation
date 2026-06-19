//
// Created by Alexey Pravilov on 19/06/2026.
//

#include "prop.h"

Prop::Prop(const glm::vec3 position, const glm::quat orientation, const glm::vec3 scale) {
    transform.position = position;
    transform.rotation = orientation;
    transform.scale = scale;
}