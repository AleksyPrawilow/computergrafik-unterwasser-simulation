//
// Created by Alexey Pravilov on 20/06/2026.
//
#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_BOTTLE_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_BOTTLE_H
#include "werkzeuge/wesen.h"


class Bottle : public Wesen {
public:
    void init() override;
private:
    Wesen * cork = nullptr;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_BOTTLE_H
