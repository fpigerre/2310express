#ifndef messages_h
#define messages_h

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 79

/**
 *  Types of messages able to be sent over a pipe
 */
typedef enum {
    GAME_OVER,
    ROUND,
    YOUR_TURN,
    ORDEREDXV,
    EXECUTE,
    HOPTION,
    SOPTION,
    LOPTION,
    HMOVEXY,
    VMOVEX,
    LONGXT,
    SHORTXT,
    LOOTEDX,
    DRIEDOUTX,
    PLAYX,
    SIDEWAYSX,
    TARGET_SHORTX,
    TARGET_LONGX,
    ERROR
} messageType;

/**
 *  Types of actions able to be carried out by a player
 */
typedef enum {
    HORIZONTAL,
    VERTICAL,
    LOOT,
    SHORT,
    LONG,
    DRY_OUT
} action;

/**
 *  A message able to be sent over a pipe
 */
struct Message {
    messageType type;
    char parameters[2];
};

/**
 *  Wait for a message on a particular file stream and interpret
 *  it as a 'Message' struct.
 *
 *  Parameters:
 *      (FILE *) file: A file stream on which to receive the message
 */
struct Message receive_message(FILE* file);

/**
 *  Interpret a message as a 'Message' struct.
 *
 *  Parameters:
 *      (char *) data: The raw message as an array of characters
 */
struct Message parse_message(char* data);

/**
 *  Send a message over a pipe using a file stream.
 *
 *  Parameters:
 *      (struct Message) message: A Message struct to send
 *      (FILE *) file: A file stream over which to send the message
 */
int send_message(struct Message message, FILE* file);

/**
 *  Encode a Message struct as a string for communication over a pipe.
 *
 *  Parameters:
 *      (char*) buffer: A string on the heap where the encoded message will be stored
 *      (struct Message) message: A Message struct to encode
*/
void encode_message(char* buffer, struct Message message);

/**
 *  Read a line from a file stream.
 */
char* read_line(FILE* file);

/**
 *  Check whether an encoded message matches a particular command.
 *
 *  Parameters:
 *      (char *) message: An encoded message to test
 *      (char *) command: The command to test the message against
 *      (int) parameterCount: The amount of parameters the command should have
 */
int matches_command(char* message, char* command, int parameterCount);

/**
 *  Convert a character representing an action, used over the wire protocol,
 *  into the action format.
 *
 *  Parameters:
 *      (char) actionCharacter: A character to convert
 *
 *  Returns:
 *      (action) A value from the 'action' enum
 */
action parse_action(char actionCharacter);

/**
 *  Test whether a particular string contains only numerical values.
 */
int is_numerical(char* string);

/**
 *  Convert a player character from character format to numerical, index format.
 *  Returns the index of the character within the alphabet.
 */
int convert_player_character(char playerCharacter);

/**
 *  Convert a numerical player index into a character format.
 *  Returns the letter of the alphabet corresponding to the index.
 */
char convert_player_index(int playerIndex);

#endif /* messages_h */
