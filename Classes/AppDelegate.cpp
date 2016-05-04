#include "AppDelegate.h"
#include "HarvestScene.h"

USING_NS_CC;

AppDelegate::AppDelegate() {
}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("Build up a base!");
        director->setOpenGLView(glview);
    }

    Size base_size = Size(960, 640);

    auto set_resolution = [glview, director, base_size](float x, float y) {
        glview->setFrameSize(x, y);
        glview->setDesignResolutionSize(base_size.width, base_size.height, ResolutionPolicy::EXACT_FIT);
    };

    //iphone
    // set_resolution(960, 640);

    //720p
    // set_resolution(1280, 720);

    //1080p
    // set_resolution(1920, 1080);

    //1024 x 600
    // set_resolution(1024, 600);

    // turn on display FPS
    // director->setDisplayStats(true);
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval((float)(1.0 / 60));

    // no matter the screen size, have it act like a base size screen, 960x640
    glview->setDesignResolutionSize(base_size.width, base_size.height, ResolutionPolicy::EXACT_FIT);

    // glfwWindowHint(GLFW_SAMPLES, 0); //supposed to disable anti aliasing but didnt

#ifdef __ANDROID__
#else
    system("xcopy \"C:\\Users\\Primary\\workspace\\basebuilder\\proj.win32\\..\\Resources\" \"C:\\Users\\Primary\\workspace\\basebuilder\\proj.win32\\Debug.win32\" /E /I /F /Y  /D /K ");
#endif

    //add the cocos studio paths. it ignores its own export path so you need to tell it where else to look
    FileUtils::getInstance()->addSearchPath("editor");

    //seed rng
    std::srand((int)time(NULL));

    // auto console = director->getConsole();
    // console->listenOnTCP(1234);

    Scene* loading_scene = Scene::create();
    loading_scene->setOnEnterCallback(
        std::function<void()>([this, loading_scene](){
        auto lbl = Label::createWithTTF("LOADING...", TITLE_FONT, 24);
        lbl->runAction(Spawn::createWithTwoActions(
            JumpBy::create(2.0f, Vec2(200, 300), 200, 10),
            ScaleTo::create(2.0f, 10)
            ));
        loading_scene->setPosition(300, 300);
        loading_scene->addChild(lbl);

        std::function<void(float)> load_func = [this, loading_scene](float){
            CCLOG("preloading");
            this->preload_all();
            CCLOG("done preloading");

            Scene* scene = Scene::create();

            scene->addChild(HarvestScene::create());

            auto director = Director::getInstance();
            director->pushScene(scene);
        };
        loading_scene->scheduleOnce(load_func, 0.00f, "whateverloading");
    })
        );
    director->runWithScene(loading_scene);


    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

void AppDelegate::preload_all()
{
    this->preload_sprites();
    // this->preload_sounds(); //TODO enable this once audio gets put in
    this->preload_particles();
};

void AppDelegate::preload_sprites()
{
    auto cache = SpriteFrameCache::getInstance();

    CCLOG("start loading sprites");

    auto sheet_paths = {
        "master.plist",
        "master_ui.plist",
        "kenney_iso_buildings.plist",
        "master_harvesters_50.plist"
    };

    for (auto path : sheet_paths) {
        cache->addSpriteFramesWithFile(path);
    };
    CCLOG("done loading sprites");

};

