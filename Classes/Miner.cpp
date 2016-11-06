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

bool Miner::resource_at_tile_pos(cocos2d::Vec2 pos)
{

    auto tile_gid = this->active_layer->getTileGIDAt(pos);
    auto tile_props = this->tilemap->getPropertiesForGID(tile_gid);

    if (tile_gid == this->resource_tile_id)
    {
        CCLOG("tile is a resource");
        return true;
    }

    return false;
}

bool Miner::is_valid_pos(cocos2d::Vec2 pos)
{
    cocos2d::Size layer_size = this->active_layer->getLayerSize();

    if (pos.x >= layer_size.width || pos.x < 0)
    {
        CCLOG("invalid tile X");
        return false;
    }
    if (pos.y >= layer_size.height || pos.y < 0)
    {
        CCLOG("invalid tile Y");
        return false;
    }
    
    return true;
}

tile_gid_t Miner::get_tile_gid_at_offset(cocos2d::Vec2 pos, cocos2d::Vec2 offset)
{
    float new_x = pos.x + offset.x;
    float new_y = pos.y + offset.y;

    if (this->is_valid_pos({ new_x, new_y }))
    {
        return this->active_layer->getTileGIDAt({ new_x, new_y });
    }
    else
    {
        return NULL;
    }
}

void Miner::move_active_tile(cocos2d::Vec2 offset)
{
    enum class DIRECTIONS {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    std::map<cocos2d::Vec2, DIRECTIONS> direction_map{
        {{-1, 0}, DIRECTIONS::TopLeft},
        {{0, -1}, DIRECTIONS::TopRight},
        {{0, 1}, DIRECTIONS::BottomLeft},
        {{1, 0}, DIRECTIONS::BottomRight},
    };

    auto offset_tile = this->get_tile_gid_at_offset(this->active_tile_pos, offset);
    if (offset_tile != NULL)
    {
        float new_x = this->active_tile_pos.x + offset.x;
        float new_y = this->active_tile_pos.y + offset.y;

        if (this->resource_at_tile_pos({new_x, new_y}))
        {
            CCLOG("not moving active tile: resource is there");
        }
        else
        {
            this->active_tile_pos.x += offset.x;
            this->active_tile_pos.y += offset.y;

            DIRECTIONS new_tile_dir = direction_map.at(offset);
            if (new_tile_dir == DIRECTIONS::BottomLeft || new_tile_dir == DIRECTIONS::TopRight)
                this->active_layer->setTileGID(this->tile_BL_TR, this->active_tile_pos);
            else
                this->active_layer->setTileGID(this->tile_TL_BR, this->active_tile_pos);

        }
    }

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
