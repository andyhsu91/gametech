#ifndef _gameUpdate_H_
#define _gameUpdate_H_

struct gameUpdate
{	
	//must be smaller than 512 bytes
	float ballPos[3];
	float ballVel[3];
	float paddlePos[3];
	bool  paddleDir[4];

};


#endif 
