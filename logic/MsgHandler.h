//
// Created by root on 8/11/19.
//

#ifndef SELECTLEARN_MSGHANDLER_H
#define SELECTLEARN_MSGHANDLER_H

#include "../net/ClientState.h"
#include "../net/MsgBase.h"
#include "../json/MsgMove.h"
//#include "../net/NetManager.h"


//void OnMsgMove(ClientState* cs, MsgBase* msg);

class MsgHandler {
public:
    // TODO: callback_fun regist in NetManager.cpp -> NetManager::NetManagerAwake()
    static void OnMsgMove(ClientState* cs, MsgBase* msg);
    static void OnMsgLogin(ClientState* cs, MsgBase* msg);
    static void OnMsgPut(ClientState* cs, MsgBase* msg);
    static void OnMsgGet(ClientState* cs, MsgBase* msg);
    static void OnMsgCallFor(ClientState* cs, MsgBase* msg);
    static void OnMsgAnswer(ClientState* cs, MsgBase* msg);

};


#endif //SELECTLEARN_MSGHANDLER_H
