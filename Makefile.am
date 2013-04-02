noinst_HEADERS = BaseApplication.h GTA2Application.h MyMotionState.h PhysicsSimulator.h Player.h Environment.h Ball.h Score.h NetworkManager.h gameUpdate.h

bin_PROGRAMS = assignment3
assignment3_CPPFLAGS = -I$(top_srcdir)
assignment3_SOURCES = BaseApplication.cpp GTA2Application.cpp PhysicsSimulator.cpp NetworkManager.cpp SoundManager.cpp Player.cpp Environment.cpp Ball.cpp NetworkManager.cpp
assignment3_CXXFLAGS = $(OGRE_CFLAGS) $(OIS_CFLAGS) $(bullet_CFLAGS) -lSDL -lSDL_mixer -lSDL_net
assignment3_LDADD = $(OGRE_LIBS) $(OIS_LIBS) $(bullet_LIBS) $(SDL_LIBS) $(SDL_MIXER_LIBS) $(CEGUI_LIBS) $(CEGUI_OGRE_LIBS)

EXTRA_DIST = buildit makeit
AUTOMAKE_OPTIONS = foreign
