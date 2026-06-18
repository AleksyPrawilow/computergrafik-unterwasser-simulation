#pragma once
#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WELTRAUMSZENEWESEN_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_WELTRAUMSZENEWESEN_H

#include "dieWesen/timer.h"
#include "werkzeuge/wesen.h"

class WeltraumszeneWesen : public Wesen {
public:
    void init() override;
    void spawnWave(Timer * timer, int numA, int numB);
};

#endif
