#pragma once
#ifndef SOUNDENGINE_H
#define SOUNDENGINE_H

#include "cocos2d.h"

class SoundEngine
{
    private:
        static bool _sound_enabled;
        static float _sound_vol;
        static float _music_vol;

    public:
        static std::string id_string;
        static std::string music_id_string;

        static int bg_music_id;

        static bool is_sound_enabled();
        static void set_sound_enabled(bool enabled);

        static bool is_music_enabled();
        static void set_music_enabled(bool enabled);


        static float get_sound_vol();
        static float get_music_vol();

        static bool get_playing_music();
        static void play_music(std::string music_path);

        static int play_sound(std::string sound_path);

        static void stop_all_sound_and_music();

};

#endif