void AppDelegate::preload_sounds()
{

#ifdef _WIN32
    CCLOG("on windows, skipping audio preload");
#else
    CocosDenshion::SimpleAudioEngine* cache = CocosDenshion::SimpleAudioEngine::getInstance();
    CCLOG("start loading audio");
    cache->preloadEffect(clean_path("sounds\\old\\Swoosh.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\barrel.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\beep.mp3").c_str());
    // cache->preloadEffect(clean_path("sounds\\old\\block.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\brick_hit.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\brick_scrape.mp3").c_str());
    // cache->preloadEffect(clean_path("sounds\\old\\cheering.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\coin.mp3").c_str());
    //cache->preloadEffect(clean_path("sounds\\old\\ding.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\doom_rocket.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\dsnoway.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\glassbreak.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\ice_freeze.mp3").c_str());
    //cache->preloadEffect(clean_path("sounds\\old\\lighter.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\radio_wave.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\real_punch.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\shield_die.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\shield_disable.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\shield_hit.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\shotgun.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\shotgun_reload.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\suicide_bomber.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\wall_destroy.mp3").c_str());

    cache->preloadEffect(clean_path("sounds\\new\\barrel\\C_barrel_1.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\block\\C_block+1.mp3").c_str());
    // cache->preloadEffect(clean_path("sounds\\new\\block\\C_block+2.mp3").c_str());
    // cache->preloadEffect(clean_path("sounds\\new\\block\\C_block_1.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\charged\\C_charged_1.mp3").c_str());
    //cache->preloadEffect(clean_path("sounds\\new\\cheering\\C_cheering_11.mp3").c_str()); //TODO Find a use for this
    //cache->preloadEffect(clean_path("sounds\\new\\cheering\\C_cheering_12.mp3").c_str()); //TODO Find a use for this
    cache->preloadEffect(clean_path("sounds\\new\\cheering\\C_cheering_13.mp3").c_str());
    //cache->preloadEffect(clean_path("sounds\\new\\coin\\C_coin_1.mp3").c_str());
    //cache->preloadEffect(clean_path("sounds\\new\\coin\\C_coin_2.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\doom_rocket\\C_grenade_1.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\doom_rocket\\C_rocket_big_1.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\doom_rocket\\C_rocket_small_1.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\dsnoway\\C_Grunt_11.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\dsnoway\\C_Grunt_12.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\dsnoway\\C_Grunt_13.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\dsnoway\\C_Grunt_14.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\dsnoway\\C_Grunt_15.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\dsnoway\\C_Grunt_16.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\dsnoway\\C_Grunt_17.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\dsnoway\\C_Grunt_18.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\dsnoway\\C_Hit_11.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\dsnoway\\C_Hit_12.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\dsnoway\\C_Hit_13.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\dsnoway\\C_Hit_14.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\dsnoway\\C_Hit_15.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\dsnoway\\C_Hit_16.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\dsnoway\\C_Hit_17.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\dsnoway\\C_Hit_18.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\extra\\C_explosion_1.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\glassbreak\\C_glassbreak_1.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\ice_freeze\\C_ice_freeze+.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\ice_freeze\\C_ice_freeze_1.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\lighter\\C_lighter_1.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\radio_wave\\C_radio_wave_1.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\radio_wave\\C_radio_wave_2.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\radio_wave\\C_radio_wave_3.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\real_punch\\C_combo_1.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\real_punch\\C_punch+1.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\real_punch\\C_punch+2.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\real_punch\\C_punch+3.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\real_punch\\C_punch_11.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\real_punch\\C_punch_21.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\real_punch\\C_punch_22.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\real_punch\\C_punch_23.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\real_punch\\C_punch_24.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\real_punch\\C_punch_25.mp3").c_str());
    // cache->preloadEffect(clean_path("sounds\\new\\real_punch\\C_punch_26.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\real_punch\\C_punch_27.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\real_punch\\C_punch_28.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\real_punch\\C_punch_31.mp3").c_str());

    cache->preloadEffect(clean_path("sounds\\new\\shotgun\\C_shotgun_1.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\shotgun_reload\\C_shotgun_reload_1.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\suicide_bomber\\C_rocket_big+1.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\suicide_bomber\\C_rocket_small+2.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\Swoosh\\C_Swoosh_1.wav").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\Swoosh\\C_Swoosh_11.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\Swoosh\\C_Swoosh_12.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\Swoosh\\C_Swoosh_13.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\Swoosh\\C_Swoosh_14.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\Swoosh\\C_Swoosh_15.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\Swoosh\\C_Swoosh_21.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\Swoosh\\C_Swoosh_22.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\Swoosh\\C_Swoosh_23.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\Swoosh\\C_Swoosh_24.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\Swoosh\\C_Swoosh_25.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\Swoosh\\C_Swoosh_26.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\Swoosh\\C_Swoosh_27.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\new\\woop\\C_woop_1.mp3").c_str());
    cache->preloadEffect(clean_path("sounds\\old\\woop.mp3").c_str());
    CCLOG("done loading audio");
#endif

};

void AppDelegate::preload_particles()
{
};
