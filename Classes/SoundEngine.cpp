#include "SoundEngine.h"
//#undef _WIN32

#ifdef _WIN32
#include "AudioEngine.h" //from cocos
#else
#include "SimpleAudioEngine.h"
#endif
#include "constants.h"
#include "base/CCUserDefault.h"


USING_NS_CC;


bool SoundEngine::_sound_enabled = true;
float SoundEngine::_sound_vol = 1.0f;
float SoundEngine::_music_vol = 1.0f;
std::string SoundEngine::id_string = "sound_enabled";
std::string SoundEngine::music_id_string = "music_enabled";
int SoundEngine::bg_music_id = -1;

bool SoundEngine::is_sound_enabled()
{
    UserDefault* ud = UserDefault::getInstance();
    return ud->getBoolForKey(SoundEngine::id_string.c_str(), true);
};

void SoundEngine::set_sound_enabled(bool enabled)
{
    // SoundEngine::_sound_enabled = enabled;
    UserDefault* ud = UserDefault::getInstance();
    return ud->setBoolForKey(SoundEngine::id_string.c_str(), enabled);
};

bool SoundEngine::is_music_enabled()
{
    UserDefault* ud = UserDefault::getInstance();
    return ud->getBoolForKey(SoundEngine::music_id_string.c_str(), true);
};

void SoundEngine::set_music_enabled(bool enabled)
{
    if (!enabled) {
        SoundEngine::stop_all_sound_and_music();
    };

    UserDefault* ud = UserDefault::getInstance();
    return ud->setBoolForKey(SoundEngine::music_id_string.c_str(), enabled);
};

float SoundEngine::get_sound_vol()
{
    return SoundEngine::_sound_vol;
};

float SoundEngine::get_music_vol()
{
    return SoundEngine::_music_vol;
};

int SoundEngine::play_sound(std::string sound_path)
{
    if (SoundEngine::is_sound_enabled())
    {
        sound_path = clean_path(sound_path);
#ifdef _WIN32
        return experimental::AudioEngine::play2d(sound_path.c_str());
#else
		auto eng = CocosDenshion::SimpleAudioEngine::getInstance();
		eng->playEffect(sound_path.c_str());
#endif

    };
    return -1;
};

void SoundEngine::play_music(std::string music_path)
{
    if (SoundEngine::is_music_enabled())
    {
        music_path = clean_path(music_path);
#ifdef _WIN32
        if (SoundEngine::bg_music_id != -1)
        {
            experimental::AudioEngine::stop(SoundEngine::bg_music_id);
        }
        SoundEngine::bg_music_id =  experimental::AudioEngine::play2d(music_path.c_str(), true);
#else
		auto eng = CocosDenshion::SimpleAudioEngine::getInstance();
		eng->playBackgroundMusic(music_path.c_str());
#endif
    };
};

bool SoundEngine::get_playing_music()
{

#ifdef _WIN32
    if (SoundEngine::bg_music_id != -1)
    {
        float duration = experimental::AudioEngine::getDuration(SoundEngine::bg_music_id);
        if (experimental::AudioEngine::getCurrentTime(SoundEngine::bg_music_id) >= duration)
        {
            return false;
        }
    }
    else
    {
        return false;
    };

    return true;
#else
    auto eng = CocosDenshion::SimpleAudioEngine::getInstance();
    return eng->isBackgroundMusicPlaying();
#endif
}

void SoundEngine::stop_all_sound_and_music()
{
#ifdef _WIN32
    experimental::AudioEngine::stopAll();
    SoundEngine::bg_music_id = -1;

#else
    auto eng = CocosDenshion::SimpleAudioEngine::getInstance();
    eng->stopAllEffects();
    eng->stopBackgroundMusic();
#endif

};
