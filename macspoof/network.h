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

void getMAC(char *netCard, char *currentMAC);
char *restoreMAC(char *netCard, char *MAC);
void interfaceChange(char *netCard, char *newMac);