#ifndef game_control_h
#define game_control_h

#include <stdio.h>

#include "connection.h"
#include "game.h"
#include "messages.h"

/**
 *  Broadcast a message to all players currently playing.
 *
 *  Parameters:
 *      (struct Message) message: The message to broadcast
 *      (struct Client *) clients: An array of clients to broadcast to
 *      (int) playerCount: The total amount of players currently playing
 */
void broadcast_message(struct Message message, struct Client* clients, int playerCount);

/**
 *  Begin a round of the game.
 *
 *  Parameters:
 *      (int) playerCount: The total amount of players currently playing
 *      (struct Client *) clients: An array containing all players currently playing
 *      (struct Game *) game: The data structure containing game data
 */
int begin_round(int playerCount, struct Client* clients, struct Game* game);

/**
 *  Once all the actions have been received for the current round, execute them
 *  accordingly.
 *
 *  Parameters:
 *      (int) playerCount: The total amount of players currently playing
 *      (struct Client *) clients: An array containing all players currently playing
 *      (struct Game *) game: The data structure containing game data
 *      (int *) roundActions: An array containing actions from the action enum for the round
 */
void execute_actions(int playerCount, struct Client* clients, struct Game* game, int* roundActions);

/**
 *  Execute a horizontal move for a particular player
 *
 *  Parameters:
 *      (struct Client *) client: The client for which to execute the horizontal move
 *      (struct Game *) game: The data structure containing game data
 */
struct Message execute_horizontal(struct Client* client, struct Game* game);

/**
 *  Execute a vertical move for a particular player
 *
 *  Parameters:
 *      (struct Client *) client: The client for which to execute the vertical move
 *      (struct Game *) game: The data structure containing game data
 */
struct Message execute_vertical(struct Client* client, struct Game* game);

/**
 *  Execute a loot action for a particular player
 *
 *  Parameters:
 *      (struct Client *) client: The client for which to execute the loot action
 *      (struct Game *) game: The data structure containing game data
 */
struct Message execute_loot(struct Client* client, struct Game* game);

/**
 *  Execute a short action for a particular player
 *
 *  Parameters:
 *      (struct Client *) client: The client for which to execute the short action
 *      (struct Game *) game: The data structure containing game data
 */
struct Message execute_short(struct Client* client, struct Game* game);

#endif /* game_control_h */
