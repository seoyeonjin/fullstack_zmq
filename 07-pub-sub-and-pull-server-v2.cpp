#include <iostream>
#include <string>
#include <zmq.hpp>

int main(){
    zmq::context_t context(1);

    zmq::socket_t publisher (context, zmq::socket_type::pub);
    publisher.bind("tcp://*:5557");
    
    zmq::socket_t collector (context, ZMQ_PULL);
    collector.bind("tcp://*:5558");

    while (1) {

        zmq::message_t message;
        collector.recv(&message); // collector가 message를 받아서 
        std::string smessage(static_cast<char*>(message.data()), message.size());
        std::cout << "server: publishing update => " << smessage << std::endl;
        publisher.send(message, zmq::send_flags::none); // 대량으로 뿌려줌
    }

    
    return 0;
}