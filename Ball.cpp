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
float mass = 100.0;
static int edgeSize = 500;
float nodeScale = .1;
float rigidScale = 50.0;
gameUpdate* mBallState;

Ball::Ball(void)
{
}
//---------------------------------------------------------------------------
Ball::~Ball(void)
{	
	if(mBallState){delete mBallState;}
}
//---------------------------------------------------------------------------
void Ball::initBall(Ogre::SceneManager* pSceneMgr, PhysicsSimulator* sim, 
	SoundManager* sm, Score* sc, bool isServer, bool isMultiplayer)
{
	mBallState = new gameUpdate; //safe
	
	mSceneMgr = pSceneMgr;
	bullet = sim;
	sound_manager = sm;
	score = sc;
	isMulti = isMultiplayer;
	cooldownMax = 100.0; //frames between playing the same sound
	
	// Create an Entity
    Ogre::Entity* ballz = mSceneMgr->createEntity("Sphere", "sphere.mesh");
 	
    // Create a SceneNode and attach the Entity to it
    ballNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("BallNode");
    ballNode->attachObject(ballz);

    ballNode->scale(nodeScale,nodeScale,nodeScale);
    ballz->setMaterialName("Examples/Tennis");
   	ballz->setCastShadows(true);
    //ballz->getAnimationState("Attack1")->setEnabled(true);
    //ballz->getAnimationState("Jump")->setLoop(true); 
	
    //float mass = isServer ? 100.0 : 0.0;
    
    
    //change this line for danbo vs sphere
    Ogre::Vector3 dim = rigidScale*ballNode->getScale();
    Ogre::Vector3 pos = ballNode->getPosition();
    ball = bullet->setRigidBoxBody(ballNode, dim, pos, mass);
    
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
    
    mBallState->ballPos[0] = 0.0;
    mBallState->ballPos[1] = 0.0;
    mBallState->ballPos[2] = 0.0;
    
    mBallState->ballVel[0] = startVelocity.getX();
    mBallState->ballVel[1] = startVelocity.getY();
    mBallState->ballVel[2] = startVelocity.getZ();
}
void Ball::resetBall(btVector3 ballPos)
{
	float rf = 1.0;
	//this function moves ball back to origin
	btVector3 opposite = btVector3(-ballPos.getX()/rf, -ballPos.getY()/rf, -ballPos.getZ()/rf);

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
    
    mBallState->ballPos[0] = 0.0;
    mBallState->ballPos[1] = 0.0;
    mBallState->ballPos[2] = 0.0;
    
    mBallState->ballVel[0] = startVelocity.getX();
    mBallState->ballVel[1] = startVelocity.getY();
    mBallState->ballVel[2] = startVelocity.getZ();
	
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
{	//this function decides whether to reset score or not
	btTransform ballTrans;
	ball->getMotionState()->getWorldTransform(ballTrans);
	btVector3 bulletBallPos = ballTrans.getOrigin();
	
	updateBallPos(bulletBallPos);
	
	if( abs(bulletBallPos.getX()) > edgeSize*1.01 ||
		abs(bulletBallPos.getY()) > edgeSize*1.01 ||
		abs(bulletBallPos.getZ()) > edgeSize*1.01	)
	{
		resetBall(bulletBallPos);
		
		if(bulletBallPos.getZ() > edgeSize*1.01) {
			if(score->resetServerScore() && cooldown.getZ()==0.0) {
				sound_manager->playFailure();
			}	
			cooldown.setZ(cooldownMax);
		}
		
		if(bulletBallPos.getZ() < -edgeSize*1.01) {
			score->resetClientScore();
			if(cooldown.getZ() == 0.0){
				sound_manager->playSuccess();
			}
			cooldown.setZ(cooldownMax);
		}
		
	} 
}



void Ball::updateBallPos(btVector3 ballPos){
	//this function does not update bullet, it only checks for score updates or bounces
	
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
			score->incrementServerScore();
			
			if(isMulti) { score->incrementClientScore(); }
		}
		cooldown.setX(cooldownMax);
		
	}
	if(yDir != currBallDir.getY() && currBallDir.getY() != 0.0){
		//ball movement direction has switched in the y axis
		//play bounce
		if(cooldown.getY()==0.0){
			if(yDir==1.0 && currBallDir.getY() == -1.0){
				sound_manager->playBounce();
				score->incrementServerScore();
				
				if(isMulti) { score->incrementClientScore(); }
			}
		}
		cooldown.setY(cooldownMax);
	}
	
	if(zDir != currBallDir.getZ() && currBallDir.getZ() != 0.0){
		//ball movement direction has switched in the z axis
		//play bounce
		if(cooldown.getZ()==0.0){
			sound_manager->playBounce();
			score->incrementServerScore();
			
			if(isMulti) { score->incrementClientScore(); }
		}
		cooldown.setZ(cooldownMax);
	}
	
	//update ball position
	currBallPos = ballPos;
	
	//update ball direction
	(xDir > 0) ? currBallDir.setX(1.0) : currBallDir.setX(-1.0);
	(yDir > 0) ? currBallDir.setY(1.0) : currBallDir.setY(-1.0);
	(zDir > 0) ? currBallDir.setZ(1.0) : currBallDir.setZ(-1.0);
	
	mBallState->ballPos[0] = currBallPos.getX();
    mBallState->ballPos[1] = currBallPos.getY();
    mBallState->ballPos[2] = currBallPos.getZ();
}


