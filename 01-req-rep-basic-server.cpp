#include <zmq.hpp>
#include <string>
#include <iostream>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n)	Sleep(n)
#endif

int main () {
    
    // socket
    zmq::context_t context (1); 
    zmq::socket_t socket (context, zmq::socket_type::rep);
    socket.bind ("tcp://*:5555"); // port number

    while (true) {
        zmq::message_t request;
        
        // recv
        socket.recv (request, zmq::recv_flags::none);
        std::string rqt = std::string(static_cast<char*>(request.data()), request.size());
        std::cout << "Received request: " << rqt << std::endl;
        
        sleep(1);

        // send
        zmq::message_t reply (5);
        memcpy (reply.data (), "World", 5);
        socket.send (reply, zmq::send_flags::none);
    }
    return 0;
}
