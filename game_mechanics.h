/*
 * game_mechanics.h
 *
 *  Created on: 10 Nov 2017
 *      Author: mor2
 */

#ifndef GAME_MECHANICS_H_
#define GAME_MECHANICS_H_

void updateScoresBannerString(volatile char* scoresBannerString,int p1score, int p2score);
void updateWinningScoreString(volatile char* winningString, int scorer);
void updateScoreString(volatile char* scoreString, int scorer);
void updateCurrentScoresString(volatile char* currentScoresString, int p1score, int p2score);
void clearProjectiles(void);
void resetProjectiles(void);
void game_update(void);

#endif /* GAME_MECHANICS_H_ */
