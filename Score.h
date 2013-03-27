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
    }
	int getScore() {
		return score;
	}
	int getMaxScore() {
		return maxScore;
	}
	void incrementScore() {
		score++;
	}
	bool resetScore() {
		bool result = false;
		if(score > maxScore){
			maxScore = score;
		}else{
			result = true;
		}
		score = 0;
		return result;
	}

private:
	int maxScore;
	int score;
};

#endif // #ifndef __Score_h_
