#include "Level.h"


#include "Beatup.h"
#include "Clock.h"
#include "Quest.h"
#include "DataManager.h"

const std::string Level::generic_id_key = "last_level";


Level::Level(int order) : order(order)
{

};

Level* Level::create_level(int level_num)
{
    CCLOG("creating level %d", level_num);
    Level* level_obj = new Level(level_num);

    struct LevelDetail {
        FaceDetail face_detail;
        PlayerDetail player_detail;
        SceneDetail scene_detail;
    };

    std::vector<LevelDetail> level_details = {
        { //1
            { 100, 15.0f, {  } },
            { 10 },
            { "Chad", BackgroundTypes::Forest }
        },
        { //2
            { 120, 15.0f, {  } },
            { 10 },
            { "Chester", BackgroundTypes::Forest }
        },
        { //3
            { 140, 15.0f, {  } },
            { 10 },
            { "Chuck", BackgroundTypes::Forest }
        },
        { //4
            { 150, 15.0f, { Face::Beard } },
            { 10 },
            { "Charming Charley", BackgroundTypes::Forest }
        },
        { //5
            { 200, 11.0f, { Face::Glasses } },
            { 20 },
            { "Brad", BackgroundTypes::Volcano }
        },
        { //6
            { 250, 11.0f, { Face::Glasses } },
            { 20 },
            { "Bozley", BackgroundTypes::Volcano }
        },
        { //7
            { 270, 11.0f, { Face::Glasses, Face::Beard } },
            { 20 },
            { "Badger", BackgroundTypes::Volcano }
        },
        { //8
            { 300, 11.0f, { Face::Glasses } },
            { 20 },
            { "Bespectacled Beast", BackgroundTypes::Volcano }
        },
        { //9
            { 350, 6.0f, { Face::Moustache, Face::Hat } },
            { 25 },
            { "Micheal", BackgroundTypes::Western }
        },
        { //10
            { 400, 6.0f, { Face::Moustache, Face::Hat } },
            { 25 },
            { "Moe", BackgroundTypes::Western }
        },
        { //11
            { 460, 6.0f, { Face::Glasses, Face::Hat } },
            { 25 },
            { "Mack", BackgroundTypes::Western }
        },
        { //12
            { 500, 6.0f, { Face::Moustache, Face::Hat, Face::Monocle } },
            { 25 },
            { "Monstrous Matt", BackgroundTypes::Western }
        },
        { //13
            { 550, 5.0f, { Face::Beard, Face::Hat } },
            { 20 },
            { "Josh", BackgroundTypes::Volcano }
        },
        { //14
            { 650, 5.0f, { Face::Beard } },
            { 20 },
            { "Jim", BackgroundTypes::Volcano }
        },
        { //15
            { 700, 5.0f, { Face::Beard } },
            { 20 },
            { "Joseph", BackgroundTypes::Volcano }
        },
        { //16
            { 750, 5.0f, { Face::Beard, Face::Moustache } },
            { 20 },
            { "Jolly Jester", BackgroundTypes::Volcano }
        },
        { //17
            { 800, 3.0f, { Face::Monocle } },
            { 10 },
            { "Tom", BackgroundTypes::Forest }
        },
        { //18
            { 850, 3.0f, { Face::Monocle, Face::Beard } },
            { 10 },
            { "Tim", BackgroundTypes::Forest }
        },
        { //19
            { 900, 3.0f, { Face::Monocle } },
            { 10 },
            { "Trevor", BackgroundTypes::Forest }
        },
        { //20
            { 1000, 3.0f, { Face::Monocle } },
            { 10 },
            { "Terrifying Terry", BackgroundTypes::Forest }
        },
    };

    LevelDetail level_detail;
    try {
        level_detail = level_details.at(level_num-1);
    }
    catch (const std::out_of_range&) {
        CCLOG("unknown level_num %d", level_num);
        delete level_obj;
        return NULL;
        //assert(false && "unknown level_num");
    };

    level_obj->face_detail = level_detail.face_detail;
    level_obj->player_detail = level_detail.player_detail;
    level_obj->scene_detail = level_detail.scene_detail;

    return level_obj;

};

void Level::setup_face(Face& face)
{
    face.hits_to_kill = this->face_detail.hp;
    face.defaults.last_attack_threshold = this->face_detail.time_between_attacks;
    face.last_attack_clock->set_threshold(face.defaults.last_attack_threshold);

    for (auto feat_t : this->face_detail.features)
    {
        face.add_feature(feat_t);
    };
};

void Level::setup_player(Beatup& beatup)
{
    beatup.player_total_hp = this->player_detail.hp;
    beatup.player_hp = beatup.player_total_hp;
};

void Level::setup_scene(Beatup& beatup)
{
    beatup.set_background(this->scene_detail.background);
    beatup.title_lbl->setString(this->scene_detail.name);

    beatup.level = this;

    beatup.level_been_won = false;
    beatup.reset_hits();
};

void Level::setup_quest(Beatup& beatup)
{
    beatup.quest = Quest::create_quest(&beatup, this->order);
    beatup.quest_completed = beatup.quest->get_is_satisfied();
};

int Level::get_generic_complete()
{
    int last_level = DataManager::get_int_from_data(Level::generic_id_key);
    return last_level;
};

void Level::set_generic_complete(int val)
{
    DataManager::set_int_from_data(Level::generic_id_key, val);
};

void Level::mark_complete()
{
    //don't set the last level if the existing one is after this
    int last_level = Level::get_generic_complete();

    if (last_level >= this->order)
    {
        return;
    };

    Level::set_generic_complete(this->order);
};
