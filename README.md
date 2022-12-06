# fullstack service networking - ZMQ

## 목차

- [1. 선택 챕터/언어](#선택-챕터언어)
- [2. REQ-REP 예제](#req-rep-예제)
- [3. PUB-SUB 예제](#pub-sub-예제)
- [4. PUB-SUB-PULL-PUSH 예제](#pub-sub-and-pull-push-예제)
- [5. PUB-SUB-PULL-PUSH 예제 V2](#pub-sub-and-pull-push-예제-v2)
- [6. DEALER ROUTER 예제](#dealer-router-async-예제)
- [7. DEALER ROUTER 예제 - client thread version](#dealer-router-async-예제---client-thread-version)

## 선택 챕터/언어

> ZMQ/C++
> zmq.hpp

## REQ-REP 예제

> 서버 동작

- 서버는 받은 데이터를 화면에 출력한다.
- 데이터를 받고 "World" 문자를 보낸다.

> 클라이언트 동작

- "Hello" 문자를 보내고 데이터를 받는 것을 10번 반복한다.

> 코드상에서 구현

- 출력하는 과정에서 받은 데이터를 string type으로 변환해 출력한다.

```c++
std::string rpl = std::string(static_cast<char*>(reply.data()), reply.size());
```

- message_t type으로 데이터를 보내기 위해 `memcpy`를 사용한다.

```c++
memcpy (request.data (), "Hello", 5);
```

> 코드

- [server](./01-req-rep-basic-server.cpp)
- [client](./02-req-rep-basic-client.cpp)

> 실행방법

- server를 먼저 실행한다.
- client는 원하는 수만큼 실행 가능하다.

```bash
g++ -std=c++17 01-req-rep-basic-server.cpp -o 01-req-rep-basic-server -lzmq
g++ -std=c++17 02-req-rep-basic-client.cpp -o 02-req-rep-basic-client -lzmq

./01-req-rep-basic-server
./02-req-rep-basic-client
```

> 실행 영상

<https://user-images.githubusercontent.com/68274803/205882323-14475325-21d5-4dd8-8f83-966d597bd7e1.mp4>

## PUB-SUB 예제

> 서버 동작

- random하게 숫자를 생성해 온도 정보를 보내는 것처럼 동작한다.

> 클라이언트 동작

- 서버로부터 온 온도 정보 메시지 중 필터를 통해 원하는 메시지만 받는다.
- 서버로부터 온 정보를 출력한다.
- total_temp에 온도 정보를 더한 뒤 평균 온도를 출력한다.

> 코드상에서 구현

- filter등록

```c++
const char *filter = (argc > 1)? argv [1]: "10001 ";
subscriber.setsockopt(ZMQ_SUBSCRIBE, filter, strlen (filter));
```

- 온 데이터를 zipcode, temp, relhumidity로 나눠서 받는다

```c++
std::istringstream iss(static_cast<char*>(update.data()));
iss >> zipcode >> temperature >> relhumidity ;
```

> 코드

- [server](./03-pub-sub-basic-server.cpp)
- [client](./04-pub-sub-basic-client.cpp)

> 실행방법

- server를 먼저 실행한다.
- client는 원하는 수만큼 실행 가능하다.

```bash
g++ -std=c++17 03-pub-sub-basic-server.cpp -o 03-pub-sub-basic-server -lzmq
g++ -std=c++17 04-pub-sub-basic-client.cpp-o 04-pub-sub-basic-client -lzmq

./03-pub-sub-basic-server
./04-pub-sub-basic-client
```

> 실행 영상

- 영상에서는 client를 3개 실행하였다.

  <https://user-images.githubusercontent.com/68274803/205862663-914b30cd-c6f6-4422-829c-c98e5067a109.mp4>

## PUB-SUB AND PULL-PUSH 예제

> 서버 동작

- collector가 client로부터 오는 message를 pull한다.
- publisher가 받은 message를 client에 send한다.

> 클라이언트 동작

- subscriber가 받은 메시지가 있으면 정보를 가져온다
- publisher가 받은 데이터를 server에 보내준다.

> 코드상에서 구현

- `zmq_poollitem_t`를 통해 message가 왔는지 확인

```c++
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
```

> 코드

- [server](./05-pub-sub-and-pull-push-server.cpp)
- [client](./06-pub-sub-and-pull-push-client.cpp)

> 실행방법

- server를 먼저 실행한다.
- client는 원하는 수만큼 실행 가능하다.

```bash
g++ -std=c++17 05-pub-sub-and-pull-push-server.cpp -o 05-pub-sub-and-pull-push-server -lzmq
g++ -std=c++17 06-pub-sub-and-pull-push-client.cpp -o 06-pub-sub-and-pull-push-client -lzmq

./05-pub-sub-and-pull-push-server
./06-pub-sub-and-pull-push-client
```

> 실행 영상

<https://user-images.githubusercontent.com/68274803/205867182-acabbccd-9bae-48d9-94dc-6006e171d679.mp4>

## PUB-SUB AND PULL-PUSH 예제 v2

> 서버 동작

- collector가 client로부터 오는 message를 pull한다.
- publisher가 받은 message를 client에 send한다.
- 어떤 client로부터 받았는지 정보를 함께 출력한다.

> 클라이언트 동작

- subscriber가 받은 메시지가 있으면 정보를 가져온다
- publisher가 받은 데이터를 server에 보내준다.

> 코드상에서 구현

- 기존에 string type을 가졌던 데이터를 보내기 위해 `c_str()`을 사용했다.

```c++
// 조합한 string
std::string msg = "(" + clientID + ":OFF)";
memcpy (message.data (), msg.c_str(), msg.length()+1);
```

> 코드

- [server](./07-pub-sub-and-pull-server-v2.cpp)
- [client](./08-pub-sub-and-pull-client-v2.cpp)

> 실행방법

- server를 먼저 실행한다.
- client는 원하는 수만큼 실행 가능하다.
- client를 실행할 때는 command line에서 client#1과 같은 client를 구분할 수 있는 문자를 같이 입력한다.

```bash
g++ -std=c++17 07-pub-sub-and-pull-server-v2.cpp -o 07-pub-sub-and-pull-server-v2 -lzmq
g++ -std=c++17 08-pub-sub-and-pull-client-v2.cpp -o 08-pub-sub-and-pull-client-v2 -lzmq

./07-pub-sub-and-pull-server-v2
./08-pub-sub-and-pull-client-v2 client#1
```

> 실행 영상

<https://user-images.githubusercontent.com/68274803/205867208-ece4c4b8-9129-45d0-9113-e7ffe9533552.mp4>

## DEALER ROUTER ASYNC 예제

> 서버 동작

- thread를 통해 구현한다.
- frontend는 client에서 오는 메시지를 받을 수 있는 socket이다.
- backend는 server와 server안의 worker를 연결하는 socket이다.

> 클라이언트 동작

- thread를 통해 구현한다.
- command line을 통해 받은 id와 생성한 request를 보낸다.
- server로부터 온 정보가 있는지 확인하고 있는 경우에만 메시지를 받고 출력한다.

> 코드상에서 구현

- `proxy`를 통해 일을 분배할 수 있다.

```c++
zmq::proxy(static_cast<void*>(frontend_),
                       static_cast<void*>(backend_),
                       nullptr);
```

- client는 `setsockopt`를 통해 본인의 id를 보낼 수 있다.

```c++
client_socket_.setsockopt(ZMQ_IDENTITY, ident_.c_str(), ident_.length());
```

- server는 client의 id 정보를 받기만 하면 된다.

```c++
zmq::message_t identity;
zmq::message_t msg;
worker_.recv(identity);
worker_.recv(msg);
```

> 코드

- [server](./09-dealer-router-async-server.cpp)
- [client](./10-dealer-router-async-client.cpp)

> 실행방법

- server를 먼저 실행한다.
- server를 실행할 때는 worker개수를 함께 받는다.
- client는 원하는 수만큼 실행 가능하다.
- client를 실행할 때는 command line에서 client#1과 같은 client를 구분할 수 있는 문자를 같이 입력한다.

```bash
g++ -std=c++17 09-dealer-router-async-server.cpp -o 09-dealer-router-async-server -lzmq -lpthread
g++ -std=c++17 10-dealer-router-async-client.cpp -o 10-dealer-router-async-client -lzmq -lpthread

./09-dealer-router-async-server 1
./10-dealer-router-async-client client#1
```

> 실행 영상

<https://user-images.githubusercontent.com/68274803/205882370-b3271e8a-834c-4c97-88e8-c8a6f0d24405.mp4>

## DEALER ROUTER ASYNC 예제 - client thread version

> 서버 동작 (그대로)

- thread를 통해 구현한다.
- frontend는 client에서 오는 메시지를 받을 수 있는 socket이다.
- backend는 server와 server안의 worker를 연결하는 socket이다.

> 클라이언트 동작 (thread를 생성하여 받은 데이터를 처리하도록 구현)

- thread를 통해 구현한다.
- command line을 통해 받은 id와 생성한 request를 보낸다.
- server로부터 온 정보가 있는지 확인하고 있는 경우에만 메시지를 받고 출력한다.

> 코드상에서 구현

- `recv_handler()`function을 구현

```c++
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
```

> 코드

- [server](./09-dealer-router-async-server.cpp)
- [client](./11-dealer-router-async-client-thread.cpp)

> 실행방법

- server를 먼저 실행한다.
- server를 실행할 때는 worker개수를 함께 받는다.
- client는 원하는 수만큼 실행 가능하다.
- client를 실행할 때는 command line에서 client#1과 같은 client를 구분할 수 있는 문자를 같이 입력한다.

```bash
g++ -std=c++17 09-dealer-router-async-server.cpp -o 09-dealer-router-async-server -lzmq -lpthread
g++ -std=c++17 11-dealer-router-async-client-thread.cpp -o 11-dealer-router-async-client-thread -lzmq -lpthread

./09-dealer-router-async-server 1
./11-dealer-router-async-client-thread client#1
```

> 실행 영상

<https://user-images.githubusercontent.com/68274803/205882432-aa6f6e99-9613-498b-8649-178760123a64.mp4>
