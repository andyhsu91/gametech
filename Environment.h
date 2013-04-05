/*
-----------------------------------------------------------------------------
Filename:    Environment.h
-----------------------------------------------------------------------------
*/
#ifndef __Environment_h_
#define __Environment_h_

#include <btBulletDynamicsCommon.h>
#include <Ogre.h>
#include <btBulletCollisionCommon.h>
#include <stdio.h>
#include "PhysicsSimulator.h"

class Environment
{
public:
    Environment(void);
    virtual ~Environment(void);
	void initEnvironment(Ogre::SceneManager* pSceneMgr, 
		PhysicsSimulator* sim, bool isMultiplayer);

private:
	Ogre::SceneManager* mSceneMgr;
	PhysicsSimulator* bullet;
};

#endif // #ifndef __Environment_h_
