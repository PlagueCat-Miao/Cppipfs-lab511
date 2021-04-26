//
// Created by hellcat on 2019/10/31.
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
#include "json/MsgFFmpeg.h"

#include <time.h>
#include "ffmpeg/RTMPParallel.h"
#include <thread>

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

bool isRun=true;



int main(int argc,char * argv[]){

    if(argc >=2){m_id=atoi(argv[1]);}
    if(argc >=3){strcpy(defaultHostName,argv[2]);}
    if(argc >=4)
    {
        int i=0;PORT=0;
        cout<<argv[3]<<endl;
        while( argv[3][i]!='\0')
        {
            PORT*=10;
            PORT+=argv[3][i]-'0';
            i++;
        }
    }
    if(argc >=5){m_identity=atoi(argv[4]);}

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


    std::cout<<"wait for wetwork "<<std::endl;
    RTMPParallel* Rpp;
    std::thread* t =nullptr;
    MsgAnswer* m_ans = nullptr;

    while(1)
    {
        m_ans = nullptr;
        if ((numbytes = read(sockfd, buf, MAXDATASIZE)) == -1)
        {
            std::cout<<"read error!\n"<<std::endl;
            return 0;
        }
        buf[numbytes]='\0';
        std::cout<<"Client Recv: "<<buf <<std::endl;
        m_ans=(MsgAnswer*)NetManager::ReceiveMsg(&cs,buf);

        if(m_ans == nullptr){
            std::cout<<"err MsgNOAnswer"<<std::endl;
        }

        if(m_ans->ret==1)
        {
            string rtmp="rtmp://";
            rtmp.append(cs.m_addr);
            rtmp.append(m_ans->resultRtmp);
            cout<<"rtmp====>"<<rtmp<<endl;

            isRun= true;
            RTMPParallel Rpp;
            strcpy(Rpp.outUrl,rtmp.c_str());
            t = new thread( &RTMPParallel::open_rtmp, Rpp,&isRun);

            m_ans->ret=3;
            m_ans->resultRtmp=rtmp;
            NetManager::Send(&cs,m_ans);

        }
        else if(m_ans->ret==4)
        {
            if(t== nullptr)
            {
                cout<<"not open rtmp"<<endl;
            }
            else
            {
                isRun=false;
                t->join();
                delete t;
                t =nullptr;
                break;
            }


        }
        else
        {
            cout<<"not need response :"<<m_ans->ret<<endl;
            break;
        }

       delete m_ans;

    }
   close(sockfd);
   return 0;
}


