/*
-----------------------------------------------------------------------------
Filename:    Score.h
-----------------------------------------------------------------------------
*/
#ifndef __Score_h_
#define __Score_h_

#include <Ogre.h>
#include <stdio.h>


class Score
{
public:
    Score(void) {
    	maxScore = 0;
    	score = 0;
    	secondScore = 0;
    	sprintf (topPlayer, "---");
    }
	int getScore() {
		return score;
	}
	int getMaxScore() {
		return maxScore;
	}
	int getSecondScore() {
		return secondScore;
	}
	void incrementScore() {
		score++;
	}
	void incrementSecondScore() {
		secondScore++;
	}
	bool resetScore() {
		bool result = false;
		if(score > maxScore) {
			maxScore = score;
			sprintf (topPlayer, "Player 1");
		} else {
			result = true;
		}
		score = 0;
		return result;
	}
	bool resetSecondScore() {
		bool result = false;
		if(secondScore > maxScore) {
			maxScore = secondScore;
			sprintf (topPlayer, "Player 2");
		} else {
			result = true;
		}
		secondScore = 0;
		return result;
	}
	char* getTopPlayer() {
		return &topPlayer[0];
	}
		

private:
	int maxScore;
	int score;
	int secondScore;
	char topPlayer[32];
};

#endif // #ifndef __Score_h_
