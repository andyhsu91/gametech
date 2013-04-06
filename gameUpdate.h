#ifndef _gameUpdate_H_
#define _gameUpdate_H_

#define SERVER_SCORE 0
#define CLIENT_SCORE 1
#define HIGH_SCORE 2


struct gameUpdate
{	
	float ballPos[3]; 	//x, y, z
	float ballVel[3]; 	//x, y, z
	float paddlePos[3];	//x, y, z
	bool  paddleDir[4];	//up, down, left, right
	int   topPlayerNum;	//1=server, 2=client
	int   scores[3];	//server, client, high score,
};


#endif 
