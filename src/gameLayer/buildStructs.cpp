#include <buildStructs.h>
#include <gameMap.h>
#include <blocks.h>
#include <cstdlib>
namespace structure{
void buildTree(GameMap& gameMap, int x, int y)
{
    int height = 4 + rand() % 4;

    // trunk
    for (int i = 0; i < height; i++) {
        gameMap.getBlock(x, y - i).type = Block::woodLog;
    }

    int top = y - height;

    // leaves
    for (int dy = 0; dy < 3; dy++) {

        int radius = 3 - dy;

        for (int dx = -radius; dx <= radius; dx++) {

            if (rand() % 100 < 80) {
                gameMap.getBlock(x + dx, top - dy).type = Block::leaves;
            }
        }
    }
}

    gameMap.getBlock(x, top - 3).type = Block::leaves;
}