float* Ball::getStartVelocity(){
	float* vel = new float[3];
	vel[0] = startVelocity.getX();
	vel[1] = startVelocity.getY();
	vel[2] = startVelocity.getZ();
	return vel;
}

void Ball::setVelocity(float* vel){
	startVelocity = btVector3(vel[0],vel[1], vel[2]);
	ball->setLinearVelocity(startVelocity);
}

void Ball::updateBulletBallPos(btVector3 gameUpdateBallPos, btVector3 bulletBallPos){
	//client side function that moves ball where the server says the ball is
	//if this function isn't called in a while, bullet will continue to estimate where it thinks the ball should be
	float xDiff = gameUpdateBallPos.getX() - bulletBallPos.getX();
	float yDiff = gameUpdateBallPos.getY() - bulletBallPos.getY();
	float zDiff = gameUpdateBallPos.getZ() - bulletBallPos.getZ();
	float rf = 10.0;
	btVector3 opposite = btVector3(xDiff/rf, yDiff/rf, zDiff/rf);
	ball->translate(opposite);
	
}

void Ball::update(gameUpdate* update)
{
	mBallState = update;
	
	btTransform ballTrans;
	ball->getMotionState()->getWorldTransform(ballTrans);
	btVector3 bulletBallPos = ballTrans.getOrigin();
	
	btVector3 gameUpdateBallPos = btVector3(update->ballPos[0], update->ballPos[1], update->ballPos[2]);
	
	
	updateBulletBallPos(gameUpdateBallPos, bulletBallPos);
	updateBallPos(gameUpdateBallPos);
	
	if( abs(gameUpdateBallPos.getX()) > edgeSize*1.01 ||
		abs(gameUpdateBallPos.getY()) > edgeSize*1.01 ||
		abs(gameUpdateBallPos.getZ()) > edgeSize*1.01	)
	{
		resetBall(gameUpdateBallPos);
		
		if(gameUpdateBallPos.getZ() > edgeSize*1.01) {
			if(score->resetServerScore() && cooldown.getZ()==0.0) {
				sound_manager->playFailure();
			}	
			cooldown.setZ(cooldownMax);
		}
		
		if(gameUpdateBallPos.getZ() < -edgeSize*1.01 && cooldown.getZ()==0.0) {
			score->resetClientScore();
			sound_manager->playSuccess();
		}
		cooldown.setZ(cooldownMax);
	} 
}
gameUpdate* Ball::getBallGameState()
{
	return mBallState;
}

