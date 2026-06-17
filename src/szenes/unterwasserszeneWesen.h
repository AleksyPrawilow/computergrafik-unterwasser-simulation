//
// Created by Alexey Pravilov on 11/06/2026.
//
#pragma once

#ifndef COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UNTERWASSERSZENEWESEN_H
#define COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UNTERWASSERSZENEWESEN_H
#include "dieWesen/uboot.h"
#include "werkzeuge/wesen.h"


class UnterwasserszeneWesen : public Wesen {
public:
    void init() override;
private:
    Uboot * uboot = nullptr;
};


#endif //COMPUTERGRAFIK_UNTERWASSER_SIMULATION_UNTERWASSERSZENEWESEN_H
