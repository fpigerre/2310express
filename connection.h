#ifndef connection_h
#define connection_h

#include <unistd.h>

/**
 *  A data structure containing the file streams necessary to communicate with a client.
 */
struct Client {
    FILE* writeStream; // Parent -> Child pipe
    FILE* readStream; // Child -> Parent pipe
    pid_t pid; // Process ID of child process
    int playerNumber; // Index of player within array of players
};

#endif /* connection_h */
