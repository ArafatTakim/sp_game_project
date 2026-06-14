#include "blueslime.h"

void UpdateBlueSlime(
    BlueSlime& slime,
    GameMap& map,
    float dt)
{
    slime.body.updateForces(dt);

    slime.body.checkCollisionOnce(
        map,
        slime.body.transform.pos
    );

    slime.body.updateFinal();
}
