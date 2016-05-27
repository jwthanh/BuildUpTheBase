#include "AppDelegate.h"
#include "HarvestScene.h"

USING_NS_CC;

AppDelegate::AppDelegate() {
}

AppDelegate::~AppDelegate() 
{
}

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
            scene->setName("root_scene");
            HarvestScene* harvest_scene = HarvestScene::create();
            harvest_scene->setName("HarvestScene");

            scene->addChild(harvest_scene);

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
    CCLOG("no sounds to load anymore");
#endif

};

void AppDelegate::preload_particles()
{
};
