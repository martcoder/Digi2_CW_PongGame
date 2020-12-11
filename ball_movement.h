/*
 * ball_movement.h
 *
 *  Created on: 10 Nov 2017
 *      Author: mor2
 */

#ifndef BALL_MOVEMENT_H_
#define BALL_MOVEMENT_H_

void updateWinningScoreString(volatile char* winningString, int scorer);
void updateScoreString(volatile char* scoreString, int scorer);
void updateCurrentScoresString(volatile char* currentScoresString, int p1score, int p2score);
void ball_update(void);

#endif /* BALL_MOVEMENT_H_ */
