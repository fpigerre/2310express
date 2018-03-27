#ifndef _310express_h
#define _310express_h

#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>

#include "connection.h"
#include "game.h"
#include "game_control.h"
#include "messages.h"

#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 79

/**
 *  Checks arguments passed to the hub program.
 */
void check_args(int argc, char** argv);

/**
 *  Forks and initialises a specific player.
 *  Returns a Client struct containing child details.
 *  (Note: If the player program doesn't output an '!' character to
 *  stdout after starting, the hub will exit)
 */
struct Client initialise_comms(char* programPath, char** const programArgs);

/**
 *  Exits the hub program when a child program is terminated.
 */
void handle_sigchld(int status);

/**
 *  Initialises a signal handler to deal with child programs exiting.
 */
void initialise_signal_handler(void);

/**
 *  Waits for and receives status information about all player programs.
 *
 *  Parameters:
 *      (int) programCount: The amount of player programs
 *      (struct Client*) clients: An array of structs containing client data
 */
void synchronise(int programCount, struct Client* clients);

#endif /* _310express_h */
