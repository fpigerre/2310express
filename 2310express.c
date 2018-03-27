#include "2310express.h"

int main(int argc, char **argv)
{
    check_args(argc, argv);

    struct Game game =
        initialise_game((argc - 3), 0, atoi(argv[2]), atoi(argv[1]));

    char programCountStr[2];
    // snprintf means output string will be null-terminated
    snprintf(programCountStr, sizeof(programCountStr), "%d", argc - 3);

    char **programNames = malloc((argc - 3) * sizeof(char *));
    for (int i = 3; i < argc; ++i) {
        programNames[i - 3] = argv[i];
    }

    // For each player in the args list, initialise pipes and execute player programs
    struct Client *clients = malloc(game.totalPlayers * sizeof(struct Client));
    for (int i = 0; i < game.totalPlayers; ++i) {
        // Turn player number into string argument
        char playerNumber[2];
        sprintf(playerNumber, "%d", i);
        // Use null terminator for execve function
        char *programArgs[6] =
            { programNames[i], programCountStr, playerNumber, argv[2], argv[1],
            NULL
        };
        clients[i] = initialise_comms(programNames[i], programArgs);
        clients[i].playerNumber = i;
    }
    initialise_signal_handler();

    // Can continue hub logic here because of exec call and exit call
    begin_round(game.totalPlayers, clients, &game);

    // Synchronise with children
    synchronise(game.totalPlayers, clients);

    free(programNames);
    free(clients);
    return 0;
}

void check_args(int argc, char **argv)
{
    if (argc < 5 || ((argc - 3) > 26)) {
        fprintf(stderr, "Usage: hub seed width player player [player ...]\n");
        exit(1);
    }
    // Number of carriages
    if (!is_numerical(argv[2])) {
        fprintf(stderr, "Bad argument\n");
        exit(2);
    }
    // Seed must be an unsigned integer
    if (!is_numerical(argv[1]) || atoi(argv[1]) < 0) {
        fprintf(stderr, "Bad argument\n");
        exit(2);
    }
}

struct Client initialise_comms(char *programPath, char **const programArgs)
{
    struct Client client;
    int childStdin[2], childStdout[2];
    pipe(childStdin);
    pipe(childStdout);

    client.pid = fork();

    // TODO: More elegant error handling
    if (client.pid == -1) {
        fprintf(stderr, "Bad start\n");
        exit(3);
    }

    if (client.pid == 0) {
        // Prepare Parent -> Child pipe
        close(childStdin[WRITE_END]);
        dup2(childStdin[READ_END], STDIN_FILENO);
        close(childStdin[READ_END]);

        // Prepare Child -> Parent pipe
        close(childStdout[READ_END]);
        dup2(childStdout[WRITE_END], STDOUT_FILENO);
        close(childStdout[WRITE_END]);

        // Redirect stderr to /dev/null
        int nullDescriptor = open("/dev/null", O_WRONLY, O_NONBLOCK);
        if (nullDescriptor != -1) {
            dup2(nullDescriptor, STDERR_FILENO);
        }
        // Data written from parent should now go to child stdin
        // Exec relevant program
        if (execv(programPath, programArgs) == -1) {
            fprintf(stderr, "Bad start\n");
            exit(3);
        }
        exit(0);
    } else {
        // Close unused pipe ends
        close(childStdin[READ_END]);
        close(childStdout[WRITE_END]);

        client.writeStream = fdopen(childStdin[WRITE_END], "w");
        client.readStream = fdopen(childStdout[READ_END], "r");

        // Make sure player sends '!' on startup
        char *buffer = malloc(sizeof(char));
        int status = read(childStdout[READ_END], buffer, sizeof(char));
        if (strncmp(buffer, "!", 1) != 0 || status != sizeof(char)) {
            fprintf(stderr, "Bad start\n");
            exit(3);
        }
    }
    return client;
}

void handle_sigchld(int status)
{
    fprintf(stderr, "Client disconnected\n");
    exit(4);
}

void initialise_signal_handler(void)
{
    // Initialise signal handler
    struct sigaction sa;
    // Zero sigaction struct so that valgrind doesn't whinge
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = handle_sigchld;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, 0);
}

void synchronise(int programCount, struct Client *clients)
{
    int childStatus;
    for (int i = 0; i < programCount; ++i) {
        waitpid(clients[i].pid, &childStatus, 0);
        if (WIFEXITED(childStatus)) {
            int status = WEXITSTATUS(childStatus);
            if (status != 0) {
                fprintf(stderr, "Player %d ended with status %d\n", i, status);
            }
        } else if (WIFSIGNALED(childStatus)) {
            fprintf(stderr, "Player %d shutdown after receiving signal %d\n",
                    i, WTERMSIG(childStatus));
        }
    }
}
