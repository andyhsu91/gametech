/*
-----------------------------------------------------------------------------
Filename:    PhysicsSimulator.h
-----------------------------------------------------------------------------
*/
#ifndef __PhysicsSimulator_h_
#define __PhysicsSimulator_h_

#include <btBulletDynamicsCommon.h>
#include <Ogre.h>
#include <btBulletCollisionCommon.h>
#include "MyMotionState.h"
#include <stdio.h>

class PhysicsSimulator
{
public:
    PhysicsSimulator(void);
    virtual ~PhysicsSimulator(void);
	void initPhysics(Ogre::SceneManager* pSceneMgr);
	void updateWorld(const Ogre::FrameEvent& evt);
	btRigidBody* setRigidBoxBody(Ogre::SceneNode *snode, 
		Ogre::Vector3 shapeDim, Ogre::Vector3 origin, double mass);
	void setBallRestitution(double restit);

private:
	Ogre::SceneManager* mSceneMgr;
	btDbvtBroadphase* overlappingPairCache;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	
	btCollisionShape* shape;
	btRigidBody* body;
	btTransform startTransform;
	
};

#endif // #ifndef __PhysicsSimulator_h_
