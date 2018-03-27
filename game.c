#include "game.h"

struct Game initialise_game(int totalPlayers, int playerNumber,
                            int carriageCount, int seed)
{
    struct Game *game = malloc(sizeof(struct Game));
    memset(game, 0, sizeof(struct Game));

    game->totalPlayers = totalPlayers;
    game->playerNumber = playerNumber;
    game->carriageCount = carriageCount;
    game->seed = seed;
    game->isRunning = 1;

    initialise_loot(game);
    initialise_players(game);
    return *game;
}

void initialise_loot(struct Game *game)
{
    game->totalLoot = ((game->seed % 4) + 1) * game->carriageCount;
    // TODO: What if malloc fails?
    game->loot = malloc(2 * sizeof(int *));
    for (int i = 0; i < 2; ++i) {
        game->loot[i] = malloc(game->carriageCount * sizeof(int));
        memset(game->loot[i], 0, game->carriageCount * sizeof(int));
    }

    // Set the initial piece of loot
    game->loot[0][(int)ceil(game->carriageCount / 2)] = 1;

    int n = 0, x, y;
    for (int i = 0; i < game->totalLoot; ++i) {
        // A recursive function could be used here, but a direct solution seems more efficient
        // The recurrence relation given in the spec denotes an arithemetic sequence
        // Therefore we can define a general formula as follows
        x = ((int)ceil(game->carriageCount / 2) +
             n * (game->seed % 101)) % game->carriageCount;
        y = (n * (game->seed % 2)) % 2;

        // Can increment here due to memset when initialising game.loot
        game->loot[y][x]++;
        n++;
    }
}

void initialise_players(struct Game *game)
{
    // Initialise player data structure
    game->players = malloc(game->totalPlayers * sizeof(struct Player));
    memset(game->players, 0, game->totalPlayers * sizeof(struct Player));

    for (int i = 0; i < game->totalPlayers; ++i) {
        game->players[i].x = i % game->carriageCount;
    }
}
