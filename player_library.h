#ifndef player_library_h
#define player_library_h

#include <signal.h>

#include "game.h"
#include "messages.h"

/**
 *  Checks arguments passed to the player program.
 */
void check_args(int argc, char** argv);

/**
 *  Exits the player program when a communication error occurs.
 */
void handle_sigpipe(int status);

/**
 *  Initialises a signal handler to deal with pipe-based communication errors.
 */
void initialise_signal_handler(void);

/**
 *  Changes data structures internal to the player program according to
 *  messages sent from the hub (such as player actions).
 *
 *  Parameters:
 *      (struct Message) currentMessage: A message being sent by the hub
 *      (struct Game*) game: The data structure containing game data
 */
void handle_generic_actions(struct Message currentMessage, struct Game* game);

#endif /* player_library_h */
