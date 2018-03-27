#include "spoiler.h"

int main(int argc, char **argv)
{
    initialise_signal_handler();
    check_args(argc, argv);

    struct Game game =
        initialise_game(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]),
                        atoi(argv[4]));
    char lastDirection = '-';

    fprintf(stdout, "!");
    fflush(stdout);

    while (game.isRunning) {
        struct Message currentMessage = receive_message(stdin);
        handle_generic_actions(currentMessage, &game);

        if (currentMessage.type == YOUR_TURN) {
            // Execute round actions
            struct Player myLocation = game.players[game.playerNumber];

            // Loot ground if there is loot
            if (game.loot[myLocation.y][myLocation.x] > 0) {
                game.loot[myLocation.y][myLocation.x]--;
                game.players[game.playerNumber].loot++;
                fprintf(stdout, "play$\n");
                fflush(stdout);
            } else {
                // Otherwise, move left first then right
                if (myLocation.x == 0) {
                    lastDirection = '+';
                } else if (myLocation.x == game.carriageCount) {
                    lastDirection = '-';
                }
                fprintf(stdout, "playh\n");
            }
        } else if (currentMessage.type == HOPTION) {
            fprintf(stdout, "sideways%c\n", lastDirection);
        }
    }

    free(game.loot);
    free(game.players);
    return 0;
}
