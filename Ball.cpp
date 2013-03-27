/*
-----------------------------------------------------------------------------
Filename:    Ball.cpp
-----------------------------------------------------------------------------
*/
#include "Ball.h"
#include <cstdlib>

using namespace std;

static int ballType = 0;
static int maxBallType = 5;

static Ogre::Vector3 velocityVec = Ogre::Vector3::ZERO;
btVector3 startVelocity = btVector3(0,0, -250);
int maxVelocity = 300;
static int speedModifier = 3;

static int edgeSize = 500;

Ball::Ball(void)
{
}
//---------------------------------------------------------------------------
Ball::~Ball(void)
{	
}
//---------------------------------------------------------------------------
void Ball::initBall(Ogre::SceneManager* pSceneMgr, 
	PhysicsSimulator* sim, SoundManager* sm, Score* sc)
{
	mSceneMgr = pSceneMgr;
	bullet = sim;
	sound_manager = sm;
	score = sc;
	cooldownMax = 20.0;
	
	// Create an Entity
    Ogre::Entity* ballz = mSceneMgr->createEntity("Sphere", "Danboard.mesh");
 
    // Create a SceneNode and attach the Entity to it
    ballNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("BallNode");
    ballNode->attachObject(ballz);

    ballNode->scale(5,5,5);
    ballz->setMaterialName("Examples/DANBO");
   	ballz->setCastShadows(true);
    
    //change this line for danbo vs sphere
    Ogre::Vector3 dim = 1*ballNode->getScale();
    Ogre::Vector3 pos = ballNode->getPosition();
    ball = bullet->setRigidBoxBody(ballNode, dim, pos, 100.0);
    randomizeStartVelocity();
    ball->setLinearVelocity(startVelocity);
    
    currBallDir.setX(0.0);
    currBallDir.setY(0.0);
    currBallDir.setZ(0.0);
    
    currBallPos.setX(0.0);
    currBallPos.setY(0.0);
    currBallPos.setZ(0.0);
    
    cooldown.setX(0.0);
    cooldown.setY(0.0);
    cooldown.setZ(0.0);
}
void Ball::resetBall(btTransform ballTrans, btVector3 ballPos)
{
	
	btVector3 opposite = btVector3(-ballPos.getX()/10, -ballPos.getY()/10, -ballPos.getZ()/10);

	ball->translate(opposite);
	randomizeStartVelocity();
	ball->setLinearVelocity(startVelocity);
		
	currBallDir.setX(0.0);
    currBallDir.setY(0.0);
    currBallDir.setZ(0.0);
    
    currBallPos.setX(0.0);
    currBallPos.setY(0.0);
    currBallPos.setZ(0.0);
    
    cooldown.setX(0.0);
    cooldown.setY(0.0);
    cooldown.setZ(0.0);
	
}
void Ball::updateBallPos(btVector3 ballPos){
	
	//find out if traveling in positive or negative direction for each axis
	float xDir = (ballPos.getX() - currBallPos.getX()) > 0.0 ? 1.0 : -1.0;
	float yDir = (ballPos.getY() - currBallPos.getY()) > 0.0 ? 1.0 : -1.0;
	float zDir = (ballPos.getZ() - currBallPos.getZ()) > 0.0 ? 1.0 : -1.0;
	
	if(cooldown.getX() >= 1.0){
		cooldown.setX(cooldown.getX()-1.0);
	} else{ cooldown.setX(0.0); }
	
	if(cooldown.getY() >= 1.0){
		cooldown.setY(cooldown.getY()-1.0);
	} else{ cooldown.setY(0.0); }
	
	if(cooldown.getZ() >= 1.0){
		cooldown.setZ(cooldown.getZ()-1.0);
	} else{ cooldown.setZ(0.0); }
	
	if(xDir != currBallDir.getX() && currBallDir.getX() != 0.0){
		//ball movement direction has switched in the x axis
		//play bounce so long as it hasn't been played in the last few frames
		if(cooldown.getX()==0.0){
			sound_manager->playBounce();
			score->incrementScore();
		}
		cooldown.setX(cooldownMax);
		
	}
	if(yDir != currBallDir.getY() && currBallDir.getY() != 0.0){
		//ball movement direction has switched in the y axis
		//play bounce
		if(cooldown.getY()==0.0){
			if(yDir==1.0 && currBallDir.getY() == -1.0){
				sound_manager->playBounce();
				score->incrementScore();
			}
		}
		cooldown.setY(cooldownMax);
	}
	
	if(zDir != currBallDir.getZ() && currBallDir.getZ() != 0.0){
		//ball movement direction has switched in the z axis
		//play bounce
		if(cooldown.getZ()==0.0){
			sound_manager->playBounce();
			score->incrementScore();
		}
		cooldown.setZ(cooldownMax);
	}
	
	//update ball position
	currBallPos = ballPos;
	
	//update ball direction
	(xDir > 0) ? currBallDir.setX(1.0) : currBallDir.setX(-1.0);
	(yDir > 0) ? currBallDir.setY(1.0) : currBallDir.setY(-1.0);
	(zDir > 0) ? currBallDir.setZ(1.0) : currBallDir.setZ(-1.0);	
	
}
void Ball::incrementBallType()
{
	if(ballType == maxBallType-1)
		bullet->setBallRestitution(1.00);
	else
		bullet->setBallRestitution(1.05);
		
	Ogre::Entity* ballz = mSceneMgr->getEntity("Sphere");

	ballType = (++ballType) % maxBallType;
	string type = "Ball/Ball" + ballType;
	ballz->setMaterialName(type);
}
void Ball::randomizeStartVelocity(void)
{

	float x = ((double)rand()/(RAND_MAX)) * maxVelocity + 100; //make sure that its at least 100
	float y = ((double)rand()/(RAND_MAX)) * maxVelocity + 100;
	float z = ((double)rand()/(RAND_MAX)) * maxVelocity + 100;
	
	//randomize start velocity, but make sure that ball starts in negative z direction.
	startVelocity = btVector3(x,y,-z);
	
}
void Ball::update()
{
	btTransform ballTrans;
	ball->getMotionState()->getWorldTransform(ballTrans);
	btVector3 ballPos = ballTrans.getOrigin();
	
	updateBallPos(ballPos);
	
	if( abs(ballPos.getX()) > edgeSize*1.01 ||
		abs(ballPos.getY()) > edgeSize*1.01 ||
		abs(ballPos.getZ()) > edgeSize*1.01	)
	{
		resetBall(ballTrans, ballPos);
		
		if(score->resetScore()) {
			sound_manager->playFailure();
		}
	} 
}

