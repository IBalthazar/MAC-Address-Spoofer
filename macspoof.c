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

void randomMAC(char arr[]);
void getMAC(char *netCard, char *oldMAC);
char *restoreMAC(char *netCard, char *MAC);
void interfaceChange(char *netCard, char *newMac);

int main(int argc, char *argv[])
{

    if (geteuid() != 0)
    {
        fprintf(stderr, "This program must be run as root.\n");
        return 1;
    }
    srand(time(NULL));
    int size = 100;
    int opt;

    char *netCard = NULL;
    int generate = 0;
    int restore = 0;
    char *oldMAC = (char *)malloc(100 * sizeof(char));
    char *newMAC = (char *)malloc(100 * sizeof(char));
    

    while((opt = getopt(argc, argv, "i:gr")) != -1){
        switch(opt){
            case 'i':
                netCard = optarg;
                break;
            case 'g':
            generate = 1;
                break;
            case 'r':     
                restore = 1;           
                break;
            case '?':
                fprintf(stderr, "Usage: %s -i <interface> [-g] [-r]\n", argv[0]);
                return 1;
        }
    }

    if (netCard == NULL)
    {
        fprintf(stderr, "Network interface not specified.\n");
        return 1;
    }
   
    else if (generate && restore)
    {
        fprintf(stderr, "Cannot generate and restore MAC address at the same time.\n");
        return 1;
    }
    else if (!generate && !restore)
    {
        fprintf(stderr, "No action specified. Use -g to generate a random MAC address or -r to restore the original MAC address.\n");
        return 1;
    }
     else if (generate){
        printf("Generating random MAC address...\n");
        randomMAC(newMAC);
        interfaceChange(netCard, newMAC);
    }
    else if (restore){
        printf("Restoring original MAC address...\n");
        getMAC(netCard, oldMAC);
        char *ogMAC = restoreMAC(netCard, oldMAC);
        interfaceChange(netCard, ogMAC);
    }
    else{
        fprintf(stderr, "Invalid options. Use -g to generate a random MAC address or -r to restore the original MAC address.\n");
        return 1;
    }
    free(oldMAC);
    free(newMAC);
    
}
void interfaceChange(char *netCard, char *newMAC){
    int size = 100;
    char *commandDown = (char *)malloc(100 * sizeof(char));
    char *commandChange = (char *)malloc(100 * sizeof(char));
    char *commandUp = (char *)malloc(100 * sizeof(char));

    snprintf(commandDown, size, "ifconfig %s down", netCard);
    snprintf(commandChange, size, "ifconfig %s hw ether %s", netCard, newMAC);
    snprintf(commandUp, size, "ifconfig %s up", netCard);
    system(commandDown);
    system(commandChange);
    system(commandUp);

    free(commandDown);
    free(commandChange);
    free(commandUp);
}
char *restoreMAC(char *netCard, char *MAC)
{
    struct ifreq ifr;
    struct ethtool_perm_addr *perm;
    perm = (struct ethtool_perm_addr *)malloc(sizeof(struct ethtool_perm_addr) + 6); // 6 bytes for MAC
    strncpy(ifr.ifr_name, netCard, IFNAMSIZ - 1);
    perm->cmd = ETHTOOL_GPERMADDR;
    perm->size = 6; // MAC address size
    ifr.ifr_data = (char *)perm;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (ioctl(sock, SIOCETHTOOL, &ifr) < 0)
    {
        perror("ioctl failed");
        close(sock);
        exit(1);
    }

    close(sock);
    unsigned char *mac = (unsigned char *)perm->data;
    snprintf(MAC, 100, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    free(perm);
    return MAC;
}

void getMAC(char *netCard, char *oldMAC)
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;
    strncpy(ifr.ifr_name, netCard, IFNAMSIZ - 1);
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0)
    {
        perror("ioctl failed");
        close(sock);
        exit(1);
    }

    close(sock);

    unsigned char *mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
    sprintf(oldMAC, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    //printf("Current MAC: %s\n", oldMAC);
}

void randomMAC(char arr[])
{
    unsigned char mac[6];
    for (int i = 0; i < 6; i++)
    {
        mac[i] = rand() % 256;
    }
    mac[0] = (mac[0] & 0xFE) | 0x02; // Set the locally administered bit and clear the multicast bit
    sprintf(arr, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
