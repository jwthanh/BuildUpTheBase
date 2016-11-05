#include "Miner.h"

Miner::Miner()
{
    this->tilemap = cocos2d::TMXTiledMap::create("tilemaps/test_map.tmx");
    this->tilemap->setScale(0.75f);

    //defaults to center of screen
    this->active_tile_pos = {5, 5};

};
