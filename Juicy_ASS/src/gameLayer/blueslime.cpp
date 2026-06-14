#include "blueslime.h"

void BlueSlime::UpdateBlueSlime(GameMap& map,float dt)
{
    body.updateForces(dt);

    body.checkCollisionOnce(
        map,
        body.transform.pos
    );

    body.updateFinal();
}
