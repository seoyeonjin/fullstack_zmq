#include <zmq.hpp>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

int main ()
{
    srand((unsigned int)time(NULL));

    zmq::context_t context (1);

    zmq::socket_t subscriber (context, zmq::socket_type::sub);
    subscriber.connect("tcp://localhost:5557");
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    zmq::socket_t publisher(context, ZMQ_PUSH);
    publisher.connect("tcp://localhost:5558");

    while (1){
        zmq_pollitem_t items[1];
        
        items[0].socket = subscriber;
        items[0].events = ZMQ_POLLIN;  

        zmq::poll(items, 1, 100);

        // msg 왔는지 확인
        if (items[0].revents & ZMQ_POLLIN){
            zmq::message_t message(4);
            subscriber.recv(message, zmq::recv_flags::none);
            std::string smessage(static_cast<char*>(message.data()), message.size());
            std::cout << "I: received message " << smessage << std::endl;
        }
        else{
            zmq::message_t message(4);
            
            int rand = std::rand() % 101;

            if (rand < 10){
                snprintf((char *) message.data(), 4,"%d", rand);
                publisher.send(message, zmq::send_flags::none);
                std::cout << "I: sending message " << rand << std::endl;

            }
        }
    }
    return 0;
}