/*
-----------------------------------------------------------------------------
Filename:    Environment.cpp
-----------------------------------------------------------------------------
*/
#include "Environment.h"
#include <cstdlib>

using namespace std;

Environment::Environment(void)
{
}
//---------------------------------------------------------------------------
Environment::~Environment(void)
{	
}
//---------------------------------------------------------------------------
void Environment::initEnvironment(Ogre::SceneManager* pSceneMgr, PhysicsSimulator* sim)
{
	mSceneMgr = pSceneMgr;
	bullet = sim;
	
	int floorSize = 10000;
	int edgeSize = 500;

    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
 
    Ogre::MeshManager::getSingleton().createPlane("base", 
    	Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, floorSize, floorSize, 20, 20, true, 1, 10, 10, Ogre::Vector3::UNIT_X);

    //Ground----------------------------------------------------------------------
    {   	    
		Ogre::Entity* ent = mSceneMgr->createEntity("GroundEntity", "base");
    	Ogre::SceneNode* snode = mSceneMgr->getRootSceneNode()->
    		createChildSceneNode("snode0", Ogre::Vector3(0,-edgeSize/2,0));
 		
 		snode->attachObject(ent);
    	ent->setMaterialName("Examples/Wood");
    	ent->setCastShadows(false);
    	
    	Ogre::Vector3 shapeDim = Ogre::Vector3(floorSize, 0.1, floorSize);
    	Ogre::Vector3 position = Ogre::Vector3(0, -edgeSize/2, 0);
    	bullet->setRigidBoxBody(snode, shapeDim, position, 0.0);
    }
    //Ceiling---------------------------------------------------------------------
	{   	    
		Ogre::Entity* ent = mSceneMgr->createEntity("CeilingEntity", "cube.mesh");
    	Ogre::SceneNode* snode = mSceneMgr->getRootSceneNode()->
    		createChildSceneNode("snode1");
 		
 		Ogre::Vector3 shapeDim = Ogre::Vector3(floorSize, 0.01, floorSize);
    	Ogre::Vector3 position = Ogre::Vector3(0, edgeSize/2, 0);
 		
 		snode->attachObject(ent);
 		snode->scale(shapeDim.x/100, shapeDim.y, shapeDim.z/100);
 		snode->translate(position);
    	ent->setMaterialName("Examples/CloudySky");
    	ent->setCastShadows(false);
    	
    	bullet->setRigidBoxBody(snode, shapeDim, position, 0.0);
    }
    //NegWall YZ------------------------------------------------------------------
    {   	    
		Ogre::Entity* ent = mSceneMgr->createEntity("NegYZEntity", "cube.mesh");
    	Ogre::SceneNode* snode = mSceneMgr->getRootSceneNode()->
    		createChildSceneNode("snode2");
 		
 		Ogre::Vector3 shapeDim = Ogre::Vector3(0.01, edgeSize, edgeSize);
    	Ogre::Vector3 position = Ogre::Vector3(-edgeSize/2, 0, 0);
 		
 		snode->attachObject(ent);
 		snode->scale(shapeDim.x, shapeDim.y/100, shapeDim.z/100);
 		snode->translate(position);
    	ent->setMaterialName("Examples/Blue50");
    	ent->setCastShadows(false);
   
    	bullet->setRigidBoxBody(snode, shapeDim, position, 0.0);
    }
    //PosWall YZ------------------------------------------------------------------
    {   	    
		Ogre::Entity* ent = mSceneMgr->createEntity("PosYZEntity", "cube.mesh");
    	Ogre::SceneNode* snode = mSceneMgr->getRootSceneNode()->
    		createChildSceneNode("snode3");
 		
 		Ogre::Vector3 shapeDim = Ogre::Vector3(0.01, edgeSize, edgeSize);
    	Ogre::Vector3 position = Ogre::Vector3(edgeSize/2, 0, 0);
 		
 		snode->attachObject(ent);
 		snode->scale(shapeDim.x, shapeDim.y/100, shapeDim.z/100);
 		snode->translate(position);
    	ent->setMaterialName("Examples/Blue50");
    	ent->setCastShadows(false);
    	
    	bullet->setRigidBoxBody(snode, shapeDim, position, 0.0);
    }
    //NegWall XY------------------------------------------------------------------
    {   	    
		Ogre::Entity* ent = mSceneMgr->createEntity("NegXYEntity", "cube.mesh");
    	Ogre::SceneNode* snode = mSceneMgr->getRootSceneNode()->
    		createChildSceneNode("snode4");
 		
 		Ogre::Vector3 shapeDim = Ogre::Vector3(edgeSize, edgeSize, 0.01);
    	Ogre::Vector3 position = Ogre::Vector3(0, 0, -edgeSize/2);
    	
 		snode->attachObject(ent);
 		snode->scale(shapeDim.x/100, shapeDim.y/100, shapeDim.z);
 		snode->translate(position);
    	ent->setMaterialName("Examples/Blue50");
    	ent->setCastShadows(false);

    	bullet->setRigidBoxBody(snode, shapeDim, position, 0.0);
    }
	
}
//---------------------------------------------------------------------------














