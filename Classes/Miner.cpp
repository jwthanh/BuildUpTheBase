#include "Miner.h"

Miner::Miner()
{
    this->tilemap = cocos2d::TMXTiledMap::create("tilemaps/test_map.tmx");
    this->tilemap->setScale(0.75f);

    this->active_layer = this->tilemap->getLayer("background");

    //defaults to center of screen
    this->active_tile_pos = {4, 4};
    this->active_tile_id = 7;

    //CCASSERT(pos.x < _layerSize.width && pos.y < _layerSize.height && pos.x >=0 && pos.y >=0, "TMXLayer: invalid position");
}

void Miner::move_active_tile(cocos2d::Vec2 offset)
{
    cocos2d::Size layer_size = this->active_layer->getLayerSize();
    float new_x = this->active_tile_pos.x + offset.x;
    float new_y = this->active_tile_pos.y + offset.y;

    if (new_x >= layer_size.width || new_x < 0)
    {
        CCLOG("invalid tile X");
        return;
    }
    if (new_y >= layer_size.height || new_y < 0)
    {
        CCLOG("invalid tile Y");
        return;
    }

    this->active_tile_pos.x += offset.x;
    this->active_tile_pos.y += offset.y;
    this->active_layer->setTileGID(this->active_tile_id, this->active_tile_pos);
};

void Miner::move_active_up()
{
    this->move_active_tile({0, -1});
};

void Miner::move_active_down()
{
    this->move_active_tile({0, 1});

};

void Miner::move_active_left()
{

    this->move_active_tile({-1, 0});
};

void Miner::move_active_right()
{
    this->move_active_tile({1, 0});
};
