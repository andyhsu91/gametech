/*
-----------------------------------------------------------------------------
Filename:    Ball.h
-----------------------------------------------------------------------------
*/
#ifndef __Ball_h_
#define __Ball_h_

#include <btBulletDynamicsCommon.h>
#include <Ogre.h>
#include <btBulletCollisionCommon.h>
#include <stdio.h>
#include "PhysicsSimulator.h"
#include "Score.h"
#include "SoundManager.h"
#include "gameUpdate.h"

class Ball
{
public:
    Ball(void);
    virtual ~Ball(void);
	void initBall(Ogre::SceneManager* pSceneMgr, PhysicsSimulator* sim, 
		SoundManager* sm, Score* sc, bool isServer, bool isMultiplayer);
	void resetBall(btVector3 ballPos);
	void updateBallPos(btVector3 ballPos);
	void incrementBallType();
	void randomizeStartVelocity(void);
	void update();
	void update(gameUpdate* update);
	gameUpdate* getBallGameState();
	float* getStartVelocity();
	void updateBulletBallPos(btVector3 gameUpdateBallPos, btVector3 bulletBallPos);
	void setVelocity(float* vel);
	
private:
	Ogre::SceneManager* mSceneMgr;
	PhysicsSimulator* bullet;
	SoundManager* sound_manager;
	Score* score;
	
	float cooldownMax; //# of frames the cooldown is in effect

	btVector3 currBallPos;
	btVector3 currBallDir;
	btVector3 cooldown;
	
	Ogre::SceneNode* ballNode;
	btRigidBody* ball;
	
	bool isMulti;
};

#endif // #ifndef __Ball_h_
