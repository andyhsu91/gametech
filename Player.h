/*
-----------------------------------------------------------------------------
Filename:    Player.h
-----------------------------------------------------------------------------
*/
#ifndef __Player_h_
#define __Player_h_

#include <btBulletDynamicsCommon.h>
#include <Ogre.h>
#include <btBulletCollisionCommon.h>
#include <stdio.h>
#include "PhysicsSimulator.h"
#include "gameUpdate.h"

class Player
{
public:
    Player(Ogre::SceneManager* pSceneMgr, PhysicsSimulator* sim, 
    	std::string node, std::string color);
    ~Player(void);
	btRigidBody* getRigidBody(void);
	void updatePosition(const Ogre::FrameEvent& evt);
	void updatePosition(const Ogre::FrameEvent& evt, gameUpdate* update);
	void updatePadDirection(int element, bool value);
	gameUpdate* getPlayerGameState(void);

private:
	Ogre::SceneManager* mSceneMgr;
	PhysicsSimulator* bullet;
	btRigidBody* paddle;
	btTransform trans;
	
	gameUpdate* mPlayerState;
	bool forceUpdate;
};

#endif // #ifndef __Player_h_
