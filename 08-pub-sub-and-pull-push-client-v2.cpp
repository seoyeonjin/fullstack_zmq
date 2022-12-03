#include <zmq.hpp>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

int main (int argc, char *argv[])
{
    srand((unsigned int)time(NULL));

    zmq::context_t context (1);

    zmq::socket_t subscriber (context, zmq::socket_type::sub);
    subscriber.connect("tcp://localhost:5557");
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    zmq::socket_t publisher(context, ZMQ_PUSH);
    publisher.connect("tcp://localhost:5558");

    std::string clientID = argv[1];
    while (1){
        zmq_pollitem_t items[1];
        
        items[0].socket = subscriber;
        items[0].events = ZMQ_POLLIN;  

        zmq::poll(items, 1, 100);

        if (items[0].revents & ZMQ_POLLIN){
            zmq::message_t message;
            subscriber.recv(message, zmq::recv_flags::none);
            std::string smessage(static_cast<char*>(message.data()), message.size());
            std::cout << clientID << ": receive status => " << smessage << std::endl;
        }

        else{     
            int rand = std::rand() % 101;

            if (rand < 10){
                sleep(1);
                zmq::message_t message(clientID.length() + 6);
               std::string msg = "("  + clientID + ":ON)";
                memcpy (message.data (), msg.c_str(), msg.length()+1);
                publisher.send(message, zmq::send_flags::none);
                std::cout << clientID << ": send status - activated" << std::endl;
            }
            else if (rand > 90){
                sleep(1);
                zmq::message_t message(clientID.length() + 7);   
                std::string msg = "(" + clientID + ":OFF)";             
                memcpy (message.data (), msg.c_str(), msg.length()+1);
                publisher.send(message, zmq::send_flags::none);
                std::cout << clientID << ": send status - deactivated" << std::endl;

            }
        }
    }
    return 0;
}