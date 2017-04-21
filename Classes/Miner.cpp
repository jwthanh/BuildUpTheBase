#include "Miner.h"
#include <array>

#include "FShake.h"
#include "MiscUI.h"

#include "Serializer.h"
#include "Util.h"

#include "2d/CCTMXTiledMap.h"
#include "2d/CCTMXLayer.h"
#include "2d/CCActionInstant.h"
#include "2d/CCSprite.h"
#include "2d/CCParticleExamples.h"
#include "goals/Achievement.h"
#include "SoundEngine.h"


enum class Directions {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

const std::array<Directions, 4> DIRECTION_ARRAY {
    Directions::TopLeft,
    Directions::TopRight,
    Directions::BottomLeft,
    Directions::BottomRight
};

std::map<cocos2d::Vec2, Directions> DIRECTION_MAP {
    {{-1, 0}, Directions::TopLeft},
    {{0, -1}, Directions::TopRight},
    {{0, 1}, Directions::BottomLeft},
    {{1, 0}, Directions::BottomRight},
};

std::map<Directions, cocos2d::Vec2> DIRECTION_MAP_REV {
    {Directions::TopLeft, {-1, 0}},
    {Directions::TopRight, {0, -1}},
    {Directions::BottomLeft, {0, 1}},
    {Directions::BottomRight, {1, 0}}
};

tile_gid_t Miner::resource_tile_id = 10;
tile_gid_t Miner::chance_tile_id = 11;

//special tiles
tile_gid_t Miner::tile_X = 9;
tile_gid_t Miner::tile_START = 4;

//across
tile_gid_t Miner::tile_TL_BR = 6;
tile_gid_t Miner::tile_BL_TR = 3;

//corners
tile_gid_t Miner::tile_TL_TR = 7;
tile_gid_t Miner::tile_TR_BR = 8;
tile_gid_t Miner::tile_BL_BR = 2;
tile_gid_t Miner::tile_TL_BL = 5;

std::vector<tile_gid_t> RAIL_IDS = {
    //across
    Miner::tile_TL_BR,
    Miner::tile_BL_TR,

    //corners
    Miner::tile_TL_TR,
    Miner::tile_TR_BR,
    Miner::tile_BL_BR,
    Miner::tile_TL_BL,

    //misc
    Miner::tile_X,
};


Miner::Miner(cocos2d::Node* parent)
{
    this->depth = 0.0;

    this->init(true); //use_existing = true

    this->parent = parent;
    this->parent->addChild(this->tilemap);

}

void Miner::animate_falling_tiles()
{
    SoundEngine::play_sound("sounds/explosion2.ogg");

    ///randomly pick a starting position in the map
    cocos2d::Size layer_size = this->active_layer->getLayerSize();
    cocos2d::Vec2 start_pos = { -1, -1 };
    tile_gid_t num_tiles = static_cast<tile_gid_t>(layer_size.width * layer_size.height);

    float duration = 0.1f;
    float max_delay = 0.5f;

    std::vector<cocos2d::FiniteTimeAction*> land_actions;

    std::mt19937 float_gen = std::mt19937(std::random_device{}());
    std::uniform_real_distribution<float> float_distribution(0, max_delay);

    for (tile_gid_t y = 0; y < layer_size.height; y++)
    {
        for (tile_gid_t x = 0; x < layer_size.width; x++)
        {
                float fx = float(x);
                float fy = float(y);
                cocos2d::Vec2 looped_pos = { fx, fy };

                cocos2d::Sprite* tile_sprite = this->active_layer->getTileAt(looped_pos);

                float move_by_offset = 25.0f;
                tile_sprite->setPositionY(tile_sprite->getPositionY()+move_by_offset);
                tile_sprite->setVisible(false);

                float delay = float_distribution(float_gen);
                auto tile_action = cocos2d::TargetedAction::create(tile_sprite,
                    cocos2d::Sequence::create(
                    cocos2d::DelayTime::create(delay),
                    cocos2d::Show::create(),
                    cocos2d::MoveBy::create(duration, { 0, -move_by_offset }),
                    NULL
                    )
                    );
                land_actions.push_back(cocos2d::Sequence::createWithTwoActions(
                    tile_action,
                    FShake::actionWithDuration(0.1f, 1.5f, 1.5f)
                    ));

        }
    };

    cocos2d::Vector<cocos2d::FiniteTimeAction*> action_array = cocos2d::Vector<cocos2d::FiniteTimeAction*>();
    std::shuffle(land_actions.begin(), land_actions.end(), std::random_device());

    for (auto& action : land_actions)
    {
        action_array.pushBack(action);
    }
    cocos2d::Spawn* seq = cocos2d::Spawn::create(action_array);
    this->tilemap->runAction(seq);

};

cocos2d::Vec2 Miner::generate_free_tile_pos()
{
	return this->generate_free_tile_pos({});
};

cocos2d::Vec2 Miner::generate_free_tile_pos(std::vector<cocos2d::Vec2> reserved_tiles)
{
    cocos2d::Size layer_size = this->active_layer->getLayerSize();
    cocos2d::Vec2 start_pos = { -1, -1 };
    tile_gid_t num_tiles = static_cast<tile_gid_t>(layer_size.width * layer_size.height);

    std::mt19937 gen = std::mt19937(std::random_device{}());
    //guessing offsets so it cant be the last tile or the first tile TODO make
    //sure the prev tile is valid
    std::uniform_int_distribution<tile_gid_t> distribution(1, num_tiles - 2);
    tile_gid_t start_id = distribution(gen);

    float duration = 0.1f;
    float max_delay = 0.5f;


    tile_gid_t counter = 0;
    for (tile_gid_t y = 0; y < layer_size.height; y++)
    {
        for (tile_gid_t x = 0; x < layer_size.width; x++)
        {
            float fx = float(x);
            float fy = float(y);
            cocos2d::Vec2 looped_pos = { fx, fy };

            counter++;
            if (start_id == counter)
            {

				//skip tile pos if its in the reserved tiles
				if (std::find(reserved_tiles.begin(), reserved_tiles.end(), looped_pos) != reserved_tiles.end())
                {
                    start_id++;
                }
                else
                {
                    start_pos = looped_pos;
                }
            };

        }
    };

	return start_pos;
};

cocos2d::Vec2 Miner::get_default_start_pos()
{
    //randomly pick a starting position in the map that isnt the resource tile
	cocos2d::Vec2 start_pos = this->generate_free_tile_pos({this->altar_tile_pos});
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
    this->tilemap = cocos2d::TMXTiledMap::create("tilemaps/low_map.tmx");
    this->tilemap->setScale(3.0f);
    this->tilemap->setPositionY(this->tilemap->getPositionY()-25.0f);

    this->active_layer = this->tilemap->getLayer("background");

    cocos2d::Vec2 start_pos;

    MinerSerializer serializer = MinerSerializer("alpha_tilemap.json", this);
    //if we want to use existing data, try to load it, otherwise fallback to default loading
    if (use_existing)
    {
        serializer.load();
        if (serializer.existing_json_found)
        {
            // start_pos = this->active_tile_pos; //serializer sets it
        }
        else
        {
            use_existing = false;
        }
    }

    if (use_existing == false)
    {
        start_pos = this->get_default_start_pos();
        this->active_layer->setTileGID(this->tile_START, start_pos);

        this->altar_tile_pos = this->generate_free_tile_pos({});
        this->active_layer->setTileGID(this->resource_tile_id, this->altar_tile_pos);

        this->init_start_pos(start_pos);
        this->prev_active_tile_pos = this->active_tile_pos - cocos2d::Vec2{-1, 0};
    }

    serializer.serialize(); //save on init, for when you go down a level and we want to save the new map

    this->animate_falling_tiles();

};

void Miner::init_start_pos(cocos2d::Vec2 new_start_pos)
{
    //since we subtract 1 from it later, we need to make sure it's never out of bounds
    // I dont think y will be problem
    if (new_start_pos.x == 0)
    {
        new_start_pos.x += 1;
    }

    this->active_tile_pos = new_start_pos;
};

void Miner::reset()
{
    auto achievement_manager = AchievementManager::getInstance();
    std::shared_ptr<BaseAchievement> achievement = achievement_manager->getAchievement(AchievementType::TotalDepth);
    achievement->increment();

    this->depth += 1.0;

    // deletes the tilemap and rebuilds it from scratch
    if (this->tilemap != NULL)
    {
        this->tilemap->removeFromParent();
    }

    //Hack to get around active_tile_pos sometimes being altar_tile_pos
    do {
        this->init(false); //use_existing = false
    } while (this->active_tile_pos == this->altar_tile_pos);

    this->parent->addChild(this->tilemap);
};

bool Miner::get_tile_is_blocked_pos(cocos2d::Vec2 pos)
{

    if (this->is_valid_pos(pos) == false) { return true; };

    auto tile_gid = this->active_layer->getTileGIDAt(pos);
    auto tile_props = this->tilemap->getPropertiesForGID(tile_gid);

    if (tile_gid == this->resource_tile_id)
    {
        CCLOG("tile is a resource");
        bool run_fireworks = false; //renable some other time
        if (run_fireworks)
        {
            auto firework = cocos2d::ParticleFireworks::create();
            firework->setDuration(1.0f);

            auto sprite = this->active_layer->getTileAt(pos);
            auto fire_pos = sprite->getPosition();
            fire_pos.x += 132.0f/2;
            fire_pos.y += 132.0f/2;
            firework->setPosition(fire_pos);

            this->tilemap->addChild(firework);
        };
        return true;
    }

    //TODO use RAIL_IDS + tile_START for this
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
    std::map<tile_gid_t, std::vector<std::array<Directions, 2>>> tile_direction_map{
        //crosses
        {
            this->tile_BL_TR, {
                { Directions::BottomLeft, Directions::TopRight},
                { Directions::TopRight, Directions::BottomLeft}
            }
        },

        {
            this->tile_TL_BR, {
                { Directions::TopLeft, Directions::BottomRight},
                { Directions::BottomRight, Directions::TopLeft}
            }
        },

        //corners
        { this->tile_TL_TR, {
                { Directions::TopLeft, Directions::TopRight},
                { Directions::TopRight, Directions::TopLeft}
            }
        },

        { this->tile_TR_BR, {
                { Directions::TopRight, Directions::BottomRight},
                { Directions::BottomRight, Directions::TopRight}
            }
        },

        { this->tile_BL_BR, {
                { Directions::BottomLeft, Directions::BottomRight},
                { Directions::BottomRight, Directions::BottomLeft}
            }
        },

        { this->tile_TL_BL, {
                { Directions::TopLeft, Directions::BottomLeft},
                { Directions::BottomLeft, Directions::TopLeft}
            }
        },
    };

    //which directions connect across tiles
    std::map<Directions, Directions> connection_map {
        {Directions::TopLeft, Directions::BottomRight},
        {Directions::BottomRight, Directions::TopLeft},
        {Directions::TopRight, Directions::BottomLeft},
        {Directions::BottomLeft, Directions::TopRight},
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
            Directions last_connection = DIRECTION_MAP[difference];

            //based on the new tile and the last connection, figure out how the old active
            // tile should look
            Directions new_tile_dir = DIRECTION_MAP.at(offset);
            std::array<Directions, 2> old_tile_new_dir = {last_connection, new_tile_dir};

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
            if (new_tile_dir == Directions::BottomLeft || new_tile_dir == Directions::TopRight)
            {
                this->active_layer->setTileGID(this->tile_BL_TR, this->active_tile_pos);
            }
            else
            {
                this->active_layer->setTileGID(this->tile_TL_BR, this->active_tile_pos);
            }

            //animate tile moving
            cocos2d::Sprite* tile_sprite = this->active_layer->getTileAt(this->active_tile_pos);

            float move_by_offset = 25.0f;
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

bool Miner::rails_connect_a_resource(cocos2d::Vec2 the_resource_tile_pos)
{
    auto check_direction_for_rail = [this, the_resource_tile_pos](cocos2d::Vec2 offset){
        cocos2d::Vec2 potential_rail_pos = the_resource_tile_pos+offset;
        if (this->is_valid_pos(potential_rail_pos))
        {
            tile_gid_t potential_rail_id = this->active_layer->getTileGIDAt(potential_rail_pos);
            CCLOG("tid: %i", potential_rail_id);

            return std::find(RAIL_IDS.begin(), RAIL_IDS.end(), potential_rail_id) != RAIL_IDS.end();
        }
        else
        {
            return false;
        }
    };

    for (auto& dir : DIRECTION_ARRAY)
    {
        bool found_rail = check_direction_for_rail(DIRECTION_MAP_REV.at(dir));
        if (found_rail)
        {
            CCLOG("Found rail");
            return true;
        }
    };
    CCLOG("Could not find rail");
    return false;
}
