Alejandro Weibel	aaw738
Andy Hsu			ah28348
Nelson Cheng		nc7559



'./makeit'			 will build this program
'./assignment2'		 runs the program 
Refer to Manual.txt for gameplay instructions.

Functionality that's been implemented
---------------------------------------------------------------
	- Source controlled by github
	- 1 ball (aka boxman) with 3 walls that bounces off of the walls, ceiling, and floor.
	- Ball always starts at (0, 0, 0) but is given a random velocity in a random direction away from player.
	- Ball bounces with collision from Bullet
	- Ball increases speed per bounce to increase difficulty 
	- WASD control paddle
	- mouse moves camera
	- CEGUI HUD system implemented
	- SDL Sound effects system implemented
	- Ball and walls textured
	- Scoring system tracks times bounced off a wall or paddle
	- Score resets when ball exits arena, high score updates as necessary
	- Toggle cursor to select quit button
	- Music player, loops until exit
	- Arena looks closer to squash court (http://en.wikipedia.org/wiki/File:US_Open_Squash_Championship_2011_Drexel_University.jpg)
	- Blender file -> mesh.xml -> Ogre .mesh file converter


Issues Encountered
---------------------------------------------------------------
	- Bullet collision inaccuracies
	- Ball sometimes move through walls due to bullet
	- Could not start menu under new window due to some sort of thread problem that ogre doesn't like
	- 

