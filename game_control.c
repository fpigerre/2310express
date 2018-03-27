#include "game_control.h"

void broadcast_message(struct Message message, struct Client *clients,
                       int playerCount)
{
    for (int i = 0; i < playerCount; ++i) {
        send_message(message, clients[i].writeStream);
    }
}

int begin_round(int playerCount, struct Client *clients, struct Game *game)
{
    int *roundActions = malloc(playerCount * sizeof(int));
    memset(roundActions, 0, playerCount);

    // Start a new round
    struct Message roundMessage = { ROUND };
    broadcast_message(roundMessage, clients, playerCount);

    for (int i = 0; i < playerCount; ++i) {
        // Send your turn message
        struct Message turnMessage = { YOUR_TURN };
        send_message(turnMessage, clients[i].writeStream);

        // Await response
        struct Message responseMessage =
            receive_message(clients[i].readStream);

        // Check client submitted action
        if (responseMessage.type == PLAYX) {
            struct Message command = { ORDEREDXV, {convert_player_index(i),
                                                   responseMessage.parameters
                                                   [0]}
            };
            // Broadcast orderedXV message
            broadcast_message(command, clients, playerCount);
            // Save round action
            roundActions[i] = parse_action(responseMessage.parameters[0]);
        } else {
            return 1;
        }
    }

    execute_actions(playerCount, clients, game, roundActions);

    free(roundActions);
    return 0;
}

void execute_actions(int playerCount, struct Client *clients,
                     struct Game *game, int *roundActions)
{
    // Execute all actions
    struct Message executeMessage = { EXECUTE };
    broadcast_message(executeMessage, clients, playerCount);

    for (int i = 0; i < playerCount; ++i) {
        action thisAction = roundActions[i];

        switch (thisAction) {
            case HORIZONTAL:
                broadcast_message(execute_horizontal(&clients[i], game),
                                  clients, playerCount);
                break;

            case VERTICAL:
                broadcast_message(execute_vertical(&clients[i], game), clients,
                                  playerCount);
                break;

            case LOOT:{
                    struct Message playerUpdate =
                        execute_loot(&clients[i], game);
                    if (playerUpdate.parameters[0]) {
                        broadcast_message(playerUpdate, clients, playerCount);
                    }
                    break;
                }

            case SHORT:
                broadcast_message(execute_short(&clients[i], game), clients,
                                  playerCount);
                struct Message playerUpdate = execute_short(&clients[i], game);
                if (playerUpdate.parameters[0] && playerUpdate.parameters[1]) {
                    broadcast_message(playerUpdate, clients, playerCount);
                }
                break;

            case LONG:
                break;

            default:
                // If player tries to send DRY_OUT
                fprintf(stderr, "Protocol error by client\n");
                exit(5);
                break;
        }
    }
}

struct Message execute_horizontal(struct Client *client, struct Game *game)
{
    struct Message prompt = { HOPTION };
    send_message(prompt, client->writeStream);
    struct Message responseMessage = receive_message(client->readStream);

    if (responseMessage.type == SIDEWAYSX) {
        // Execute data structure changes
        if (responseMessage.parameters[0] == '+') {
            if (game->players[client->playerNumber].x < game->carriageCount) {
                game->players[client->playerNumber].x++;
            } else {
                fprintf(stderr, "Illegal move by client\n");
                exit(6);
            }
        } else if (responseMessage.parameters[0] == '-') {
            if (game->players[client->playerNumber].x > 0) {
                game->players[client->playerNumber].x--;
            } else {
                fprintf(stderr, "Illegal move by client\n");
                exit(6);
            }
        } else {
            // Assuming invalid character == protocol error
            fprintf(stderr, "Protocol error by client\n");
            exit(5);
        }
        struct Message playerUpdate =
            { HMOVEXY, {convert_player_index(client->playerNumber),
                        responseMessage.parameters[0]}
        };
        return playerUpdate;

    } else {
        fprintf(stderr, "Protocol error by client\n");
        exit(5);
    }
}

struct Message execute_vertical(struct Client *client, struct Game *game)
{
    if (game->players[client->playerNumber].y == 0) {
        game->players[client->playerNumber].y = 1;
    } else {
        game->players[client->playerNumber].y = 0;
    }
    struct Message playerUpdate =
        { VMOVEX, {convert_player_index(client->playerNumber)} };
    return playerUpdate;
}

struct Message execute_loot(struct Client *client, struct Game *game)
{
    struct Player playerLocation = game->players[client->playerNumber];
    struct Message playerUpdate = { LOOTEDX };
    if (game->loot[playerLocation.y][playerLocation.x] > 0) {
        game->loot[playerLocation.y][playerLocation.x]--;
        playerUpdate.parameters[0] =
            convert_player_index(client->playerNumber);
    }
    return playerUpdate;
}

struct Message execute_short(struct Client *client, struct Game *game)
{
    struct Message prompt = { SOPTION };
    send_message(prompt, client->writeStream);
    struct Message responseMessage = receive_message(client->readStream);

    struct Message playerUpdate = { SHORTXT };

    if (responseMessage.type == TARGET_SHORTX) {
        // Execute data structure changes
        int targetNumber =
            convert_player_character(responseMessage.parameters[0]);
        if (targetNumber != -1) {
            struct Player player = game->players[client->playerNumber];
            struct Player target = game->players[targetNumber];

            if (player.x == target.x && player.y == target.y) {
                if (target.loot > 0) {
                    target.loot--;
                    game->loot[target.y][target.x]++;

                    playerUpdate.parameters[0] =
                        convert_player_index(client->playerNumber);
                    playerUpdate.parameters[1] = responseMessage.parameters[0];
                    return playerUpdate;
                }
            }
        }
    } else {
        fprintf(stderr, "Protocol error by client\n");
        exit(5);
    }
    return playerUpdate;
}
