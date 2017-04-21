#pragma once
#ifndef MINER_H
#define MINER_H

#include <vector>

#include "math/Vec2.h"

using tile_gid_t = uint32_t;
using res_count_t = long double;


namespace cocos2d
{
    class Node;

    class TMXTiledMap;
    class TMXLayer;
}

struct TileID
{
    //resources
    static const tile_gid_t altar_tile;
    static const tile_gid_t chance_tile;

    //special tiles
    static const tile_gid_t tile_X;
    static const tile_gid_t tile_START;

    //across
    static const tile_gid_t tile_TL_BR;
    static const tile_gid_t tile_BL_TR;

    //corners
    static const tile_gid_t tile_TL_TR;
    static const tile_gid_t tile_TR_BR;
    static const tile_gid_t tile_BL_BR;
    static const tile_gid_t tile_TL_BL;
};

class Miner
{
    public:
        res_count_t depth;

    public:
        cocos2d::TMXTiledMap* tilemap;
        cocos2d::TMXLayer* active_layer;

        cocos2d::Vec2 active_tile_pos;
        cocos2d::Vec2 prev_active_tile_pos;
        cocos2d::Vec2 altar_tile_pos;

        cocos2d::Node* parent;
        Miner(cocos2d::Node* parent);
        void init(bool use_existing);
        void init_active_tile_pos();
        void reset();

		cocos2d::Vec2 generate_free_tile_pos();
		cocos2d::Vec2 generate_free_tile_pos(std::vector<cocos2d::Vec2> reserved_tiles);

        cocos2d::Vec2 get_existing_start_pos();

        void animate_falling_tiles();

        bool get_tile_is_blocked_pos(cocos2d::Vec2 pos);
        bool is_valid_pos(cocos2d::Vec2 pos);
        tile_gid_t get_tile_gid_at_offset(cocos2d::Vec2 pos, cocos2d::Vec2 offset);

        void move_active_tile(cocos2d::Vec2 pos);
        void move_active_top_right();
        void move_active_bottom_left();
        void move_active_top_left();
        void move_active_bottom_right();

        //check if tiles around the resource tiles are rails
        // TODO: use a param for the tile instead of looking for it
        bool rails_connect_a_resource(cocos2d::Vec2 resource_tile_pos);

        std::string get_pretty_printed_tiles();
};

#endif
