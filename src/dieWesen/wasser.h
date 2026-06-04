//
// Created by Alexey Pravilov on 04/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WASSER_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WASSER_H
#include "werkzeuge/wesen.h"


class Wasser: public Wesen {
public:
    void init() override;
    void prepareUniforms() const override;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WASSER_H
