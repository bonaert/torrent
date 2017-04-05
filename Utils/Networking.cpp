#include <iostream>
#include "Networking.hpp"

// Receive

int receive_data(int socket_fd, void *message, int length) {
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

int get_data_from_socket(int socket_fd, char *buffer, int size) {
    int errorCode = receive_data(socket_fd, buffer, size);
    if (errorCode <= -1) {
        perror("Receive - message data");
        std::cout << "The size of the message is " << size << std::endl;
        return -1;
    }

    return size;
}

int get_message_length(int socket_fd) {
    /*
     * Tries to get the length of the message
     * If can't read from the socket, throws an error
     * If the socket is closed, returns -1
     * Otherwise, returns the length of the message
     * */
    size_t length = 0;
    ssize_t length_bytes_read = receive_data(socket_fd, &length, sizeof(length));

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
int receive_message(int socket_fd, char *buffer) {
    int length = get_message_length(socket_fd);  // Gets the length
    if (length == -1) { // Socket fermé
        return -1;
    }

    return get_data_from_socket(socket_fd, buffer, length);  // Gets the data
}

/*
 * Tries to receive from the socket, but stops after a timeout.
 * Returns True if the read was succesful.
 * Returns False otherwise.
 */
bool receive_message_with_timeout(int socket_fd, char *buffer, int timeout_val) {
    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(socket_fd, &set);

    timeout.tv_sec = timeout_val;
    timeout.tv_usec = 0;

    bool success = false;

    int num_ready_descriptors = select(socket_fd + 1, &set, NULL, NULL, &timeout);
    if (num_ready_descriptors > 0) {  // There is a readable socket
        int num_bytes_read = receive_message(socket_fd, buffer);

        if (num_bytes_read > 0) {
            success = true;
        }
    }
    return success;
}

// Send

int send_data(int socket_fd, char *buffer, int length) {
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

int send_message(int socket_fd, const char *message) {
    size_t length = strlen(message) + 1;
    //std::cout << "Sending message of size (including \\0) of " << length << " bytes" << std::endl;
    //std::cout << "Message: " << message << "to" << socket_fd <<  std::endl;
    if (send_data(socket_fd, (char *) &length, sizeof(length)) <= -1) {
        perror("Send message - Message length");
        return -1;
    } // Send the length
    if (send_data(socket_fd, (char *) message, (int) length) <= -1) {
        perror("Send message - Message data");
        return -1;
    } // Send the data
    return (int) length;
}

int init_connection_to_server(char *server_ip_address, int port) {
    struct hostent *he;
    int serv_socket;
    if ((he = gethostbyname(server_ip_address)) == NULL) {
        perror("gethostbyname");
        exit(1);
    }

    serv_socket = create_socket();
    if (connect_to_server(serv_socket, port, he) <= -1) {
        const std::string &message = "The connect to the server with port "
                                     + std::to_string(port) + " has failed:";
        perror(message.c_str());
        return -1;
    }
    return serv_socket;
}

int create_socket() {
    int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    return sockfd;

}

/*
 * Returns the socket_fd created by the connect call.
 */
int connect_to_server(int socket, int port, struct hostent *addr) {
    struct sockaddr_in their_addr;

    their_addr.sin_family = AF_INET;    // host byte order
    their_addr.sin_port = htons(port);  // short, network byte order
    their_addr.sin_addr = *((struct in_addr *) addr->h_addr);

    memset(&(their_addr.sin_zero), '\0', 8);  // zero the rest of the struct

    return connect(socket, (struct sockaddr *) &their_addr, sizeof(struct sockaddr));
}
