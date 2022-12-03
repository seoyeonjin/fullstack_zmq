#include <zmq.hpp>
#include <iostream>
#include <sstream>
#include <unistd.h>


int main (int argc, char *argv[])
{
    zmq::context_t context (1);

    // socket
    std::cout << "Collecting updates from weather server...\n" << std::endl;
    zmq::socket_t subscriber (context, zmq::socket_type::sub);
    subscriber.connect("tcp://localhost:5556");

    //  Subscribe to zipcode, default is NYC, 10001
	const char *filter = (argc > 1)? argv [1]: "10001 ";
    subscriber.setsockopt(ZMQ_SUBSCRIBE, filter, strlen (filter));

    int update_nbr;
    long total_temp = 0;
    for (update_nbr = 0; update_nbr < 20; update_nbr++) { // 20번 반복

        zmq::message_t update;
        int zipcode, temperature, relhumidity;

        subscriber.recv(update, zmq::recv_flags::none);

        std::istringstream iss(static_cast<char*>(update.data()));
		iss >> zipcode >> temperature >> relhumidity ;

		total_temp += temperature;
        std::cout << "Receive temperature for zipcode '" << filter << "' was " << temperature << "F" << std::endl;
        sleep(1);
    }
    // 평균 temp 출력
    std::cout 	<< "Average temperature for zipcode '"<< filter
    			<<"' was "<< ((float)total_temp / update_nbr) <<"F"
    			<< std::endl;
    return 0;
}