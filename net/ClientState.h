//
// Created by root on 8/5/19.
//
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>

#ifndef SELECTLEARN_CLIENTSTATE_H
#define SELECTLEARN_CLIENTSTATE_H


#include "ByteArray.h"
#include <time.h>

class ClientState {

public:
    int fd;        //客户端socket描述符
    std::string m_addr;
    std::string m_port;
    ByteArray readBuff;
    int id ;
    int identity;// 1 admin 2 3 Edge
    std::string ipfsHash;
};
/*
        if(NetManager::clients[i].identity==1) cout<<": "<<"admin"<<endl;
        if(NetManager::clients[i].identity==2) cout<<": "<<"gateway"<<endl;
        if(NetManager::clients[i].identity==3) cout<<": "<<"edge"<<endl;
 */




#endif //SELECTLEARN_CLIENTSTATE_H
