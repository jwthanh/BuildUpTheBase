#include "AppDelegate.h"
#include "HarvestScene.h"
#include "GameLogic.h"
#include "FileOperation.h"

USING_NS_CC;

/* easy logging stuff*/
#define ELPP_DISABLE_DEFAULT_CRASH_HANDLING
#include "external/easylogging.h"

INITIALIZE_EASYLOGGINGPP
/* end init of easy logging stuff*/


AppDelegate::AppDelegate() {
}

AppDelegate::~AppDelegate()
{
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

        std::function<void(float)> load_func = [this, loading_scene](float){
            CCLOG("preloading...");
            this->preload_all();
            CCLOG("...done preloading");

            Scene* scene = Scene::create();
            scene->setName("root_scene");

            HarvestScene* harvest_scene = HarvestScene::create();
            harvest_scene->setName("HarvestScene");

            scene->addChild(harvest_scene);

            auto director = Director::getInstance();
            director->replaceScene(scene);
        };
        loading_scene->scheduleOnce(load_func, 0.00f, "whateverloading");
    };

    loading_scene->setOnEnterCallback(load_cb);
    director->pushScene(loading_scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    GameLogic::save_all();

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

    auto texture_cache = Director::getInstance()->getTextureCache();
    texture_cache->addImage("items/dagger.png");
    texture_cache->addImage("items/ashen_mirror.png");
    texture_cache->addImage("items/homunculus.png");
    CCLOG("done loading sprites");

};

void AppDelegate::preload_sounds()
{

#ifdef _WIN32
    CCLOG("on windows, skipping audio preload");
#else
    CCLOG("no sounds to load anymore");
#endif

};

void AppDelegate::preload_particles()
{
};
