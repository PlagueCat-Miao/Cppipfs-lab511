//
// Created by root on 8/5/19.
//

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdlib.h>
#include "net/ByteArray.h"
#include <iostream>
#include "json/MsgMove.h"
#include "logic/MsgHandler.h"
#include "net/NetManager.h"
#include "db/MySQL.h"
using namespace std;




int main(int argc,char * argv[]){

    if(!MySQL::connect()){return 44;}

    MsgFactory::AwakeLoad();
    NetManager::NetManagerAwake();


    NetManager::StartLoop(2248);

    return 0;
}
