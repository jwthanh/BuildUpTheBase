#include "ShatterNode.h"
#include "2d/CCSpriteFrameCache.h"
#include "base/CCDirector.h"
#include "2d/CCSpriteBatchNode.h"
#include "renderer/CCTextureCache.h"

bool ShatterSprite::init(const std::string&texFileName){
    this->Sprite::initWithFile(texFileName.c_str());
    createShatter();
    return true;
}


ShatterSprite* ShatterSprite::create(const std::string& filename)
{
    ShatterSprite *sprite = new (std::nothrow) ShatterSprite();
    if (sprite && sprite->initWithFile(filename))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

ShatterSprite* ShatterSprite::createWithSpriteFrame(SpriteFrame *spriteFrame)
{
    ShatterSprite *sprite = new (std::nothrow) ShatterSprite();
    if (sprite && spriteFrame && sprite->initWithSpriteFrame(spriteFrame))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

ShatterSprite* ShatterSprite::createWithSpriteFrameName(const std::string& spriteFrameName)
{
    SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);

#if COCOS2D_DEBUG > 0
    char msg[256] = { 0 };
    sprintf(msg, "Invalid spriteFrameName: %s", spriteFrameName.c_str());
    CCASSERT(frame != nullptr, msg);
#endif

    return createWithSpriteFrame(frame);
}

ShatterSprite* ShatterSprite::createWithTexture(Texture2D *texture)
{
    ShatterSprite *sprite = new (std::nothrow) ShatterSprite();
    if (sprite && sprite->initWithTexture(texture))
    {
        sprite->autorelease();
        sprite->createShatter();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

ShatterSprite* ShatterSprite::createWithTexture(Texture2D *texture, const Rect& rect, bool rotated)
{
    ShatterSprite *sprite = new (std::nothrow) ShatterSprite();
    if (sprite && sprite->initWithTexture(texture, rect, rotated))
    {
        sprite->autorelease();
        sprite->createShatter();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool ShatterSprite::initWithFile(const std::string& filename)
{
    CCASSERT(filename.size() > 0, "Invalid filename for sprite");

    Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(filename);
    if (texture)
    {
        Rect rect = Rect::ZERO;
        rect.size = texture->getContentSize();
        auto ret = initWithTexture(texture, rect);
        createShatter();
        return ret;
    }

    // don't release here.
    // when load texture failed, it's better to get a "transparent" sprite then a crashed program
    // this->release();
    return false;
}

bool ShatterSprite::initWithSpriteFrameName(const std::string& spriteFrameName)
{
    CCASSERT(spriteFrameName.size() > 0, "Invalid spriteFrameName");

    SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
    return initWithSpriteFrame(frame);
}

bool ShatterSprite::initWithSpriteFrame(SpriteFrame *spriteFrame)
{
    CCASSERT(spriteFrame != nullptr, "");

    bool bRet = initWithTexture(spriteFrame->getTexture(), spriteFrame->getRect());
    createShatter();

    setSpriteFrame(spriteFrame);

    return bRet;
}


static std::random_device rand_device;
static std::mt19937_64 rand_generator(rand_device());
static std::uniform_int_distribution<int> rand_distribution;

void ShatterSprite::createShatter(){
    //----calculate grid size and fragCount
    Size contentSize = this->getContentSize();
    const int nRow = (int)floorf(contentSize.height / grid_side_len);//grid row count
    const int nCol = (int)floorf(contentSize.width / grid_side_len);//grid col count
    const int fragCount = nRow*nCol;
    //----create fragBatchNode
    frag_batch_node = SpriteBatchNode::createWithTexture(this->getTexture(), fragCount);

    this->addChild(frag_batch_node);
    frag_batch_node->setVisible(false);

    //----create frags and add them to fragBatchNode and grid
    //make frag_grid
    frag_grid.resize(nRow);
    for (int i = 0; i < nRow; i++){
        frag_grid[i].resize(nCol);
    }

    for (int i = 0; i < nRow; i++){
        for (int j = 0; j < nCol; j++){
            ShatterFrag* frag = new ShatterFrag();
            frag->autorelease();
            frag->initWithTexture(this->getTexture());

            //set to grid
            frag_grid[i][j] = frag;
            //add to batchNode
            frag_batch_node->addChild(frag);
            //random
            frag->random_number = rand_distribution(rand_generator);
        }
    }
}

void ShatterSprite::resetShatter(){
    Size contentSize = this->getContentSize();

    int num_rows = (int)frag_grid.size();
    int num_cols = (num_rows == 0 ? 0 : (int)frag_grid[0].size());

    const float half_grid_len = (float)0.5*grid_side_len;

    float offset_x = this->getTextureRect().origin.x;
    float offset_y = this->getTextureRect().origin.y;

    for (int i = 0; i < num_rows; i++){
        for (int j = 0; j < num_cols; j++){
            ShatterFrag* frag = frag_grid[i][j];
            //position
            float x = j * grid_side_len + half_grid_len;
            float y = contentSize.height - (i * grid_side_len + half_grid_len);
            //texture and textureRect
            frag->setTextureRect(Rect(
                x - half_grid_len + offset_x,
                (contentSize.height - y) - half_grid_len + offset_y,
                grid_side_len,
                grid_side_len
                ));

            //set position
            frag->setPosition(Vec2(x, y));
            //scale
            frag->setScale(initial_frag_scale);
            //opacity
            frag->setOpacity(255);
            //visible
            frag->setVisible(true);
        }
    }

}

void ShatterSprite::updateShatterAction(float time, float dt, float growSpeedOfTargetR){
    //update frags
    Size contentSize = this->getContentSize();
    Vec2 center = Vec2(contentSize.width / 2, contentSize.height / 2);

    //radius of surrounding circle
    float initalTargetR = Vec2(contentSize.width, contentSize.height).getLength() / 2;

    int nRow = (int)frag_grid.size();
    int nCol = nRow ? (int)frag_grid[0].size() : 0;

    for (int i = 0; i < nRow; i++){
        for (int j = 0; j < nCol; j++){
            ShatterFrag*frag = frag_grid[i][j];

            if (frag->getOpacity() == 0 || frag->getScale() == 0){
                frag->setVisible(false);
                continue;
            }

            //current target R
            float targetR = initalTargetR + time*growSpeedOfTargetR;
            //update postion

            Vec2 fragPos = frag->getPosition();

            Vec2 direction;
            float disToCenter = Vec2(fragPos - center).getLength();

            if (disToCenter == 0) {
                direction = Vec2::ZERO;
            }
            else {
                direction = fragPos - center;
                direction.x /= disToCenter;
                direction.y /= disToCenter;
            }

            float edge_dist = targetR - disToCenter;
            float edge_random_dist = edge_dist + frag->random_number % (int)initalTargetR - initalTargetR / 2;//add random to disToEdge

            float move_length = (float)(edge_random_dist*0.0333f); //we only move some percent of disToEdgeWithRandom

            Vec2 moved_pos = direction * move_length;
            frag->setPosition(fragPos + moved_pos);

            //update opacity
            GLubyte opacity = (GLubyte)MAX(0, 255 - 255 * disToCenter / initalTargetR);
            frag->setOpacity(opacity);

            //update scale
            float scale = MAX(
                0,
                this->initial_frag_scale - this->initial_frag_scale * disToCenter / initalTargetR
                );
            frag->setScale(scale);
        }
    }
}
//-----------------------------------------
//creates the action
ShatterAction* ShatterAction::create(float duration){

    ShatterAction *p = new ShatterAction();
    p->initWithDuration(duration);
    p->autorelease();

    return p;
}

//initializes the action
bool ShatterAction::initWithDuration(float duration){
    if (ActionInterval::initWithDuration(duration))
    {
        //my init code
        initMembers();
        return true;
    }

    return false;
}

bool ShatterAction::isDone(){
    return ActionInterval::isDone();
}

void ShatterAction::stop(){

    //clean up and reset
    ((ShatterSprite*)_target)->frag_batch_node->setVisible(true);
    //call father stop
    ActionInterval::stop();
}

void ShatterAction::startWithTarget(Node *pTarget){
    //reset
    ((ShatterSprite*)pTarget)->resetShatter();
    ((ShatterSprite*)pTarget)->frag_batch_node->setVisible(true);
    //call father startWithTarget
    ActionInterval::startWithTarget(pTarget);

}

void ShatterAction::update(float time){
    //the time actually is percentage, namely eclipsedTime/totalDuration
    //the t in action's update(t) and sprite's update(t) is different meaning
    //t in action's update(t) means the progress percentage, see ActionInterval::step()
    //t in sprite's update(t) means the deltaTime between current frame and previous frame, see Director::drawScene()
    //cout<<"time:"<<time<<endl;
    time_foe = cur_time;
    float progressPercentage = time;
    cur_time = progressPercentage*getDuration();
    ((ShatterSprite*)_target)->updateShatterAction(cur_time, cur_time - time_foe, m_growSpeedOfTargetR);

}
