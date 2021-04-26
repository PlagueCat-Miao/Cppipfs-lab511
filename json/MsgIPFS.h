//
// Created by root on 8/28/19.
//

#ifndef SELECTLEARN_MSGIPFS_H
#define SELECTLEARN_MSGIPFS_H

#include <string>
#include <iostream>
#include "../net/CJsonObject.hpp"
#include "../net/MsgBase.h"
#include <iostream>


/*
enum IdentityList{
    asAdmin =1,
    asGateWay ,
    asEdge,
    asNull
};
*/


extern void* create_MsgLogin();
extern void* create_MsgPut();
extern void* create_MsgGet();


class MsgLogin :public  MsgBase {
public:
    //string protoName;
    int id;         // 2^28
    int identity;   // 1 admin 2 GateWay 3 Edge
    int result;     // 0 send 1 ack
    string ipfsHash;
protected:
    void SetPropertys(){
        SetProperty("protoName",asString,&protoName);
        SetProperty("identity",asInt,&identity);
        SetProperty("id",asInt,&id);
        SetProperty("result",asInt,&result);
        SetProperty("ipfsHash",asString,&ipfsHash);

    }
public:

    static void MsgAwake(){

        //TODO regist Msgname for reflection/interface
        /* build/find void* create_MsgXXX() */
        /* this function regist in MsgBase.cpp -> MsgFactory::AwakeLoad() */
        create_Func func =create_MsgLogin;
        MsgFactory::RegistClass("MsgLogin",func);
        cout<<"MsgLogin"<<" regist"<<endl;

    }
    virtual void MsgStart(){
        //TODO set value
        // this funcion will active in Construcort == MsgXXX()
        protoName="MsgLogin";
        id=-4;
        identity=-4;
        result=-4;

        //TODO  SetProperty regist_JSON_Property
        /* ifnot use MsgXXXX::SetPropertys() , [ this->SetPropertys(); is not work because 'this' only is Object(dui xiang)]
         * child(class) will use their SetPropertys() [ Parent::init(){SetPropertys(child's)} ]
         * but not Parent's SetPropertys() */
        MsgLogin::SetPropertys();


    }

public:
    MsgLogin(){
        //TODO Init or not
        //MsgBase::Init();
        MsgStart();
    }
};

class MsgPut :public  MsgBase {
public:
    //string protoName;
    int id;         // 2^28
    int result;     // 0 send 1 ack
    string ipfsHash;
    int time;

protected:
    void SetPropertys(){
        SetProperty("protoName",asString,&protoName);
        SetProperty("id",asInt,&id);
        SetProperty("result",asInt,&result);
        SetProperty("ipfsHash",asString,&ipfsHash);
        SetProperty("time",asInt,&time);
    }
public:

    static void MsgAwake(){

        //TODO regist Msgname for reflection/interface
        /* build/find void* create_MsgXXX() */
        /* this function regist in MsgBase.cpp -> MsgFactory::AwakeLoad() */
        create_Func func =create_MsgPut;
        MsgFactory::RegistClass("MsgPut",func);
        cout<<"MsgPut"<<" regist"<<endl;

    }
    virtual void MsgStart(){
        //TODO set value
        // this funcion will active in Construcort == MsgXXX()
        protoName="MsgPut";
        id=-4;
        time=-4;
        result=-4;

        //TODO  SetProperty regist_JSON_Property
        /* ifnot use MsgXXXX::SetPropertys() , [ this->SetPropertys(); is not work because 'this' only is Object(dui xiang)]
         * child(class) will use their SetPropertys() [ Parent::init(){SetPropertys(child's)} ]
         * but not Parent's SetPropertys() */
        MsgPut::SetPropertys();


    }

public:
    MsgPut(){
        //TODO Init or not
        //MsgBase::Init();
        MsgStart();
    }
};

class MsgGet :public  MsgBase {
public:
    //string protoName;
    /* TODO QA
     *  sever: 0 lose 1 have
     *  client: 1 getby_deviceid 2 getby_IpfsHash 3 getby_time 4 show_all
     * */
    int QA;
    string request;
    int id;         // 2^28
    string ansList_IpfsHash; //47B

protected:
    void SetPropertys(){
        SetProperty("protoName",asString,&protoName);
        SetProperty("QA",asInt,&QA);
        SetProperty("id",asInt,&id);
        SetProperty("request",asString,&request);
        SetProperty("ansList_IpfsHash",asString,&ansList_IpfsHash);

    }
public:

    static void MsgAwake(){

        //TODO regist Msgname for reflection/interface
        /* build/find void* create_MsgXXX() */
        /* this function regist in MsgBase.cpp -> MsgFactory::AwakeLoad() */
        create_Func func =create_MsgGet;
        MsgFactory::RegistClass("MsgGet",func);
        cout<<"MsgGet"<<" regist"<<endl;

    }
    virtual void MsgStart(){
        //TODO set value
        // this funcion will active in Construcort == MsgXXX()
        protoName="MsgGet";


        //TODO  SetProperty regist_JSON_Property
        /* ifnot use MsgXXXX::SetPropertys() , [ this->SetPropertys(); is not work because 'this' only is Object(dui xiang)]
         * child(class) will use their SetPropertys() [ Parent::init(){SetPropertys(child's)} ]
         * but not Parent's SetPropertys() */
        MsgGet::SetPropertys();


    }

public:
    MsgGet(){
        //TODO Init or not
        //MsgBase::Init();
        MsgStart();
    }
};


#endif //SELECTLEARN_MSGIPFS_H
