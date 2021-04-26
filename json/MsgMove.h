//
// Created by root on 8/8/19.
//

#ifndef SELECTLEARN_MSGTEST_H
#define SELECTLEARN_MSGTEST_H

#include <string>
#include <iostream>
#include "../net/CJsonObject.hpp"
#include "../net/MsgBase.h"
#include <iostream>
#include <vector>

extern void* create_MsgMove();
class MsgMove :public  MsgBase {
public:
     //string protoName;
     int x;
     int y;
     int z;
     vector<string> m_Attrs;
protected:
    void SetPropertys(){
        SetProperty("protoName",asString,&protoName);
        SetProperty("x",asInt,&x);
        SetProperty("y",asInt,&y);
        SetProperty("z",asInt,&z);
        SetProperty("m_Attrs",asStrings,&m_Attrs);
    }
public:

    static void MsgAwake(){

        //TODO regist Msgname for reflection/interface
        /* build/find void* create_MsgXXX() */
        /* this function regist in MsgBase.cpp -> MsgFactory::AwakeLoad() */
        create_Func func =create_MsgMove;
        MsgFactory::RegistClass("MsgMove",func);
        cout<<"MsgMove"<<" regist"<<endl;

    }
    virtual void MsgStart(){

        //TODO set value
        // this funcion will active in Construcort == MsgXXX()
        protoName="MsgMove";
        x=3;
        y=4;
        z=5;
        m_Attrs.push_back("myName: MsgMove");
        m_Attrs.push_back("myTeam: Red");
        m_Attrs.push_back("my_id: PlagueCat");
        m_Attrs.push_back("Personal Signature: my tank name is Hellcat");
        //TODO  SetProperty regist_JSON_Property
        /* ifnot use MsgXXXX::SetPropertys() , [ this->SetPropertys(); is not work because 'this' only is Object(dui xiang)]
         * child(class) will use their SetPropertys() [ Parent::init(){SetPropertys(child's)} ]
         * but not Parent's SetPropertys() */
        MsgMove::SetPropertys();


    }

public:
    MsgMove(){
        //TODO Init or not
        //MsgBase::Init();
        MsgStart();
    }
};



#endif //SELECTLEARN_MSGTEST_H
