#pragma once
#ifndef MINER_H
#define MINER_H

#include "cocos2d.h"

using tile_gid_t = uint32_t;

class Miner
{
    public:
        tile_gid_t resource_tile_id = 130;

        //across
        tile_gid_t tile_TL_BR = 131;
        tile_gid_t tile_BL_TR = 69;

        //corners
        tile_gid_t tile_TL_TR = 72;
        tile_gid_t tile_TR_BR = 54;
        tile_gid_t tile_BL_BR = 36;
        tile_gid_t tile_TL_BL = 63;


    public:
        cocos2d::TMXTiledMap* tilemap;
        cocos2d::TMXLayer* active_layer;

        cocos2d::Vec2 active_tile_pos;
        cocos2d::Vec2 prev_active_tile_pos;

        Miner();


        bool resource_at_tile_pos(cocos2d::Vec2 pos);
        bool is_valid_pos(cocos2d::Vec2 pos);
        tile_gid_t get_tile_gid_at_offset(cocos2d::Vec2 pos, cocos2d::Vec2 offset);

        void move_active_tile(cocos2d::Vec2 pos);
        void move_active_up();
        void move_active_down();
        void move_active_left();
        void move_active_right();
};

#endif
