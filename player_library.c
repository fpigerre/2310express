#include "player_library.h"

void check_args(int argc, char **argv)
{
    if (argc != 5) {
        fprintf(stderr, "Usage: player pcount myid width seed\n");
        exit(1);
    }

    if (!is_numerical(argv[1]) || atoi(argv[1]) < 2 || atoi(argv[1]) > 26) {
        fprintf(stderr, "Invalid player count\n");
        exit(2);
    }
    // Player ID must be an unsigned integer with player count bounds
    if (!is_numerical(argv[2]) || atoi(argv[2]) < 0
        || atoi(argv[2]) > (atoi(argv[1]) - 1)) {
        fprintf(stderr, "Invalid player ID\n");
        exit(3);
    }

    if (!is_numerical(argv[3]) || atoi(argv[3]) < 2) {
        fprintf(stderr, "Invalid width\n");
        exit(4);
    }
    // Seed must be an unsigned integer
    if (!is_numerical(argv[4]) || atoi(argv[4]) < 0) {
        fprintf(stderr, "Invalid seed\n");
        exit(5);
    }
}

void handle_sigpipe(int status)
{
    fprintf(stderr, "Communication error\n");
    exit(6);
}

void initialise_signal_handler(void)
{
    // Initialise signal handler
    struct sigaction sa;
    // Zero sigaction struct so that valgrind doesn't whinge
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = handle_sigpipe;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGPIPE, &sa, 0);
}

void handle_generic_actions(struct Message currentMessage, struct Game *game)
{
    switch (currentMessage.type) {
        case GAME_OVER:
            game->isRunning = 0;
            game->roundActive = 0;
            break;

        case ROUND:
            // Make round active in game struct
            game->roundActive = 1;
            break;

        case ORDEREDXV:
            fprintf(stderr, "%c ordered %c\n", currentMessage.parameters[0],
                    currentMessage.parameters[1]);
            break;
            /*
               case EXECUTE:
               // Execute here
               break; */

        case HMOVEXY:{
                int playerNumber =
                    convert_player_character(currentMessage.parameters[0]);
                if (currentMessage.parameters[0] == '+') {
                    if (game->players[playerNumber].x < game->carriageCount) {
                        game->players[playerNumber].x++;
                    }
                } else if (currentMessage.parameters[0] == '-') {
                    if (game->players[playerNumber].x > 0) {
                        game->players[playerNumber].x--;
                    }
                }
                fprintf(stderr, "%c moved to %d/%d\n",
                        currentMessage.parameters[0],
                        game->players[playerNumber].x,
                        game->players[playerNumber].y);
                break;
            }

        case VMOVEX:{
                int playerNumber =
                    convert_player_character(currentMessage.parameters[0]);
                if (game->players[playerNumber].y == 0) {
                    game->players[playerNumber].y = 1;
                } else {
                    game->players[playerNumber].y = 0;
                }
                fprintf(stderr, "%c moved to %d/%d\n",
                        currentMessage.parameters[0],
                        game->players[playerNumber].x,
                        game->players[playerNumber].y);
                break;
            }

        case LONGXT:{
                int player =
                    convert_player_character(currentMessage.parameters[0]);
                int target =
                    convert_player_character(currentMessage.parameters[1]);
                // Is the player able to hit the target on the lower level?
                if (game->players[player].y == 0) {
                    if (game->players[target].y == 0
                        && game->players[target].x !=
                        game->players[player].x) {
                        game->players[target].hits++;
                        fprintf(stderr, "%c targets %c who has %d hits\n",
                                currentMessage.parameters[0],
                                currentMessage.parameters[1],
                                game->players[target].hits);
                        break;
                    }
                    // Is the player able to hit the player on the upper level without any obstacles?
                } else if (game->players[target].y == 1
                           && game->players[target].x !=
                           game->players[player].x) {
                    int playerCarriage = game->players[player].x;
                    int targetCarriage = game->players[target].x;
                    int playerBlocking = 0;
                    for (int i = 0; i < game->totalPlayers; ++i) {
                        if (game->players[i].y == 1) {
                            if (playerCarriage > targetCarriage) {
                                if (game->players[i].x > playerCarriage
                                    && game->players[i].x < targetCarriage) {
                                    playerBlocking = 1;
                                }
                            } else {
                                if (game->players[i].x < playerCarriage
                                    && game->players[i].x > targetCarriage) {
                                    playerBlocking = 1;
                                }
                            }
                        }
                        if (!playerBlocking) {
                            game->players[target].hits++;
                            fprintf(stderr, "%c targets %c who has %d hits\n",
                                    currentMessage.parameters[0],
                                    currentMessage.parameters[1],
                                    game->players[target].hits);
                            break;
                        }
                    }
                }
                fprintf(stderr, "%c has no target\n",
                        currentMessage.parameters[0]);
                break;
            }

        default:
            // Handle error/misc cases
            break;
    }
}
