#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/if.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>

#include "utils.h"
#include "network.h"


int main(int argc, char *argv[])
{

    if (geteuid() != 0)
    {
        fprintf(stderr, "This program must be run as root.\n");
        return 1;
    }
    srand(time(NULL));
    
    int opt;

    char *netCard = NULL;
    int customMAC = 0;
    int generate = 0;
    int restore = 0;
    int show = 0;

    char *currentMAC = (char *)malloc(100 * sizeof(char));
    char *newMAC = (char *)malloc(100 * sizeof(char));

    while ((opt = getopt(argc, argv, "i:c:grs")) != -1)
    {
        switch (opt)
        {
        case 'i':
            netCard = optarg;
            break;
        case 'c':
            printf("%s\n", optarg);
            printf("%s\n", newMAC);
            strncpy(newMAC, optarg, 17);
            newMAC[17] = '\0';
            customMAC = 1;
            break;
        case 'g':
            generate = 1;
            break;
        case 'r':
            restore = 1;
            break;
        case 's':
            show = 1;
            break;
        case '?':
            fprintf(stderr, "Usage: %s -i <interface> [-c <new_mac>] [-g] [-r] [-s]\n", argv[0]);
            return 1;
        }
    }

    if (netCard == NULL)
    {
        fprintf(stderr, "Network interface not specified.\n");
        return 1;
    }
    else if (customMAC)
    {
        printf("%s\n", newMAC);
        printf("%s\n", newMAC);
        if (isValidMAC(newMAC) == true)
        {
            printf("Changing MAC address to %s...\n", newMAC);
            interfaceChange(netCard, newMAC);
        }
    }
    else if (generate && restore)
    {
        fprintf(stderr, "Cannot generate and restore MAC address at the same time.\n");
        return 1;
    }
    else if (!generate && !restore && !show)
    {
        fprintf(stderr, "No action specified. Use -g to generate a random MAC address or -r to restore the original MAC address.\n");
        return 1;
    }
    else if (generate)
    {
        printf("Generating random MAC address...\n");
        randomMAC(newMAC);
        interfaceChange(netCard, newMAC);
    }
    else if (restore)
    {
        printf("Restoring original MAC address...\n");
        getMAC(netCard, currentMAC);
        char *ogMAC = restoreMAC(netCard, currentMAC);
        interfaceChange(netCard, ogMAC);
    }
    else if (show)
    {
        getMAC(netCard, currentMAC);
        printf("Current MAC Address is: %s\n", currentMAC);
    }
    else
    {
        fprintf(stderr, "Invalid options. Use -g to generate a random MAC address or -r to restore the original MAC address.\n");
        return 1;
    }
    free(currentMAC);
    free(newMAC);
}
