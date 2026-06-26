//
// Created by Alexey Pravilov on 26/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_LOGO_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_LOGO_H
#include "werkzeuge/wesen.h"


class Logo : public Wesen {
public:
    void init() override;
    void prepareUniforms() const override;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_LOGO_H
