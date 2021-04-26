//
// Created by hellcat on 2019/11/20.
//

#ifndef IPFS_DEMO_CLIENT_NORMAL_H
#define IPFS_DEMO_CLIENT_NORMAL_H

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
#include "json/MsgFFmpeg.h"

using namespace std;

bool rtmp_runing=false;

class easy_Netmanger{
public:
    int sockfd, numbytes;

public:
    bool Init_net();
    MsgBase* encode_recv_data(ClientState *client);
    ~easy_Netmanger()
    {
        close(sockfd);
    }

};

bool Login();
void print_help();
bool put_test();
vector<string> Split(string list_src, char A);
bool show_datalist();
bool get_test();
bool CallforWetwork( bool &isRun);



#endif //IPFS_DEMO_CLIENT_NORMAL_H
