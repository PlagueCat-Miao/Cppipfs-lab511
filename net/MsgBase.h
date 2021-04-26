//
// Created by root on 8/6/19.
//

#ifndef SELECTLEARN_MSGBASE_H
#define SELECTLEARN_MSGBASE_H

#define bYTE unsigned char

#include <vector>
#include <string>
#include <iostream>
#include <map>

#include "CJsonObject.hpp"
using namespace std;

typedef void* (*create_Func)();


class JsonObejectBase{
protected:
    enum CenumJsonTypeMap{
        asInt =1,
        asInts ,
        asString,
        asStrings,
        ERR

    };

    struct classPropertyVectors{
    vector<string>              m_MemberName;
    vector<CenumJsonTypeMap>    m_MemberType;
    vector<void*>               m_MemberPropertyAddr;
    vector<int>                 m_MemberLength;
    } m_classProperty;

    virtual void SetPropertys()=0;

    void SetProperty(string name, CenumJsonTypeMap tyep, void*addr,int size =1)
    {
        m_classProperty.m_MemberName.push_back(name);
        m_classProperty.m_MemberType.push_back(tyep);
        m_classProperty.m_MemberPropertyAddr.push_back(addr);
        m_classProperty.m_MemberLength.push_back(size);
    }
public:
    static string Serialize(classPropertyVectors m_classP);
    static bool DeSerialize(classPropertyVectors m_classP,string jsBytes);//use  m_classProperty.m_MemberPropertyAddr to return value
};

class MsgFactory{
private:
    static map<string, create_Func> *msgClassMap;

public:
    static void* GetMsgClassByName(string name){

        map<string, create_Func>::iterator it=msgClassMap->find(name);

        if(it ==msgClassMap->end()){return NULL;}
        create_Func func= it->second;

        if(!func){return NULL;}
        return func();//Execution to get class object!!
    }
    static void  RegistClass(string name,create_Func func){(*msgClassMap)[name]= func;}
    static void  AwakeLoad();

};


void* create_MsgBase();
class MsgBase :public  JsonObejectBase{

public:
    string protoName;

public:
    static string Encode(MsgBase msgBasd){
        string ans = JsonObejectBase::Serialize(msgBasd.m_classProperty);
        return  ans;
    }
    static MsgBase* Decode(string protoName,string bs,int offset,int count){

        MsgBase* ans=(MsgBase*)MsgFactory::GetMsgClassByName(protoName);
        if(ans==NULL){cout<<"ERR Decode no MsgBase "<<endl;}

        string jsbs;
        jsbs.append(bs,offset,count);

        DeSerialize(ans->m_classProperty,jsbs);
        return ans;
    }

    static string EncodeName(MsgBase msgBasd){
        string nameStr =msgBasd.protoName;
        int len = msgBasd.protoName.length();
        // char k[4];//==sting will copy end with '\0'=0 ,themwill not copy \0
        // k[0]=len%256;
        // k[1]=len/256;
        // k[2]=0;
        //string ans= k;
        string ans="  ";//force 2 lentgh &&not stop at '\0'
        //string ans[2]={};//can't ans+=xxxx
        ans[0]=len%256+1;
        ans[1]=len/256+1;
        ans = ans+nameStr;
        return ans;
    }
    static string DecodeName(string bs,int offset,int &count){
        count=0;
        if(offset+2>bs.length()){return "";}//at least 2 word

        int len =((bs[offset+1]-1)<<8|(bs[offset]-1));

        if(offset +2+len > bs.length()){return "";}//we lost
        count = 2+len;

        string ans;
        ans.append(bs,offset+2,len);

        return ans;

    }

protected:
    void SetPropertys(){
        SetProperty("protoName",asString,&protoName);
    }
public:
    static void MsgAwake(){

        //TODO regist Msgname for reflection/interface
        /* build/find void* create_MsgXXX() */
        /* this function regist in MsgBase.cpp -> MsgFactory::AwakeLoad() */
        create_Func func =create_MsgBase;
        MsgFactory::RegistClass("MsgBase",func);
        cout<<"MsgBase"<<" regist"<<endl;

    }

    virtual void MsgStart(){
        //TODO set value
        // this funcion will active in Construcort == MsgXXX()
        protoName="MsgBase";

        //TODO  SetProperty regist_JSON_Property
        /* ifnot use MsgXXXX::SetPropertys() , [ this->SetPropertys(); is not work because 'this' only is Object(dui xiang)]
         * child(class) will use their SetPropertys() [ Parent::init(){SetPropertys(child's)} ]
         * but not Parent's SetPropertys() */
        MsgBase::SetPropertys();

    }


};





#endif //SELECTLEARN_MSGBASE_H
