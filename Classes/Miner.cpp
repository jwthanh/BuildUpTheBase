#include "Miner.h"
#include <array>

#include "FShake.h"
#include "MiscUI.h"

#include "cocos2d.h"
#include "Serializer.h"

Miner::Miner(cocos2d::Node* parent)
{
    this->init(true); //use_existing = true

    this->parent = parent;
    this->parent->addChild(this->tilemap);

}

cocos2d::Vec2 Miner::get_default_start_pos()
{
    ///randomly pick a starting position in the map

    cocos2d::Size layer_size = this->active_layer->getLayerSize();
    cocos2d::Vec2 start_pos = { -1, -1 };
    tile_gid_t num_tiles = static_cast<tile_gid_t>(layer_size.width * layer_size.height);

    std::mt19937 gen = std::mt19937(std::random_device{}());
    //guessing offsets so it cant be the last tile or the first tile TODO make
    //sure the prev tile is valid
    std::uniform_int_distribution<tile_gid_t> distribution(1, num_tiles - 2);
    tile_gid_t start_id = distribution(gen);

    tile_gid_t counter = 0;
    for (tile_gid_t y = 0; y < layer_size.height; y++)
    {
        for (tile_gid_t x = 0; x < layer_size.width; x++)
        {
            counter++;
            if (start_id == counter)
            {
                float fx = float(x);
                float fy = float(y);
                start_pos = { fx, fy };
                return start_pos;
            };

        }
    };
    return start_pos;
}

cocos2d::Vec2 Miner::get_existing_start_pos()
{
    ///pick a starting position in the map based on the existing tiles

    cocos2d::Size layer_size = this->active_layer->getLayerSize();
    cocos2d::Vec2 start_pos = { -1, -1 };

    for (tile_gid_t y = 0; y < layer_size.height; y++)
    {
        for (tile_gid_t x = 0; x < layer_size.width; x++)
        {
            float fx = float(x);
            float fy = float(y);
            start_pos = { fx, fy };
            tile_gid_t existing_tile_id = this->active_layer->getTileGIDAt(start_pos);
            if (existing_tile_id == this->tile_START)
            {
                return start_pos;
            };

        }
    };
    return start_pos;
}

cocos2d::Vec2 Miner::get_start_pos()
{

    MinerSerializer serializer = MinerSerializer("alpha_tilemap.json", this);
    serializer.load();
    bool use_existing_start = serializer.existing_json_found;

    CCLOG("using existing json %i", use_existing_start);
    if (use_existing_start == false)
    {
        return this->get_default_start_pos();
    }
    else 
    {
        return this->active_tile_pos; //serializer sets it
    }
}

void Miner::init(bool use_existing)
{

    this->tilemap = cocos2d::TMXTiledMap::create("tilemaps/test_map.tmx");
    this->tilemap->setScale(0.75f);

    this->active_layer = this->tilemap->getLayer("background");

    cocos2d::Vec2 start_pos;
    if (use_existing == false)
    {
        start_pos = this->get_default_start_pos();
    }
    else
    {
        MinerSerializer serializer = MinerSerializer("alpha_tilemap.json", this);
        serializer.load();
        start_pos = this->active_tile_pos; //serializer sets it
    };

    this->init_start_pos(start_pos);
};

void Miner::init_start_pos(cocos2d::Vec2 new_start_pos)
{
    //since we subtract 1 from it later, we need to make sure it's never out of bounds
    // I dont think y will be problem
    if (new_start_pos.x == 0)
    {
        new_start_pos.x += 1;
    }

    this->prev_active_tile_pos = new_start_pos - cocos2d::Vec2{-1, 0};
    this->active_tile_pos = new_start_pos;
    this->active_layer->setTileGID(this->tile_START, this->active_tile_pos);
};

void Miner::reset()
{
    // deletes the tilemap and rebuilds it from scratch
    if (this->tilemap != NULL)
    {
        this->tilemap->removeFromParent();
    }

    this->init(false); //use_existing = false
    this->parent->addChild(this->tilemap);
};

bool Miner::get_tile_is_blocked_pos(cocos2d::Vec2 pos)
{

    auto tile_gid = this->active_layer->getTileGIDAt(pos);
    auto tile_props = this->tilemap->getPropertiesForGID(tile_gid);

    if (tile_gid == this->resource_tile_id)
    {
        CCLOG("tile is a resource");
        auto firework = cocos2d::ParticleFireworks::create();
        firework->setDuration(1.0f);

        auto sprite = this->active_layer->getTileAt(pos);
        auto fire_pos = sprite->getPosition();
        fire_pos.x += 132.0f/2;
        fire_pos.y += 132.0f/2;
        firework->setPosition(fire_pos);

        this->tilemap->addChild(firework);
        return true;
    }

    std::vector<tile_gid_t> blocked_tiles = {
        //across
        this->tile_TL_BR,
        this->tile_BL_TR,

        //corners
        this->tile_TL_TR,
        this->tile_TR_BR,
        this->tile_BL_BR,
        this->tile_TL_BL,

        //misc
        this->tile_X,
        this->tile_START
    };

    auto match = std::find(blocked_tiles.begin(), blocked_tiles.end(), tile_gid);
    if (match != blocked_tiles.end())
    {
        CCLOG("tile is a rail or other special tile");
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
            this->tilemap->runAction(shake);

            //animate specific tile
            cocos2d::Sprite* blocked_sprite = this->active_layer->getTileAt(pos);
            animate_flash_action(blocked_sprite, 0.2f, 1.1f);
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

            //set the old tile to the new image, if its not the X tile
            if (this->active_layer->getTileGIDAt(this->active_tile_pos) != this->tile_START)
            {
                this->active_layer->setTileGID(new_old_tile_id, this->active_tile_pos);
            }

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

            //animate tile moving
            cocos2d::Sprite* tile_sprite = this->active_layer->getTileAt(this->active_tile_pos);

            float move_by_offset = 200.0f;
            tile_sprite->setPositionY(tile_sprite->getPositionY()+move_by_offset);
            tile_sprite->runAction(cocos2d::Sequence::createWithTwoActions(
                cocos2d::MoveBy::create(0.1f, {0, -move_by_offset}),
                cocos2d::TargetedAction::create(this->tilemap, FShake::actionWithDuration(0.05f, 2.5f, 2.5f))
            ));

        }
    }
    else if (offset_tile == NULL)
    {
        //shake map
        auto shake = FShake::actionWithDuration(0.1f, 2.5f, 2.5f);
        this->tilemap->runAction(shake);
    };

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
