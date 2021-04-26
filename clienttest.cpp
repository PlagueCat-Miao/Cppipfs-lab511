//
// Created by root on 8/12/19.
//
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>

#include "json/MsgMove.h"

#include "logic/MsgHandler.h"

#include "net/ClientState.h"
#include "net/NetManager.h"
#include "net/MsgBase.h"
#include "json/MsgIPFS.h"
#include "json/MsgMove.h"
#include "net/ByteArray.h"
#include "ipfs/IPFSCtrl.h"
#include <time.h>

using namespace std;

#ifndef COMMON2_H_
#define COMMON2_H_
const int BACKLOG=5;           //listen队列中等待的连接数
const int MAXDATASIZE=1024;    //缓冲区大小
 int PORT=2248;           //服务器端口
 char defaultHostName[20] = "127.0.0.1";

#endif

#ifndef CLIENT_ATTRIBUTE_
#define CLIENT_ATTRIBUTE_
int m_id=251;
int m_identity=3;
#endif



void put_test(ClientState cs,string src);

int main(int argc,char * argv[]){

    if(argc >=2){strcpy(defaultHostName,argv[1]);}
    if(argc >=3)
    {
       int i=0;PORT=0;
       cout<<argv[2]<<endl;
       while( argv[2][i]!='\0')
       {
           PORT*=10;
           PORT+=argv[2][i]-'0';
           i++;
       }
    }

    char hostname[100];
    gethostname(hostname,sizeof(hostname));
    int sockfd, numbytes;
    struct hostent *he;
    struct sockaddr_in serv_addr;
    if ((he = gethostbyname(defaultHostName)) == NULL)
    {
        std::cout<<"gethostbyname error!"<<std::endl;
        return 0;;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cout<< "socket error!"<<std::endl;
        return 0;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(serv_addr.sin_zero), 8);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1)
    {
        std::cout<<"connect error!"<<std::endl;
        return 0;
    }

    MsgFactory::AwakeLoad();
    NetManager::NetManagerAwake();

    MsgLogin msg;
    MsgLogin *ans;
    msg.id = m_id;
    msg.identity = m_identity;
    msg.ipfsHash=NetManager::m_ipfsHash;
    msg.result = 0;//send not ack

    ClientState cs;
    cs.fd=sockfd;
    cs.m_addr=defaultHostName;
    cs.m_port=to_string(PORT);

    cout<<" different processes,different sockfd-stack. This sockfd():"<<sockfd<<endl;
    cout<<" so in different processes,same sockfd(int),different Net-System-File. "<<endl;
    cout<<" MyID:"<<msg.id;
    if(msg.identity==1) cout<<" : "<<"admin"<<endl;
    if(msg.identity==2) cout<<" : "<<"gateway"<<endl;
    if(msg.identity==3) cout<<" : "<<"edge"<<endl;

    NetManager::Send(&cs,&msg);
    char buf[MAXDATASIZE];
    if ((numbytes = read(sockfd, buf, MAXDATASIZE)) == -1)
    {
        std::cout<<"read error!\n"<<std::endl;
        return 0;
    }
    buf[numbytes]='\0';
    std::cout<<"Client Recv: "<<buf <<std::endl;
    ans=(MsgLogin*)NetManager::ReceiveMsg(&cs,buf);
    if(ans->result==1){cout<<"login OK"<<endl;}

    put_test(cs,"../ipfs/file/Hellcat.txt");


    while (1);
    close(sockfd);
    return 0;
}

void put_test(ClientState cs,string file_source)
{
    int numbytes;
    MsgPut msg;
    MsgPut *ans;

    msg.id=m_id;
    string putans=IPFSCtrl::Put(file_source);

    msg.ipfsHash=putans;

    msg.time=time(0);

    NetManager::Send(&cs,&msg);
    char buf[MAXDATASIZE];
    if ((numbytes = read(cs.fd, buf, MAXDATASIZE)) == -1)
    {
        std::cout<<"read error!\n"<<std::endl;
        return ;
    }
    buf[numbytes]='\0';
    std::cout<<"Client Recv: "<<buf <<std::endl;
    ans=(MsgPut*)NetManager::ReceiveMsg(&cs,buf);//have new
    if(ans->result==1){cout<<"Put OK"<<endl;}



}

