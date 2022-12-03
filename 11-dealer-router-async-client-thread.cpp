#include <vector>
#include <thread>
#include <memory>
#include <functional>
#include <string>
#include <zmq.hpp>
#include "zhelpers.hpp"
#include <iostream>
#include <unistd.h>

class client_task {
public:
    client_task(std::string ident)
        : ctx_(1),
          client_socket_(ctx_, ZMQ_DEALER),
          ident_(ident)
    {}
    void recv_handler(zmq::pollitem_t items_[]){
        while (1){
            zmq::poll(items_, 1, 1000);
            if (items_[0].revents & ZMQ_POLLIN) {
                zmq::message_t msg;
                client_socket_.recv(msg);
                std::string smsg(static_cast<char*>(msg.data()), msg.size());
                std::cout << ident_ << " received: " << smsg << std::endl;
            }
        }
    }

    void start() {
        char identity[10] = {};
        client_socket_.setsockopt(ZMQ_IDENTITY, ident_.c_str(), ident_.length());
        client_socket_.connect("tcp://localhost:5570");
        std::cout << "Client " << ident_ << " started" <<std::endl;
        zmq::pollitem_t items[] = {
            { client_socket_, 0, ZMQ_POLLIN, 0 } };
        int request_nbr = 0;
        std::thread t(std::bind(&client_task::recv_handler, this, items)); //recv 처리할 thread 생성
        t.detach();
        try {
            while (true) {
                request_nbr += 1;
                std::cout << "Req #" << request_nbr << std::endl;
                char request_string[16] = {};
                sprintf(request_string, "request #%d", request_nbr);
                client_socket_.send(request_string, strlen(request_string));

                sleep(1);

            }
        }
        catch (std::exception &e) {}
    }

private:
    zmq::context_t ctx_;
    zmq::socket_t client_socket_;
    std::string ident_;
};

int main(int argc, char* argv[]){

    client_task ct(argv[1]);
    std::thread t1(std::bind(&client_task::start, &ct));

    t1.detach();
    getchar();
    return 0;
}