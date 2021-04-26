//
// Created by hellcat on 2019/10/29.
//
/*
* The world calls for wetwork, and we answer
* No greater good.no just cause.
*/

#ifndef IPFS_DEMO_MSGFFMPEG_H
#define IPFS_DEMO_MSGFFMPEG_H

#include <string>
#include <iostream>
#include "../net/CJsonObject.hpp"
#include "../net/MsgBase.h"
#include <iostream>



extern void* create_MsgCallFor();
extern void* create_MsgAnswer();

class MsgCallFor :public  MsgBase {
public:
    //string protoName;
    int id;         // 2^28
    //int identity;   // 1 admin 2 GateWay 3 Edge
    int toId;    // camera 2^28
    int ret;     // 0 err 1 start 2 wait 3 complete 4 stop
    string resultRtmp;

protected:
    void SetPropertys(){
        SetProperty("protoName",asString,&protoName);
        //SetProperty("identity",asInt,&identity);
        SetProperty("id",asInt,&id);
        SetProperty("toId",asInt,&toId);
        SetProperty("ret",asInt,&ret);
        SetProperty("resultRtmp",asString,&resultRtmp);

    }
public:

    static void MsgAwake(){

        //TODO regist Msgname for reflection/interface
        /* build/find void* create_MsgXXX() */
        /* this function regist in MsgBase.cpp -> MsgFactory::AwakeLoad() */
        create_Func func =create_MsgCallFor;
        MsgFactory::RegistClass("MsgCallFor",func);
        cout<<"MsgCallFor"<<" regist"<<endl;

    }
    virtual void MsgStart(){
        //TODO set value
        // this funcion will active in Construcort == MsgXXX()
        protoName="MsgCallFor";
        id=-4;
        toId=-4;    // camera 2^28
        ret=0;


        //TODO  SetProperty regist_JSON_Property
        /* ifnot use MsgXXXX::SetPropertys() , [ this->SetPropertys(); is not work because 'this' only is Object(dui xiang)]
         * child(class) will use their SetPropertys() [ Parent::init(){SetPropertys(child's)} ]
         * but not Parent's SetPropertys() */
        MsgCallFor::SetPropertys();


    }

public:
    MsgCallFor(){
        //TODO Init or not
        //MsgBase::Init();
        MsgStart();
    }
    MsgCallFor(int source,int toid,int result,string result_rtmp){
        MsgStart();
        id=source;    // source
        toId=toid;    // camera 2^28
        ret=result;
        resultRtmp=result_rtmp;
    }
};




class MsgAnswer :public  MsgBase {
public:
    //string protoName;
    int id;         // 2^28
    int toId;    // camera 2^28
    int ret;     // 0 err 1 start 2 wait 3 complete 4 stop
    string resultRtmp;

protected:
    void SetPropertys(){
        SetProperty("protoName",asString,&protoName);
        SetProperty("id",asInt,&id);
        SetProperty("toId",asInt,&toId);
        SetProperty("ret",asInt,&ret);
        SetProperty("resultRtmp",asString,&resultRtmp);

    }
public:

    static void MsgAwake(){

        //TODO regist Msgname for reflection/interface
        /* build/find void* create_MsgXXX() */
        /* this function regist in MsgBase.cpp -> MsgFactory::AwakeLoad() */
        create_Func func =create_MsgAnswer;
        MsgFactory::RegistClass("MsgAnswer",func);
        cout<<"MsgAnswer"<<" regist"<<endl;

    }
    virtual void MsgStart(){
        //TODO set value
        // this funcion will active in Construcort == MsgXXX()
        protoName="MsgAnswer";
        id=-4;
        toId=-4;    // camera 2^28
        ret=0;


        //TODO  SetProperty regist_JSON_Property
        /* ifnot use MsgXXXX::SetPropertys() , [ this->SetPropertys(); is not work because 'this' only is Object(dui xiang)]
         * child(class) will use their SetPropertys() [ Parent::init(){SetPropertys(child's)} ]
         * but not Parent's SetPropertys() */
        MsgAnswer::SetPropertys();


    }

public:
    MsgAnswer(){
        //TODO Init or not
        //MsgBase::Init();
        MsgStart();
    }
    MsgAnswer(int source,int toid,int result,string result_rtmp){
        MsgStart();
         id=source;    // source
         toId=toid;    // camera 2^28
         ret=result;
         resultRtmp=result_rtmp;
    }
};

#endif //IPFS_DEMO_MSGFFMPEG_H
