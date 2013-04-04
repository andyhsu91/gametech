/*
-----------------------------------------------------------------------------
Filename:    PhysicsSimulator.cpp
-----------------------------------------------------------------------------
*/
#include "PhysicsSimulator.h"
#include <cstdlib>

using namespace std;


static double gravity = -100;
static bool debug = true;
static double restitution = 1.05;
static double sr = 0.94; //scale ratio



PhysicsSimulator::PhysicsSimulator(void)
{
}
//---------------------------------------------------------------------------
PhysicsSimulator::~PhysicsSimulator(void)
{
	//Remove the rigidbodies from the dynamics world and delete them
	for (int i=dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}

	//Delete collision shapes
	for (int j=0;j<collisionShapes.size();j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	//Delete dynamics world
	delete dynamicsWorld;
	//Delete solver
	delete solver;
	//Delete broadphase
	delete overlappingPairCache;
	//Delete dispatcher
	delete dispatcher;
	delete collisionConfiguration;
	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();
}
//---------------------------------------------------------------------------
void PhysicsSimulator::initPhysics(Ogre::SceneManager* pSceneMgr)
{
	collisionConfiguration = new btDefaultCollisionConfiguration(); //safe
	dispatcher = new btCollisionDispatcher(collisionConfiguration); //safe
	overlappingPairCache = new btDbvtBroadphase(); //safe
	solver = new btSequentialImpulseConstraintSolver; //safe

	dynamicsWorld = new btDiscreteDynamicsWorld
		(dispatcher, overlappingPairCache, solver, collisionConfiguration); //safe

	dynamicsWorld->setGravity(btVector3(0, gravity, 0));
}
//---------------------------------------------------------------------------
void PhysicsSimulator::updateWorld(const Ogre::FrameEvent& evt)
{
	dynamicsWorld->stepSimulation(evt.timeSinceLastFrame,1);

	//moves positions of all objects
	for (int j=dynamicsWorld->getNumCollisionObjects()-1; j>=0 ;j--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
		obj->setRestitution(restitution);
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			Ogre::SceneNode *mNode = static_cast<Ogre::SceneNode *>(body->getUserPointer());
			
			btTransform trans;
			body->getMotionState()->getWorldTransform(trans);
			
			btQuaternion ori = trans.getRotation();
			btVector3 pos = trans.getOrigin();
			mNode->setPosition(Ogre::Vector3(pos.x(),pos.y(),pos.z()));
			mNode->setOrientation(Ogre::Quaternion(ori.w(),ori.x(),ori.y(),ori.z()));
		}
	}
}
//---------------------------------------------------------------------------
btRigidBody* PhysicsSimulator::setRigidBoxBody(Ogre::SceneNode *snode, 
		Ogre::Vector3 shapeDim, Ogre::Vector3 origin, double mass)
{	
	shape = new btBoxShape(btVector3(sr*shapeDim.x, sr*shapeDim.y, sr*shapeDim.z));
	collisionShapes.push_back(shape);
	
	startTransform.setIdentity();
	
	btVector3 localInertia(0,0,0);
	
	if(mass != 0.f)
		shape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(origin.x, origin.y, origin.z));
	
	btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
//	MyMotionState* motionState = new MyMotionState(snode);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo); 
	body->setUserPointer((void *) (snode));

	dynamicsWorld->addRigidBody(body); 
	
	return body;
}
void PhysicsSimulator::setBallRestitution(double restit) 
{
	restitution = restit;
}


























