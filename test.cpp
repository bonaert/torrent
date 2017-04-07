#include <iostream>
#include "Tracker.hpp"

void setup() {
    srand((unsigned int) time(NULL));
}

int main() {
    setup();

    ConnectRequest req1;
    ConnectRequest req2;
    std::cout << req1.protocolID << " " << req1.action << " " << req1.transactionID << std::endl;
    std::cout << req2.protocolID << " " << req2.action << " " << req2.transactionID << std::endl;

    return 0;
}