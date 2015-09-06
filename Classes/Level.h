#pragma once
#ifndef LEVEL_H
#define LEVEL_H

#include "Face.h"

#include "constants.h"

class Quest;
class Beatup;

struct FaceDetail
{
    int hp; //hits_to_kill
    float time_between_attacks; //last_attack_threshold

    std::vector<Face::FeatureType> features;
};

struct PlayerDetail
{
    int hp; //player_total_hp
};

struct SceneDetail
{
    std::string name;
    BackgroundTypes background;
};

class Level
{
    public:
        static const std::string generic_id_key;

        FaceDetail face_detail;
        PlayerDetail player_detail;
        SceneDetail scene_detail;

        int order;

        Level(int order);
        static Level* create_level(int level_num);

        void setup_face(Face& face);
        void setup_player(Beatup& beatup);
        void setup_quest(Beatup& beatup);
        void setup_scene(Beatup& beatup);

        void mark_complete();
        static int get_generic_complete();
        static void set_generic_complete(int val);
};

#endif
