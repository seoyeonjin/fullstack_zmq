#include <zmq.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>

#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0)) // 변수 생성

int main () {
    std::cout << "Publishing updates at weather server..." << std::endl;
   
    // socket
    zmq::context_t context (1);
    zmq::socket_t publisher (context, zmq::socket_type::pub);
    publisher.bind("tcp://*:5556");

    srandom ((unsigned) time (NULL)); //random하게 숫자를 생성하기 위한 설정
    while (1) {

        int zipcode, temperature, relhumidity;

        zipcode     = within (100000);
        temperature = within (215) - 80;
        relhumidity = within (50) + 10;

        zmq::message_t message(20);
        snprintf ((char *) message.data(), 20 ,
        	"%05d %d %d", zipcode, temperature, relhumidity);
        publisher.send(message, zmq::send_flags::none);

    }
    return 0;
}