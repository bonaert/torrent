#ifndef PROJET_NETWORKING_H
#define PROJET_NETWORKING_H

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstdlib>

int initConnectionToServer(char *server_ip_address, int port);

int initConnectionToServer(uint32_t ip, int port);

int createTCPSocket();

int createUDPSocket(int sourcePort, int timeout = 3);

int connectToServer(int socket, int port, struct hostent *addr);

int connectToServer(int socket, int port, uint32_t ip);

int receiveData(int socket_fd, void *message, int length);

int getDataFromSocket(int socket_fd, char *buffer, int size);

int getMessageLength(int socket_fd);

int receiveMessage(int socket_fd, char *buffer);

bool receiveMessageWithTimeout(int socket_fd, char *buffer, int timeout);

int sendData(int socket_fd, char *buffer, int length);

int sendMessage(int socket_fd, const char *message);

void initAddress(struct sockaddr_in *address, int port, struct hostent *addr);

void initAddress(struct sockaddr_in *address, int port, uint32_t ip);



sockaddr_in *buildAddress(const std::string &name, int port);


/*
 * UTILITIES
 */

int getPortFromUrl(const std::string &url);

std::string getDomainFromUrl(const std::string &url);

std::string getHumanReadableIP(uint32_t ip);

#endif //PROJET_NETWORKING_H
