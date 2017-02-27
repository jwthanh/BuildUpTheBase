#include "AppDelegate.h"
#include "HarvestScene.h"
#include "FileOperation.h"

#include "magic_particles/mp_cocos.h"
#include "magic_particles/platform_cocos.h"

#include "cocos2d.h"

#include "Util.h"
#include "SoundEngine.h"

#include "ck/ck.h"
#include "ck/config.h"
USING_NS_CC;

/* easy logging stuff*/
#define ELPP_DISABLE_DEFAULT_CRASH_HANDLING
#include "external/easylogging.h"

INITIALIZE_EASYLOGGINGPP
/* end init of easy logging stuff*/


AppDelegate::AppDelegate() {
    this->device = NULL;
}

AppDelegate::~AppDelegate()
{
	if (device)
	{
		device->Destroy();
		delete device;
		device=NULL;
	}

	MP_Manager& MP=MP_Manager::GetInstance();
	MP.Destroy();

    CkShutdown();
}

void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {

    //setup logging paths
    el::Configurations conf;
    conf.setToDefault();
    std::stringstream log_config_ss;
    log_config_ss << "* GLOBAL:\n";
    log_config_ss << " FILENAME = " << FileIO::get_correct_path("game_log.log", false) << "\n";
    log_config_ss << " FORMAT = " << "%msg" << "\n";
    log_config_ss << "* INFO:\n";
    log_config_ss << " FORMAT = " << "%datetime{%H:%m} %msg\n";
    log_config_ss << "* ERROR:\n";
    log_config_ss << " FORMAT = " << "%datetime %level %msg\n";
    log_config_ss << "* WARNING:\n";
    log_config_ss << " FORMAT = " << "%datetime %level %msg\n";
    conf.parseFromText(log_config_ss.str());
    el::Loggers::reconfigureLogger("default", conf);

    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview) {
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

    //800p
    // set_resolution(800, 480);

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

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Vec2 center_pos = Vec2(
        origin.x + visibleSize.width / 2.0f,
        origin.y + visibleSize.height / 2.0f
    );

    Scene* loading_scene = Scene::create();
    Sprite* loading_bg = Sprite::create("loading/loading.png");
    loading_scene->addChild(loading_bg);

    loading_bg->setPosition(center_pos);
    loading_bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    loading_bg->getTexture()->setAliasTexParameters();
    loading_bg->setScale(16.0f);


    auto load_cb = [this, loading_scene]()
    {

        // SoundEngine::play_sound("sounds/secret5.ogg");
        std::function<void(float)> load_func = [this, loading_scene](float){
            CCLOG("preloading...");
            this->preload_all();
            CCLOG("...done preloading");

            Scene* scene = Scene::create();
            scene->setName("root_scene");

            Sprite* loading_bg = Sprite::create("loading/loading.png");
            loading_bg->setName("loading_bg");
            scene->addChild(loading_bg);

            HarvestScene* harvest_scene = HarvestScene::create();

            loading_bg->setPosition(get_center_pos());
            loading_bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            loading_bg->getTexture()->setAliasTexParameters();
            loading_bg->setScale(16.0f);

            harvest_scene->setName("HarvestScene");

            scene->addChild(harvest_scene);

            auto director = Director::getInstance();
            director->replaceScene(scene);
        };
        loading_scene->scheduleOnce(load_func, 0.00f, "whateverloading");
    };

    loading_scene->setOnEnterCallback(load_cb);
    director->pushScene(loading_scene);

    this->init_magic_particles();

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // GameLogic::save_all(); //causes crash on android when saving from city or items scene. Dont know why

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
    this->preload_sounds(); //TODO enable this once audio gets put in
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

    for (auto& path : sheet_paths) {
        CCLOG("  loading %s", path);
        cache->addSpriteFramesWithFile(path);
    };

    auto texture_cache = Director::getInstance()->getTextureCache();
    texture_cache->addImage("items/dagger.png");
    texture_cache->addImage("items/ashen_mirror.png");
    texture_cache->addImage("items/homunculus.png");
    CCLOG("done loading sprites");

};

void AppDelegate::preload_sounds()
{
#ifdef _WIN32
    CkConfig config;
    CkInit(&config);
    CCLOG("on windows, skipping audio preload");
#else
    // CCLOG("no sounds to load anymore");
    /// proj.android/jni/hellocpp/main.cpp is where ck is being initialized
#endif

    SoundLibrary::getInstance()->fill_sound_cache();


};

void AppDelegate::preload_particles()
{
};

void AppDelegate::init_magic_particles()
{
    cocos2d::Size winSize = Director::getInstance()->getWinSizeInPixels();

    int client_wi=winSize.width;
    int client_he=winSize.height;

    device=new MP_Device_Cocos(client_wi, client_he);
    device->Create();

    MP_Manager& MP=MP_Manager::GetInstance();

    MP_Platform* platform=new MP_Platform_COCOS;
#ifdef MAGIC_3D
    MAGIC_AXIS_ENUM axis=MAGIC_pXpYnZ;
#else
    MAGIC_AXIS_ENUM axis=MAGIC_pXnY;
#endif

#ifdef SHADER_WRAP
    bool filters[MAGIC_RENDER_STATE__MAX];
    for (int i=0;i<MAGIC_RENDER_STATE__MAX;i++)
        filters[i]=false;
    filters[MAGIC_RENDER_STATE_BLENDING]=true;
    filters[MAGIC_RENDER_STATE_TEXTURE]=true;
    filters[MAGIC_RENDER_STATE_ADDRESS_U]=true;
    filters[MAGIC_RENDER_STATE_ADDRESS_V]=true;
    filters[MAGIC_RENDER_STATE_ZENABLE]=true;
    filters[MAGIC_RENDER_STATE_ZWRITE]=true;
#ifndef SHADER_ALPHATEST_WRAP
    filters[MAGIC_RENDER_STATE_ALPHATEST_INIT]=true;
    filters[MAGIC_RENDER_STATE_ALPHATEST]=true;
#endif
#else
    bool* filters=NULL;
#endif

    MP.Initialization(filters, true, axis, platform, MAGIC_INTERPOLATION_ENABLE, MAGIC_CHANGE_EMITTER_DEFAULT, 1024, 1024, 1, 1.f, 0.1f, true);

    MP.LoadAllEmitters();

    MP.RefreshAtlas();

    //MP.CloseFiles(); //FIXME figure out why there wasnt any apparent MP.OpenFiles happening. commenting out this line stops a heap error (maybe because its not trying to close an unopened file? not sure)

    MP.Stop();

#ifndef MAGIC_3D
    // eng: locate emitters the same as editor
    // rus: расставляем эмиттеры также, как они стояли в редакторе
    HM_EMITTER hmEmitter=MP.GetFirstEmitter();
    while (hmEmitter)
    {
        Magic_CorrectEmitterPosition(hmEmitter, client_wi, client_he);
        hmEmitter=MP.GetNextEmitter(hmEmitter);
    }
#endif
};
