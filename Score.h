/*
-----------------------------------------------------------------------------
Filename:    Score.h
-----------------------------------------------------------------------------
*/
#ifndef __Score_h_
#define __Score_h_

#include <Ogre.h>
#include <stdio.h>
#include "gameUpdate.h"

class Score
{
public:
    Score(void) {
    	maxScore = 0;
    	serverScore = 0;
    	clientScore = 0;
    	topPlayerNum=-1;
    	sprintf (topPlayer, "---");
    }
	int getServerScore() {
		return serverScore;
	}
	int getMaxScore() {
		return maxScore;
	}
	int getClientScore() {
		return clientScore;
	}
	void incrementServerScore() {
		serverScore++;
	}
	void incrementClientScore() {
		clientScore++;
	}
	bool resetServerScore() {
		bool result = false;
		if(serverScore > maxScore) {
			maxScore = serverScore;
			topPlayerNum=1;
			sprintf(topPlayer, "Player 1");
		} else {
			result = true;
		}
		serverScore = 0;
		return result;
	}
	bool resetClientScore() {
		bool result = false;
		if(clientScore > maxScore) {
			maxScore = clientScore;
			topPlayerNum=2;
			sprintf (topPlayer, "Player 2");
		} else {
			result = true;
		}
		clientScore = 0;
		return result;
	}
	char* getTopPlayer() {
		return &topPlayer[0];
	}

	int getTopPlayerNum(){
		return topPlayerNum;
	}
	
	void updateScore(gameUpdate* update){
		
		serverScore = update->scores[SERVER_SCORE];
		clientScore = update->scores[CLIENT_SCORE];
		maxScore = update->scores[HIGH_SCORE];
		topPlayerNum = update->topPlayerNum;
		if(topPlayerNum==1){
			sprintf(topPlayer, "Player 1");
		} else{
			sprintf(topPlayer, "Player 2");
		}
	}
		

private:
	int maxScore;
	int serverScore;
	int clientScore;
	int topPlayerNum;
	char topPlayer[32];
};

#endif // #ifndef __Score_h_
