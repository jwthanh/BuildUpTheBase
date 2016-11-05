#pragma once
#ifndef MINER_H
#define MINER_H

#include "cocos2d.h"

class Miner
{
    public:
        cocos2d::TMXTiledMap* tilemap;
        cocos2d::Vec2 active_tile_pos;

        Miner();
};

#endif
