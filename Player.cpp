/*
-----------------------------------------------------------------------------
Filename:    Player.cpp
-----------------------------------------------------------------------------
*/
#include "Player.h"
#include "PhysicsSimulator.h"
#include <cstdlib>

#define PAD_LEFT  0
#define PAD_RIGHT 1
#define PAD_UP    2
#define PAD_DOWN  3

using namespace std;

static int edgeSize = 500;

static double paddleModifier = 400.0f;
static double paddleScale = 0.75f;

static bool padDirect[4] = {false};
gameUpdate* mPlayerState;

bool forceUpdate;
 

Player::Player(Ogre::SceneManager* pSceneMgr, PhysicsSimulator* sim, std::string node)
{
	forceUpdate = false;
	
	mSceneMgr = pSceneMgr;
	bullet = sim;
	
	Ogre::Entity* ent = mSceneMgr->createEntity("PosXYEntity"+node, "cube.mesh");
   	Ogre::SceneNode* snode = mSceneMgr->getRootSceneNode()->
  		createChildSceneNode(node);
		
	Ogre::Vector3 shapeDim = Ogre::Vector3(edgeSize/5, edgeSize/5, 0.01);
   	Ogre::Vector3 position = Ogre::Vector3(0, 0, edgeSize/2);
		
	snode->attachObject(ent);
	snode->scale(shapeDim.x/100, shapeDim.y/100, shapeDim.z);
	snode->translate(position);
   	ent->setMaterialName("Examples/Red50");
   	ent->setCastShadows(false);
   	
   	paddle = bullet->setRigidBoxBody(snode, shapeDim, position, 0.0);
   	
	paddle->setCollisionFlags( paddle->getCollisionFlags() | 
		btCollisionObject::CF_KINEMATIC_OBJECT);
	paddle->setActivationState(DISABLE_DEACTIVATION);
	
	mPlayerState = new gameUpdate;

}
//---------------------------------------------------------------------------
Player::~Player(void)
{	
}
btRigidBody* Player::getRigidBody(void)
{
	return paddle;
}
void Player::updatePosition(const Ogre::FrameEvent& evt)
{
	paddle->getMotionState()->getWorldTransform(trans);
	btVector3 pos = trans.getOrigin();

	double movement_spd = paddleModifier * evt.timeSinceLastFrame;

	if( mPlayerState->paddleDir[PAD_RIGHT] )
	{
		if(pos.getX() < edgeSize/2)
			pos.setX(pos.getX() + movement_spd);
	}
	if( mPlayerState->paddleDir[PAD_LEFT] )
	{
		if(pos.getX() > -edgeSize/2)
			pos.setX(pos.getX() - movement_spd);
	}
	if( mPlayerState->paddleDir[PAD_UP] )
	{

		if(pos.getY() < edgeSize/2)
			pos.setY(pos.getY() + movement_spd);
	}
	if( mPlayerState->paddleDir[PAD_DOWN] )
	{
		if(pos.getY() > -edgeSize/2)
			pos.setY(pos.getY() - movement_spd);
	}
	
	if(forceUpdate) {
		pos.setX(mPlayerState->paddlePos[0]);
		pos.setY(mPlayerState->paddlePos[1]);
		pos.setZ(mPlayerState->paddlePos[2]);	
	}
	
	trans.setOrigin(pos);
	paddle->getMotionState()->setWorldTransform(trans);
	
	mPlayerState->paddlePos[0] = pos.getX();
	mPlayerState->paddlePos[1] = pos.getY();
	mPlayerState->paddlePos[2] = pos.getZ();

}
void Player::updatePosition(const Ogre::FrameEvent& evt, gameUpdate* update)
{
	mPlayerState = update;
	forceUpdate = true;
	updatePosition(evt);
}
void Player::updatePadDirection(int element, bool value)
{
	mPlayerState->paddleDir[element] = value;
}
gameUpdate* Player::getPlayerGameState(void)
{
	return mPlayerState;
}
