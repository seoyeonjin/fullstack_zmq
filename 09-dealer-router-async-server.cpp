#include <vector>
#include <thread>
#include <memory>
#include <functional>
#include <iostream>
#include <zmq.hpp>

class server_worker {
public:
    server_worker(zmq::context_t &ctx, int sock_type, int id)
        : ctx_(ctx),
          worker_(ctx_, sock_type),
          id_(id)
    {}

    void work() {
            worker_.connect("inproc://backend");
            std::cout << "Worker#" << id_ << " started" << std::endl;
        try {
            while (true) {
                zmq::message_t identity;
                zmq::message_t msg;
                zmq::message_t copied_id;
                zmq::message_t copied_msg;
                worker_.recv(identity);
                worker_.recv(msg);
                std::string smsg(static_cast<char*>(msg.data()), msg.size());
                std::string sident(static_cast<char*>(identity.data()), identity.size());
                std::cout << "Worker#" << id_ << " received " << smsg << " from " << sident << std::endl;

                copied_id.copy(&identity);
                copied_msg.copy(&msg);
                worker_.send(copied_id, ZMQ_SNDMORE);
                worker_.send(copied_msg);
            }
        }
        catch (std::exception &e) {}
    }



private:
    zmq::context_t &ctx_;
    zmq::socket_t worker_;
    int id_;
};


class server_task {
public:
    server_task(int num_)
        : ctx_(1),
          frontend_(ctx_, ZMQ_ROUTER),
          backend_(ctx_, ZMQ_DEALER),
          num_thread_(num_)
    {}

    void run() {
        frontend_.bind("tcp://*:5570");
        backend_.bind("inproc://backend");

        std::vector<server_worker *> worker; //worker 등록하기위한 vector
        std::vector<std::thread *> worker_thread;
        for (int i = 0; i < num_thread_; ++i) {
            worker.push_back(new server_worker(ctx_, ZMQ_DEALER,i));

            worker_thread.push_back(new std::thread(std::bind(&server_worker::work, worker[i])));
            worker_thread[i]->detach();
        }


        try {
            zmq::proxy(static_cast<void*>(frontend_),
                       static_cast<void*>(backend_),
                       nullptr);
        }
        catch (std::exception &e) {}

        for (int i = 0; i < num_thread_; ++i) {
            delete worker[i];
            delete worker_thread[i];
        }
    }


private:
    zmq::context_t ctx_;
    zmq::socket_t frontend_;
    zmq::socket_t backend_;
    int num_thread_;
};

int main (int argc, char* argv[])
{
    server_task st(atoi(argv[1])); 
    std::thread t(std::bind(&server_task::run, &st));
    t.detach();

    getchar();
    return 0;
}