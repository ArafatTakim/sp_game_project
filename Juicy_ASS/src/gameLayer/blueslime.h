#pragma once

#include <physics.h>

struct BlueSlime
{
    PhysicalEntity body;

    float health = 20.0f;

    bool active = true;

    void UpdateBlueSlime(
        GameMap& map,
        float dt);
};
