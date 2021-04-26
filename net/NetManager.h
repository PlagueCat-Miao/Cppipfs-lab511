//
// Created by root on 8/11/19.
//

#ifndef SELECTLEARN_NETMANAGER_H
#define SELECTLEARN_NETMANAGER_H

/*server*/
#include <stdio.h>
#include <map>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include "ClientState.h"
#include "MsgBase.h"
#include "../logic/MsgHandler.h"


typedef void (* OnFunc)(ClientState* , MsgBase*);


class NetManager{

public:
    static void NetManagerAwake();
public:

    //TODO ClientStatelist for select
    // they use int->fd_set to grovement socket we have 1024 limit
    static ClientState clients[FD_SETSIZE];
    static int max_clientIdx;

    //  save listenFd
    static int listenFd;
    //地址信息结构体大小
    static int maxFd;
    static fd_set allSet;//所有socket的文件描述符的集合
    static int sin_size;

    //TODO IPFS Information
    // Sever_ipfsHash init in NetManager.cpp->NetManagerAwake
    static string m_ipfsHash;
   // static vector<FileState> *files_video;
public:
    static ClientState* FindCSByID(int id)
    {
        for(int i=0;i<=max_clientIdx;i++)
            if(clients[i].fd!=-1&&clients[i].id==id)
                return &clients[i];
        return NULL;
    }

public:
    // MsgHandler msg->func

    static map<string, OnFunc> *methodMsgHandlerMap;
    static OnFunc GetMethodMsgHandlerByName(string name);
    static void  RegistMsgHandler(string name,OnFunc func){
        (*methodMsgHandlerMap)[name]= func;
        cout<<"On"<<name <<" online"<<endl;
    }
public:
    static void StartLoop(int );
    static void ReadListenfd(fd_set &readset,int &n_ready);
    static void ReadClientfd(fd_set &readset,int &n_ready);
    static void OnReceiveData(ClientState *client);
    static void Send(ClientState *client,MsgBase* msg);
    static MsgBase* ReceiveMsg(ClientState *client,char* byte);


};



#endif //SELECTLEARN_NETMANAGER_H
