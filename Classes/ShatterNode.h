#ifndef HelloWorld_ensShatterNode_h
#define HelloWorld_ensShatterNode_h
#include "2d/CCSprite.h"
#include "2d/CCActionInterval.h"

using namespace cocos2d;

class ShatterFrag : public Sprite
{
    public:
        int random_number;
        ShatterFrag() { random_number = -1; }
};

class ShatterAction;

class ShatterSprite : public Sprite
{
    public:
        friend class ShatterAction;

        ShatterSprite(){
            grid_side_len = 2.7f;
            initial_frag_scale = 2.0f;
            frag_batch_node = NULL;
        }
        virtual ~ShatterSprite(){}
        bool init(const std::string&texFileName);
        static ShatterSprite* create(const std::string& filename);
        bool initWithFile(const std::string& filename);
        bool initWithSpriteFrameName(const std::string& spriteFrameName);
        bool initWithSpriteFrame(SpriteFrame *spriteFrame);
        static ShatterSprite* createWithSpriteFrame(SpriteFrame *spriteFrame);
        static ShatterSprite* createWithSpriteFrameName(SpriteFrame *spriteFrame);
        static ShatterSprite* createWithSpriteFrameName(const std::string& spriteFrameName);
        static ShatterSprite* createWithTexture(Texture2D *texture);
        static ShatterSprite* createWithTexture(Texture2D *texture, const Rect& rect, bool rotated);

    protected:
        void createShatter();
        void resetShatter();
        void updateShatterAction(float time, float dt, float growSpeedOfTargetR);

        float grid_side_len;//grid side length, also frag size
        float initial_frag_scale;//we want to make frags a litte bigger at start time.
        std::vector<std::vector<ShatterFrag*> > frag_grid;//hold all the frags
        SpriteBatchNode* frag_batch_node;//all frags add to this batchNode
};

class ShatterAction : public ActionInterval
{
public:
    //creates the action
    static ShatterAction* create(float duration);
    //initializes the action
    bool initWithDuration(float duration);

    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);
    virtual void stop();
    virtual bool isDone();

    ShatterAction(){ initMembers(); }
    virtual~ShatterAction(){ }

protected:
    void initMembers(){
        time_foe = 0;
        cur_time = 0;

        m_growSpeedOfTargetR = 50;
    }

    float time_foe;
    float cur_time;

    float m_growSpeedOfTargetR;

};
#endif
