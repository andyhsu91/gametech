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

class Player
{
public:
    Player(Ogre::SceneManager* pSceneMgr, PhysicsSimulator* sim, std::string node);
    ~Player(void);
	btRigidBody* getRigidBody(void);
	void updatePosition(const Ogre::FrameEvent& evt);
	void updatePadDirection(int element, bool value);

private:
	Ogre::SceneManager* mSceneMgr;
	PhysicsSimulator* bullet;
	
	btRigidBody* paddle;
	btTransform trans;
};

#endif // #ifndef __Player_h_
