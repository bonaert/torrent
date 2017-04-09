#include <iostream>
#include <fcntl.h>
#include "Networking.hpp"
#include "Tools.hpp"

// Receive

int receiveData(int socket_fd, void *message, int length) {
    int bytes_read = 0;
    int bytes_to_read = length;
    char *pointer = (char *) message;
    while (bytes_to_read > 0) {
        ssize_t data_bytes_read = recv(socket_fd, pointer, (size_t) bytes_to_read, 0);
        bytes_read += (int) data_bytes_read;
        bytes_to_read -= (int) data_bytes_read;
        pointer += data_bytes_read;

        if (data_bytes_read <= -1) {
            return -1;
        }
    }

    return bytes_read;
}

int getDataFromSocket(int socket_fd, char *buffer, int size) {
    int errorCode = receiveData(socket_fd, buffer, size);
    if (errorCode <= -1) {
        perror("Receive - message data");
        std::cout << "The size of the message is " << size << std::endl;
        return -1;
    }

    return size;
}

int getMessageLength(int socket_fd) {
    /*
     * Tries to get the length of the message
     * If can't read from the socket, throws an error
     * If the socket is closed, returns -1
     * Otherwise, returns the length of the message
     * */
    size_t length = 0;
    ssize_t length_bytes_read = receiveData(socket_fd, &length, sizeof(length));

    if (length_bytes_read <= -1) {
        perror("receive_message - message length");
        return -1;
    } else if (length_bytes_read == 0) {
        // Le client a fermé le socket
        return -1;
    }
    return (int) (length * sizeof(char));
}

/*
 * Reads from the socket and puts the result in the buffer.
 * If the socket is closed, doesn't modify the buffer and returns -1.
 * Otherwise, puts the message in the buffer and return the length of the message.
 */
int receiveMessage(int socket_fd, char *buffer) {
    int length = getMessageLength(socket_fd);  // Gets the length
    if (length == -1) { // Socket fermé
        return -1;
    }

    return getDataFromSocket(socket_fd, buffer, length);  // Gets the data
}

/*
 * Tries to receive from the socket, but stops after a timeout.
 * Returns True if the read was succesful.
 * Returns False otherwise.
 */
bool receiveMessageWithTimeout(int socket_fd, char *buffer, int timeout_val) {
    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(socket_fd, &set);

    timeout.tv_sec = timeout_val;
    timeout.tv_usec = 0;

    bool success = false;

    int num_ready_descriptors = select(socket_fd + 1, &set, NULL, NULL, &timeout);
    if (num_ready_descriptors > 0) {  // There is a readable socket
        int num_bytes_read = receiveMessage(socket_fd, buffer);

        if (num_bytes_read > 0) {
            success = true;
        }
    }
    return success;
}

// Send

int sendData(int socket_fd, const char *buffer, int length) {
    int bytes_sent = 0;
    int bytes_to_send = length;
    while (bytes_to_send > 0) {
        ssize_t data_bytes_sent = send(socket_fd, buffer, (size_t) bytes_to_send, MSG_NOSIGNAL);
        bytes_sent += (int) data_bytes_sent;
        bytes_to_send -= (int) data_bytes_sent;
        buffer += data_bytes_sent;

        if (data_bytes_sent <= -1) {
            return -1;
        }
    }

    return bytes_sent;
}

int sendMessage(int socket_fd, const char *message) {
    size_t length = strlen(message) + 1;
    //std::cout << "Sending message of size (including \\0) of " << length << " bytes" << std::endl;
    //std::cout << "Message: " << message << "to" << socket_fd <<  std::endl;
    if (sendData(socket_fd, (char *) &length, sizeof(length)) <= -1) {
        perror("Send message - Message length");
        return -1;
    } // Send the length
    if (sendData(socket_fd, (char *) message, (int) length) <= -1) {
        perror("Send message - Message data");
        return -1;
    } // Send the data
    return (int) length;
}

int initConnectionToServer(char *server_ip_address, int port) {
    struct hostent *he;
    int serv_socket;
    if ((he = gethostbyname(server_ip_address)) == NULL) {
        perror("gethostbyname");
        exit(1);
    }

    serv_socket = createTCPSocket();
    if (connectToServer(serv_socket, port, he) <= -1) {
        const std::string &message = "The connect to the server with port "
                                     + std::to_string(port) + " has failed:";
        perror(message.c_str());
        return -1;
    }
    return serv_socket;
}


int initConnectionToServer(uint32_t ip, int port, int timeout) {
    int socket = createTCPSocket();
    if (!connectToServer(socket, port, ip, timeout)) {
        const std::string &message = "The connect to the host with port "
                                     + std::to_string(port)
                                     + " and ip address "
                                     + getHumanReadableIP(ip)
                                     + " has failed";
        perror(message.c_str());
        close(socket);
        return -1;
    }
    return socket;
}

/*
 * SOCKETS
 */

int createTCPSocket() {
    int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    return sockfd;
}

