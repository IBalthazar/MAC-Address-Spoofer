#include "network.h"

void interfaceChange(char *netCard, char *newMAC)
{
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

void getMAC(char *netCard, char *currentMAC)
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
    sprintf(currentMAC, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}