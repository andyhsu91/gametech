#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#include <Ogre.h>

class MyMotionState : public btMotionState
{
public:
	MyMotionState(Ogre::SceneNode* node)
	{
		mNode = node;
		mTrans.setIdentity();
	}
	virtual ~MyMotionState() {}
	virtual void getWorldTransform(btTransform &worldTrans) const
	{
		worldTrans = mTrans;
	}
	virtual void setWorldTransform(const btTransform &worldTrans)
	{
		mTrans = worldTrans;
		btQuaternion ori = mTrans.getRotation();
		btVector3 pos = mTrans.getOrigin();
		mNode->setPosition(Ogre::Vector3(pos.x(),pos.y(),pos.z()));
		mNode->setOrientation(Ogre::Quaternion(ori.w(),ori.x(),ori.y(),ori.z()));
	}

protected:
	Ogre::SceneNode* mNode;
	btTransform mTrans;
	
};