int createUDPSocket(int sourcePort, int timeout) {
    int socketFd;

    if ((socketFd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Couldn't create UDP socket");
        exit(1);
    }

    struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    if (setsockopt(socketFd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("Error");
    }

    struct sockaddr_in sourceAddress;
    sourceAddress.sin_family = AF_INET;
    sourceAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    sourceAddress.sin_port = htons((uint16_t) sourcePort);
    bzero(&sourceAddress.sin_zero, 8);

    if (bind(socketFd, (struct sockaddr *) &sourceAddress, sizeof(sourceAddress)) < 0) {
        std::string errorMessage = "Couldn't bind to port " + sourcePort;
        perror(errorMessage.c_str());
        exit(1);
    }


    return socketFd;
}


void initAddress(struct sockaddr_in *address, int port, struct hostent *addr) {
    address->sin_family = AF_INET;    // host byte order
    address->sin_port = htons((uint16_t) port);  // short, network byte order
    address->sin_addr = *((struct in_addr *) addr->h_addr);

    bzero(&(address->sin_zero), 8);  // zero the rest of the struct
}

void initAddress(struct sockaddr_in *address, int port, uint32_t ip) {
    struct in_addr inAddress;
    inAddress.s_addr = ip;

    address->sin_family = AF_INET;    // host byte order
    address->sin_port = htons((uint16_t) port);  // short, network byte order
    address->sin_addr = inAddress;
    bzero(&(address->sin_zero), 8);  // zero the rest of the struct
}


struct sockaddr_in *buildAddress(const std::string &name, int port) {
    struct hostent *host = gethostbyname(name.c_str());
    if (host == nullptr) {
        std::string string = "Couldn't find host '" + name + "'";
        herror(string.c_str());
        return nullptr;
    }

    struct sockaddr_in *address = new sockaddr_in;
    initAddress(address, port, host);

    return address;
}


void setSocketAsNonBlocking(int socket);

void setSocketAsBlocking(int socket);

/*
 * Returns the socket_fd created by the connect call.
 */
int connectToServer(int socket, int port, struct hostent *addr) {
    struct sockaddr_in their_addr;
    initAddress(&their_addr, port, addr);
    return connect(socket, (struct sockaddr *) &their_addr, sizeof(struct sockaddr));
}

int connectToServer(int socket, int port, uint32_t ip) {
    struct sockaddr_in their_addr;
    initAddress(&their_addr, port, ip);
    return connect(socket, (struct sockaddr *) &their_addr, sizeof(struct sockaddr));
}

bool connectToServer(int socket, int port, uint32_t ip, int timeout) {
    struct sockaddr_in their_addr;
    struct timeval tv;
    fd_set myset;
    socklen_t lon;
    int valopt;

    initAddress(&their_addr, port, ip);



    // Source: http://developerweb.net/viewtopic.php?id=3196
    setSocketAsNonBlocking(socket);

    // Trying to connect with timeout
    std::cout << "Attempting to connect to IP " << getHumanReadableIP(ip)
              << " at port " << port << std::endl;
    int res = connect(socket, (struct sockaddr *) &their_addr, sizeof(their_addr));

    if (res < 0) {
        if (errno != EINPROGRESS) {
            fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
            return false;
        }

        tv.tv_sec = timeout;
        tv.tv_usec = 0;
        FD_ZERO(&myset);
        FD_SET(socket, &myset);
        int code = select(socket + 1, NULL, &myset, NULL, &tv);
        if (code > 0) {
            lon = sizeof(int);
            getsockopt(socket, SOL_SOCKET, SO_ERROR, (void *) (&valopt), &lon);
            if (valopt) {
                fprintf(stderr, "Error in connection() %d - %s\n", valopt, strerror(valopt));
                setSocketAsBlocking(socket);
                return false;
            }
        } else {
            lon = sizeof(int);
            getsockopt(socket, SOL_SOCKET, SO_ERROR, (void *) (&valopt), &lon);
            fprintf(stderr, "Timeout or error() %d - %s\n", valopt, strerror(valopt));
            setSocketAsBlocking(socket);
            return false;
        }

    }
    setSocketAsBlocking(socket);
    return true;
}

void setSocketAsBlocking(int socket) {
    long arg = fcntl(socket, F_GETFL, NULL);
    arg &= (~O_NONBLOCK);
    fcntl(socket, F_SETFL, arg);
}

void setSocketAsNonBlocking(int socket) {
    long arg = fcntl(socket, F_GETFL, NULL);
    arg |= O_NONBLOCK;
    fcntl(socket, F_SETFL, arg);
}


/*
 * UTILITIES
 */

int getPortFromUrl(const std::string &url) {
    int index = (int) url.find(':', 7); // Skip the udp:// part of the url
    return index == -1 ? 80 : readInt(url, index + 1);
}

std::string getDomainFromUrl(const std::string &url) {
    // Finding stuff to eliminate at the end (searches for port and /announce)
    int index = (int) url.find(':', 7);
    if (index == -1) {
        index = (int) url.find('/', 7);
    }


    if (index == -1) {
        return url.substr(6); // Remove the "udp://" part
    } else {
        // Remove the "udp://" part and the useless stuff (port, /announce) at the end.
        return url.substr(6, (unsigned long) (index - 6));
    }
}

std::string getHumanReadableIP(uint32_t ip) {
    struct in_addr ip_addr;
    ip_addr.s_addr = ip;
    return inet_ntoa(ip_addr);
};
