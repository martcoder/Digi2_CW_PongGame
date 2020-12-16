/*
 * game_mechanics.h
 *
 *  Created on: 10 Nov 2017
 *      Author: mor2
 */

#ifndef GAME_MECHANICS_H_
#define GAME_MECHANICS_H_
#endif /* GAME_MECHANICS_H_ */

#include "game_mechanics_auxiliary.h"

void game_update(void);

// bonus related functions
void bonus_enable(void);
void handle_bonus_projectile_hitting_ball(void);

// game state related functions
void game_load(void);
void game_intro(void);
void game_start(void);
void moving(void);
void player1scoring(void);
void player2scoring(void);
void winning(void);


