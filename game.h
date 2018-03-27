#ifndef game_h
#define game_h

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

/**
 *  A data structure representing a player's location, amount of loot and hits.
 */
struct Player {
    int x;
    int y;
    unsigned int loot;
    unsigned int hits;
};

/**
 *  A data structure that contains all the information necessary to initialise
 *  and startup the game.
 */
struct Game {
    unsigned int totalPlayers;
    unsigned int playerNumber;
    unsigned int carriageCount;
    unsigned int seed;
    unsigned int totalLoot;
    
    int isRunning; // Whether or not a GAME_OVER message has been received
    int roundActive;
    
    // An array representing the distribution of loot within the train
    // Accessed using loot[y][x], where y is the level of the train, and
    // x is the particular carriage
    unsigned int** loot;
    struct Player* players;
};

/**
 *  Initialise a game struct for the first time.
 *
 *  Parameters:
 *      (int) totalPlayers: The total amount of players currently playing
 *      (int) playerNumber: The index of the particular player program
 *      (int) carriageCount: The total amount of carriages in the game
 *      (int) seed: The seed with which to distribute loot
 *
 *  Returns a fully initialised (with memory allocated) Game struct.
 */
struct Game initialise_game(int totalPlayers, int playerNumber, int carriageCount, int seed);

/**
 *  Allocate memory for a game's loot array and begin distributing initial loot.
 */
void initialise_loot(struct Game* game);

/**
 *  Allocate memory for a game's array of players.
 */
void initialise_players(struct Game* game);

#endif /* game_h */
