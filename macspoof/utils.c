#include "utils.h"

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

bool isValidMAC(char *MAC)
{

    unsigned char mac[6];
    if (strlen(MAC) != 17)
    {
        fprintf(stderr, "Invalid MAC Address");
        return false;
    }
    else if (sscanf(MAC, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) != 6)
    {
        fprintf(stderr, "Invalid MAC Address");
        return false;
    }
    else if ((mac[0] & 0x01) != 0)
    {
        fprintf(stderr, "Invalid MAC address. The multicast bit must be 0.\n");
        return false;
    }
    return true;
}