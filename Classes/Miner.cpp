#include "Miner.h"
#include <array>

#include "FShake.h"
#include "MiscUI.h"

#include "cocos2d.h"

Miner::Miner()
{
    this->tilemap = cocos2d::TMXTiledMap::create("tilemaps/test_map.tmx");
    this->tilemap->setScale(0.75f);

    this->active_layer = this->tilemap->getLayer("background");

    //defaults to center of screen
    this->prev_active_tile_pos = {4, 4};
    this->active_tile_pos = {3, 4};

    //CCASSERT(pos.x < _layerSize.width && pos.y < _layerSize.height && pos.x >=0 && pos.y >=0, "TMXLayer: invalid position");
}

bool Miner::get_tile_is_blocked_pos(cocos2d::Vec2 pos)
{

    auto tile_gid = this->active_layer->getTileGIDAt(pos);
    auto tile_props = this->tilemap->getPropertiesForGID(tile_gid);

    if (tile_gid == this->resource_tile_id)
    {
        CCLOG("tile is a resource");
        return true;
    }

    std::vector<tile_gid_t> rail_gids = {
        this->tile_TL_BR,
        this->tile_BL_TR,

        this->tile_TL_TR,
        this->tile_TR_BR,
        this->tile_BL_BR,
        this->tile_TL_BL,
    };

    auto match = std::find(rail_gids.begin(), rail_gids.end(), tile_gid);
    if (match != rail_gids.end())
    {
        CCLOG("tile is a rail");
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

    std::map<cocos2d::Vec2, DIRECTIONS> direction_map {
        {{-1, 0}, DIRECTIONS::TopLeft},
        {{0, -1}, DIRECTIONS::TopRight},
        {{0, 1}, DIRECTIONS::BottomLeft},
        {{1, 0}, DIRECTIONS::BottomRight},
    };

    std::map<tile_gid_t, std::vector<std::array<DIRECTIONS, 2>>> tile_direction_map{
        //crosses
        { 
            this->tile_BL_TR, {
                { DIRECTIONS::BottomLeft, DIRECTIONS::TopRight},
                { DIRECTIONS::TopRight, DIRECTIONS::BottomLeft}
            }
        },

        {
            this->tile_TL_BR, {
                { DIRECTIONS::TopLeft, DIRECTIONS::BottomRight},
                { DIRECTIONS::BottomRight, DIRECTIONS::TopLeft}
            }
        },

        //corners
        { this->tile_TL_TR, {
                { DIRECTIONS::TopLeft, DIRECTIONS::TopRight},
                { DIRECTIONS::TopRight, DIRECTIONS::TopLeft}
            }
        },

        { this->tile_TR_BR, {
                { DIRECTIONS::TopRight, DIRECTIONS::BottomRight},
                { DIRECTIONS::BottomRight, DIRECTIONS::TopRight}
            }
        },

        { this->tile_BL_BR, {
                { DIRECTIONS::BottomLeft, DIRECTIONS::BottomRight},
                { DIRECTIONS::BottomRight, DIRECTIONS::BottomLeft}
            }
        },

        { this->tile_TL_BL, {
                { DIRECTIONS::TopLeft, DIRECTIONS::BottomLeft},
                { DIRECTIONS::BottomLeft, DIRECTIONS::TopLeft}
            }
        },
    };

    //which directions connect across tiles
    std::map<DIRECTIONS, DIRECTIONS> connection_map {
        {DIRECTIONS::TopLeft, DIRECTIONS::BottomRight},
        {DIRECTIONS::BottomRight, DIRECTIONS::TopLeft},
        {DIRECTIONS::TopRight, DIRECTIONS::BottomLeft},
        {DIRECTIONS::BottomLeft, DIRECTIONS::TopRight},
    };

    auto offset_tile = this->get_tile_gid_at_offset(this->active_tile_pos, offset);
    if (offset_tile != NULL)
    {
        float new_x = this->active_tile_pos.x + offset.x;
        float new_y = this->active_tile_pos.y + offset.y;

        if (this->get_tile_is_blocked_pos({new_x, new_y}))
        {
            CCLOG("not moving active tile: tile is blocked");

            cocos2d::Vec2 pos = { new_x, new_y };

            //shake map
            auto shake = FShake::actionWithDuration(0.1f, 2.5f, 2.5f);
            cocos2d::Sprite* blocked_sprite = this->active_layer->getTileAt(pos);
            //animate specific tile
            animate_flash_action(blocked_sprite, 0.2f, 1.1f);
            this->tilemap->runAction(shake);
        }
        else
        {
            //connection between previous and active tiles
            cocos2d::Vec2 difference = this->prev_active_tile_pos-this->active_tile_pos;
            DIRECTIONS last_connection = direction_map[difference];

            //based on the new tile and the last connection, figure out how the old active
            // tile should look
            DIRECTIONS new_tile_dir = direction_map.at(offset);
            std::array<DIRECTIONS, 2> old_tile_new_dir = {last_connection, new_tile_dir};

            //go through the list of possible directions and pull out the matching tile for
            //the two connection
            tile_gid_t new_old_tile_id = 7; //crisscross tile, for graceful fails
            for (auto& pair : tile_direction_map)
            {
                tile_gid_t gid = pair.first;
                for (auto& direction: pair.second)
                {
                    if (direction == old_tile_new_dir)
                    {
                        new_old_tile_id = gid;
                    }
                }
            }

            //set the old tile to the new image
            this->active_layer->setTileGID(new_old_tile_id, this->active_tile_pos);

            //update prev_active and active tile pos
            this->prev_active_tile_pos = this->active_tile_pos;
            this->active_tile_pos.x = new_x;
            this->active_tile_pos.y = new_y;

            //the new tile direction will be an across tile
            if (new_tile_dir == DIRECTIONS::BottomLeft || new_tile_dir == DIRECTIONS::TopRight)
            {
                this->active_layer->setTileGID(this->tile_BL_TR, this->active_tile_pos);
            }
            else
            {
                this->active_layer->setTileGID(this->tile_TL_BR, this->active_tile_pos);
            }

        }
    }

};

void Miner::move_active_top_right()
{
    this->move_active_tile({0, -1});
};

void Miner::move_active_bottom_left()
{
    this->move_active_tile({0, 1});
};

void Miner::move_active_top_left()
{
    this->move_active_tile({-1, 0});
};

void Miner::move_active_bottom_right()
{
    this->move_active_tile({1, 0});
}

void Miner::save()
{
    std::vector<std::vector<tile_gid_t>> layers;
    for (auto child : this->tilemap->getChildren())
    {
        auto layer = dynamic_cast<cocos2d::TMXLayer*>(child);
        if (layer != NULL)
        {
            auto serialized_tiles = this->serialize_layer(layer);
            layers.push_back(serialized_tiles);
        }
    }
}

std::vector<tile_gid_t> Miner::serialize_layer(cocos2d::TMXLayer* layer)
{
    cocos2d::Size layer_size = layer->getLayerSize();

    std::vector<tile_gid_t> tiles = std::vector<tile_gid_t>();
    tiles.reserve(int(layer_size.height*layer_size.width));

    for (tile_gid_t y = 0; y < layer_size.height; y++)
    {
        for (tile_gid_t x = 0; x < layer_size.width; x++)
        {
            tile_gid_t pos = static_cast<int>(x + layer_size.width * y);
            tile_gid_t gid = layer->getTiles()[pos];
            tiles.push_back(gid);
        }
    }

    return tiles;
};
