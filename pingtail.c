#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Define a struct to pass parameters to the thread function
struct ThreadArgs {
    char baseIP[20];
    int startRange;
    int endRange;
};

#ifdef _WIN32
    // ANSI color codes for Windows in ConEmu
    #define GREEN ""
    #define BLUE ""
    #define RESET ""
#else
    // ANSI color codes for Unix-like systems (Linux, macOS)
    #define GREEN "\033[32m"
    #define BLUE "\033[34m"
    #define RESET "\033[0m"
#endif

// Function to ping an IP address and print the result with color
void *pingAndPrint(void *args) {
    struct ThreadArgs *targs = (struct ThreadArgs *)args;

    for (int i = targs->startRange; i <= targs->endRange; ++i) {
        char ip[20];
        snprintf(ip, sizeof(ip), "%s%d", targs->baseIP, i);
        char pingCommand[100];

        // Use "ping" for Windows, or "ping" for Unix-like systems
        #ifdef _WIN32
            snprintf(pingCommand, sizeof(pingCommand), "ping %s -n 1 > NUL 2>&1", ip);
        #else
            snprintf(pingCommand, sizeof(pingCommand), "ping -c 1 %s > /dev/null 2>&1", ip);
        #endif

        int result = system(pingCommand);

        if (result == 0) {
            printf("%s%s is responding to ICMP.%s\n", GREEN, ip, RESET);
        }
    }

    return NULL;
}

int main() {
    char baseIP[20] = "10.0.1.";
    int startRange = 1;
    int endRange = 254;

    printf("Enter the base IP (default: 10.0.1.): ");
    char input[20];
    if (fgets(input, sizeof(input), stdin)) {
        if (strlen(input) > 1) {
            input[strcspn(input, "\n")] = '\0'; // Remove the newline character
            strcpy(baseIP, input);
        }
    }

    printf("Enter the starting IP range (default: 1): ");
    if (fgets(input, sizeof(input), stdin)) {
        if (strlen(input) > 1) {
            startRange = atoi(input);
        }
    }

    printf("Enter the ending IP range (default: 254): ");
    if (fgets(input, sizeof(input), stdin)) {
        if (strlen(input) > 1) {
            endRange = atoi(input);
        }
    }

    printf("%sPinging all at once...%s\n", GREEN, RESET);

    int numThreads = endRange - startRange + 1;
    pthread_t threads[numThreads];
    struct ThreadArgs targs[numThreads];

    for (int i = 0; i < numThreads; ++i) {
        targs[i].startRange = startRange + i;
        targs[i].endRange = startRange + i;
        strcpy(targs[i].baseIP, baseIP);
        pthread_create(&threads[i], NULL, pingAndPrint, &targs[i]);
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("Press Enter to exit...");
    getchar();  // Wait for Enter key press

    return 0;
}
