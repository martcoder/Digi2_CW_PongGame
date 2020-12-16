
#ifndef GAME_MECHANICS_AUXILIARY_H_
#define GAME_MECHANICS_AUXILIARY_H_

#endif /* GAME_MECHANICS_AUXILIARY_H_ */

#ifndef MPS430F5438A_H
#define MPS430F5438A_H
#include    "msp430f5438a.h"
#endif

#ifndef HAL_HEADERS_H
#define HAL_HEADERS_H
#include    "hal_UCS.h"
#include    "hal_PMM.h"
#include    "hal_lcd.h"
#endif

#ifndef GENERAL_SETTINGS_H
#define GENERAL_SETTINGS_H
#include    "general_settings.h"
#endif

#include "lookup.c"

void racket_movement_effect(int player);

void clearProjectiles(void);
void resetProjectiles(void);
void powerUpProjectiles(void);

int top_wall_reached();
int bottom_wall_reached();
int right_wall_reached();
int left_wall_reached();
int P1_racket_hit();
int P1_racket_centre_hit();
int P1_racket_upper_hit();
int P1_racket_lower_hit();
int P2_bonus_racket_hit();
int P2_racket_hit();
int P2_racket_centre_hit();
int P2_racket_upper_hit();
int P2_racket_lower_hit();

void updateBannerString(volatile char* bannerString,int p1score, int p2score, int p1bonusEnabled, int p2bonusEnabled);
void updateWinningScoreString(volatile char* winningString, int scorer);
void updateScoreString(volatile char* scoreString, int scorer);
void updateCurrentScoresString(volatile char* currentScoresString, int p1score, int p2score);

