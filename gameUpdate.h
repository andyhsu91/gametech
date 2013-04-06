#ifndef _gameUpdate_H_
#define _gameUpdate_H_



struct gameUpdate
{	
	float ballPos[3]; 	//x, y, z
	float ballVel[3]; 	//x, y, z
	float paddlePos[3];	//x, y, z
	bool  paddleDir[4];	//up, down, left, right
	int   scores[3];	//server, client, high score
};


#endif 
