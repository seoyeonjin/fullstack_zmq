#include <zmq.hpp>
#include <string>
#include <iostream>

int main ()
{

    // socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, zmq::socket_type::req);

    std::cout << "Connecting to hello world server..." << std::endl;
    socket.connect ("tcp://localhost:5555");

    // 10번 반복
    for (int request_nbr = 0; request_nbr != 10; request_nbr++) {
        zmq::message_t request (5);
        memcpy (request.data (), "Hello", 5); // memcpy로 데이터 복사
        std::cout << "Sending request " << request_nbr << " …" << std::endl;
        socket.send (request, zmq::send_flags::none);

        // recv
        zmq::message_t reply;
        socket.recv (reply, zmq::recv_flags::none);
        
        std::string rpl = std::string(static_cast<char*>(reply.data()), reply.size());
        
        std::cout << "Received reply " << request_nbr << " [" << rpl << "] " << std::endl;
    }
    return 0;
}