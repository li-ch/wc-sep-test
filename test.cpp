#include "rdma.h"
#include <cstring>
#include <iostream>
#include <sys/time.h>
#include <unistd.h> 
#include "buf_list.h"
using namespace std;
using namespace amber;
using namespace amber::rdma;

const int max_packet_size = (1 << 21);
const int SND_NUM = 1e2;
const int CLI_NUM = 2;
const int SVR_NUM = 2;
const int QP_NUM = 2;


const char* svr_ip[] = {"172.16.18.197", "172.16.18.198"};
const char* svr_port[] = {"54321", "43210"};
const char* cli_ip[] = {"172.16.18.207", "172.16.18.205"};

int sz;


void Client()
{
    RDMA_Client cli(SVR_NUM * QP_NUM);
    BufferList pool(sizeof(RDMA_Message) + max_packet_size, 500);
    for (int i = 0; i < SVR_NUM; ++i)
    {
        for (int j = 0; j < QP_NUM; ++j)
            cli.Connect(svr_ip[i], svr_port[j], (void *)pool.Head(), pool.Capcity());
	    puts("Client Connect Success");
    }

    sleep(3);

    puts("Client Start Sending");
    for (int i = 0; i < SVR_NUM * SND_NUM * QP_NUM; ++i)
    {
        shared_ptr<RDMA_Message> f;
        auto buf = pool.GetFreeBuf();
        if (buf == nullptr)
        {
            --i;
            continue;
        }
        f.reset((RDMA_Message*)buf, [&pool](RDMA_Message* msg){pool.FreeBuf((char*)msg);});
        f.get()->idx = i % (QP_NUM * SVR_NUM);
        f.get()->data_len = max_packet_size;
        sprintf(f.get()->msg, "Test rdma message seq[%d]", i);
	puts("Client Message");
        if ((i + 1) % (SND_NUM / 5) == 0)
        {
            printf("Send Msg %d %d\n", f->idx, i);
        }
        cli.Send(f);
    }

    sleep(10);
}

int num;
void Handle(shared_ptr<RDMA_Message>& msg)
{
    if (++num % (SND_NUM / 10) == 0)
    {
        printf("Recv Msg idx[%d] %d, %d, (%s), addr: %p\n", msg.get()->idx, msg.get()->data_len, sz++, msg.get()->msg, msg.get());
    }
}

void Server()
{
    RDMA_Server svr(CLI_NUM, QP_NUM, bind(Handle, placeholders::_1));

    for (int i = 0; i < QP_NUM; ++i) {
        svr.Listen(svr_port[i], sizeof(RDMA_Message) + max_packet_size);
	puts("Server Listen Success");
    }

    for (int i = 0; i < CLI_NUM; ++i)
    {
        for (int j = 0; j < QP_NUM; ++j)
        {
            svr.Accept(j);
	    puts("Server Accept Success");
        }
    }

    sleep(10);
}

int main(int argc, char* argv[])
{
    if (argc == 1)
    {   
        puts("Client");
        Client();
    }   
    else
    {   
        puts("Server");
        Server();
    }   
    puts("End");
    return 0;
}
