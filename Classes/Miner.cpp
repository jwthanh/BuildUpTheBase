#include "Miner.h"

Miner::Miner()
{
    this->tilemap = cocos2d::TMXTiledMap::create("tilemaps/test_map.tmx");
    this->tilemap->setScale(0.75f);

    this->active_layer = this->tilemap->getLayer("background");

    //defaults to center of screen
    this->active_tile_pos = {4, 4};
    this->active_tile_id = 7;
};

void Miner::move_active_up()
{
    this->active_tile_pos.y--;
    this->active_layer->setTileGID(this->active_tile_id, this->active_tile_pos);
};

void Miner::move_active_down()
{
    this->active_tile_pos.y++;
    this->active_layer->setTileGID(this->active_tile_id, this->active_tile_pos);

};

void Miner::move_active_left()
{

    this->active_tile_pos.x--;
    this->active_layer->setTileGID(this->active_tile_id, this->active_tile_pos);
};

void Miner::move_active_right()
{
    this->active_tile_pos.x++;
    this->active_layer->setTileGID(this->active_tile_id, this->active_tile_pos);

};
