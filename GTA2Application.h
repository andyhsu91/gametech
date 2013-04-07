/*
-----------------------------------------------------------------------------
Filename:    GTA2Application.h
-----------------------------------------------------------------------------
*/
#ifndef __GTA2Application_h_
#define __GTA2Application_h_

#include "BaseApplication.h"
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>

class GTA2Application : public BaseApplication
{
public:
	CEGUI::OgreRenderer* mRenderer;
    GTA2Application(void);
    virtual ~GTA2Application(void);

protected:
    virtual void createScene(void);
    virtual void createCamera(void);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt );
    void setMute(bool val);
    virtual bool keyPressed( const OIS::KeyEvent& evt );
    virtual bool keyReleased( const OIS::KeyEvent& evt );
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    bool quit(const CEGUI::EventArgs &e);
	bool start(const CEGUI::EventArgs &e);
	bool netStart(const CEGUI::EventArgs &e);
};

#endif // #ifndef __GTA2Application_h_
