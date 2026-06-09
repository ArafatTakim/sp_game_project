#pragma once
#include <physics.h>

vec PhysicalEntity::performCollisonOnOneAxis(GameMap& gameMap, vec pos, vec delta) {
	if (delta.x == 0 && delta.y == 0) return pos;

	vec dimensions = { transform.w, transform.h };

	int minX = floor(pos.x - dimensions.x / 2.f - 1);
	int maxX = ceil(pos.x + dimensions.x / 2.f + 1);
	int minY = floor(pos.y - dimensions.y / 2.f - 1);
	int maxY = ceil(pos.y + dimensions.y / 2.f + 1);

	minX = std::max(0, minX);
	maxX = std::min(gameMap.w, maxX);
	minY = std::max(0, minY);
	maxY = std::min(gameMap.h, maxY);

	for (int x = minX; x <= maxX; x++) {
		for (int y = minY; y <= maxY; y++) {
			auto& b = gameMap.getBlock(x, y);

			if (b.isCollidable()) {
				Transform2D entity;
				entity.pos = pos;
				entity.w = dimensions.x;
				entity.h = dimensions.y;

				Transform2D block;
				block.pos = { x + 0.5f, y + 0.5f };
				block.w = block.h = 1;

				if (entity.intersectTransform(block, -0.0005f)) {

					if (delta.x != 0) {
						if (delta.x < 0) { //comming from left
							leftTouch = 1;
							pos.x = x + 1.f + dimensions.x / 2.f;
							return pos;
						}

						if (delta.x > 0) { //comming from right
							rightTouch = 1;
							pos.x = x - dimensions.x / 2.f;
							return pos;
						}
					}

					else if (delta.y != 0) {
						if (delta.y < 0) { //comming from down
							upTouch = 1;
							pos.y = y + 1.f + dimensions.y / 2.f;
							return pos;
						}

						if (delta.y > 0) { //comming from up
							downTouch = 1;
							pos.y = y - dimensions.y / 2.f;
							return pos;
						}
					}
				}
			}
		}
	}
	return pos;
}


void PhysicalEntity::checkCollisionOnce(GameMap& gameMap, vec &pos) {
	vec delta = subtractVectors(pos, lastPosition);

	vec newPos = performCollisonOnOneAxis(gameMap, { pos.x, lastPosition.y }, { delta.x, 0 });

	newPos = performCollisonOnOneAxis(gameMap, { newPos.x, pos.y }, { 0, delta.y });

	pos = newPos;

}