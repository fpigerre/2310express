#include "messages.h"

struct Message receive_message(FILE * file)
{
    char *incomingData = read_line(file);
    struct Message message = parse_message(incomingData);
    free(incomingData);
    return message;
}

struct Message parse_message(char *data)
{
    struct Message result = { ERROR };
    if (strcmp(data, "game_over\n") == 0) {
        return result.type = GAME_OVER, result;
    } else if (strcmp(data, "round\n") == 0) {
        return result.type = ROUND, result;
    } else if (strcmp(data, "yourturn\n") == 0) {
        return result.type = YOUR_TURN, result;
    } else if (matches_command(data, "ordered", 2)) {
        result.type = ORDEREDXV;
        result.parameters[0] = data[strlen(data) - 3];
        result.parameters[1] = data[strlen(data) - 2];
        return result;
    } else if (strcmp(data, "execute\n") == 0) {
        return result.type = EXECUTE, result;
    } else if (strcmp(data, "h?\n") == 0) {
        return result.type = HOPTION, result;
    } else if (strcmp(data, "s?\n") == 0) {
        return result.type = SOPTION, result;
    } else if (strcmp(data, "l?\n") == 0) {
        return result.type = LOPTION, result;
    } else if (matches_command(data, "hmove", 2)) {
        result.type = HMOVEXY;
        result.parameters[0] = data[strlen(data) - 3];
        result.parameters[1] = data[strlen(data) - 2];
        return result;
    } else if (matches_command(data, "vmove", 1)) {
        result.type = VMOVEX;
        result.parameters[0] = data[strlen(data) - 2];
        return result;
    } else if (matches_command(data, "long", 2)) {
        result.type = LONGXT;
        result.parameters[0] = data[strlen(data) - 3];
        result.parameters[1] = data[strlen(data) - 2];
        return result;
    } else if (matches_command(data, "short", 2)) {
        result.type = SHORTXT;
        result.parameters[0] = data[strlen(data) - 3];
        result.parameters[1] = data[strlen(data) - 2];
        return result;
    } else if (matches_command(data, "looted", 1)) {
        result.type = LOOTEDX;
        result.parameters[0] = data[strlen(data) - 2];
        return result;
    } else if (matches_command(data, "play", 1)) {
        result.type = PLAYX;
        result.parameters[0] = data[strlen(data) - 2];
        return result;
    }
    return result;
}

int send_message(struct Message message, FILE * file)
{
    char *buffer = malloc(BUFFER_SIZE * sizeof(char));
    memset(buffer, 0, BUFFER_SIZE * sizeof(char));
    encode_message(buffer, message);
    fputs(buffer, file);
    free(buffer);
    return fflush(file) == 0;
}

void encode_message(char *buffer, struct Message message)
{
    switch (message.type) {
        case GAME_OVER:
            strcpy(buffer, "game_over\n");
            break;

        case ROUND:
            strcpy(buffer, "round\n");
            break;

        case YOUR_TURN:
            strcpy(buffer, "yourturn\n");
            break;

        case ORDEREDXV:
            strcpy(buffer, "orderedXV\n");
            buffer[strlen(buffer) - 3] = message.parameters[0];
            buffer[strlen(buffer) - 2] = message.parameters[1];
            break;

        case EXECUTE:
            strcpy(buffer, "execute\n");
            break;

        case HOPTION:
            strcpy(buffer, "h?\n");
            break;

        case SOPTION:
            strcpy(buffer, "s?\n");
            break;

        case LOPTION:
            strcpy(buffer, "l?\n");
            break;

        default:
            // Handle error/misc cases
            break;
    }
}

char *read_line(FILE * file)
{
    char *result = malloc(BUFFER_SIZE * sizeof(char));
    memset(result, 0, BUFFER_SIZE);
    fgets(result, BUFFER_SIZE, file);
    return result;
}

int matches_command(char *message, char *command, int parameterCount)
{
    char *buffer = malloc(strlen(command) * sizeof(char));
    memset(buffer, 0, strlen(command) * sizeof(char));
    memcpy(buffer, message, strlen(command) * sizeof(char));
    if ((strncmp(buffer, command, strlen(command)) == 0)
        && (strlen(message) - 1) == (strlen(command) + parameterCount)) {
        free(buffer);
        return 1;
    }
    free(buffer);
    return 0;
}

action parse_action(char actionCharacter)
{
    actionCharacter = tolower(actionCharacter);
    switch (actionCharacter) {
        case 'h':
            return HORIZONTAL;

        case 'v':
            return VERTICAL;

        case '$':
            return LOOT;

        case 's':
            return SHORT;

        case 'l':
            return LONG;
    }
    return 0;
}

int is_numerical(char *string)
{
    for (int i = 0; i < strlen(string); ++i) {
        if (!isdigit(string[i])) {
            return 0;
        }
    }
    return 1;
}

int convert_player_character(char playerCharacter)
{
    // This function assumes use of ascii for character encoding
    if (playerCharacter >= 'A' && playerCharacter <= 'Z') {
        return playerCharacter - 'A';
    } else if (playerCharacter >= 'a' && playerCharacter <= 'z') {
        return playerCharacter - 'a';
    }
    return -1;
}

char convert_player_index(int playerIndex)
{
    if (playerIndex >= 0 && playerIndex <= 26) {
        return (char)('A' + playerIndex);
    }
    return '\0';
}
