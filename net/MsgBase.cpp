//
// Created by root on 8/6/19.
//
#include <vector>
#include <string>
#include <iostream>
#include "CJsonObject.hpp"
#include "MsgBase.h"
#include <iostream>
#include <vector>
#include "../json/MsgMove.h"
#include "../json/MsgIPFS.h"
#include "../json/MsgFFmpeg.h"
using namespace std;

map<string, create_Func> *MsgFactory::msgClassMap= new map<string, create_Func>();


void MsgFactory::AwakeLoad()
{
    // TODO regist MsgXXX::MsgAwake
    MsgBase::MsgAwake();
    MsgMove::MsgAwake();
    MsgLogin::MsgAwake();
    MsgPut::MsgAwake();
    MsgGet::MsgAwake();
    MsgCallFor::MsgAwake();
    MsgAnswer::MsgAwake();
}

string JsonObejectBase::Serialize(JsonObejectBase::classPropertyVectors m_classP) {
    neb::CJsonObject oJson;
    int nSize = m_classP.m_MemberName.size();
    for(int i=0;i<nSize;i++)
    {
        void * pAddr =m_classP.m_MemberPropertyAddr[i];
        switch(m_classP.m_MemberType[i])
        {
            case asInt :
                oJson.Add(m_classP.m_MemberName[i], (*(int*)pAddr));
                break;
            case asString :
                oJson.Add(m_classP.m_MemberName[i], (*(string*)pAddr));
                break;
            case asStrings :
                //pAddr is vector<string> * -> vsAddr
                vector<string> *vsAddr;
                vsAddr=(vector<string>*)pAddr;
                for(int i =0;i<vsAddr->size();i++)
                {
                    oJson.Add(m_classP.m_MemberName[i],(*vsAddr)[i]);
                }
                break;
            default:
                cout<<"err JsonObejectBase::Serialize-null type"<<endl;
                break;

        }

    }
    string ans = oJson.ToString();
    return ans;
}
bool JsonObejectBase::DeSerialize(JsonObejectBase::classPropertyVectors m_classP, string bs) {


    //bs+="\0";//break;
    neb::CJsonObject iJson(bs);
    int nSize = m_classP.m_MemberName.size();
    for(int i=0;i<nSize;i++)
    {
        void * pAddr =m_classP.m_MemberPropertyAddr[i];
        string name = m_classP.m_MemberName[i];


        switch(m_classP.m_MemberType[i])
        {
            case asInt :
                iJson.Get( m_classP.m_MemberName[i],(*(int*)pAddr));
                break;
            case asString:
                iJson.Get( m_classP.m_MemberName[i],(*(string*)pAddr));
                break;
            case asStrings:
                int vsSize;
                vsSize=iJson[m_classP.m_MemberName[i]].GetArraySize();
                for(int i=0;i<vsSize;i++)
                {
                    string temp;
                    iJson.Get( m_classP.m_MemberName[i],temp);
                    ((vector<string>*)pAddr)->push_back(temp);
                }
                break;
            default:
                cout<<"err JsonObejectBase::Serialize-null type"<<endl;
                return false;
                break;
        }

    }
    return true;
}

void* create_MsgBase(){MsgBase *msg= new MsgBase; return (msg==NULL)?NULL:msg;}



/*
int Josntest1_main(int argc,char * argv[]){
    //Encode
    MsgMove msg;
    msg.x=3;
    msg.y=4;
    msg.z=5;
    string a=MsgMove::Encode(msg);
    cout<< a <<endl;
    //Decode
    string bs =a;
    MsgBase*k;
    MsgMove* msg2_p;
    k=MsgBase::Decode("MsgMove",bs,0,bs.length());
    msg2_p=(MsgMove*)k;
    cout<< "protoname:" <<msg2_p->protoName<<endl;
    cout<< "x:" <<msg2_p->x<<endl;
    cout<< "y:" <<msg2_p->y<<endl;
    cout<< "z:" <<msg2_p->z<<endl;
}
 */
/*
int main(int argc,char * argv[]){
    //Encode
    MsgMove msg;
    msg.x=100;
    msg.y=-20;
    msg.z=0;


    string nameBytes =MsgMove::EncodeName(msg);
    string bodyBytes=MsgMove::Encode(msg);

    int len =nameBytes.length()+bodyBytes.length();

    string sendBytes="  ";//force 2 lentgh &&not stop at '\0'
    sendBytes[0]=len%256;
    sendBytes[1]=len/256;

    sendBytes+=nameBytes;
    sendBytes+=bodyBytes;


    cout <<sendBytes<<endl;
    cout <<len<<endl;

    for (int i = 0; i < 4; ++i) {
        cout <<"miao:"<<(int)sendBytes[i]<<endl;
    }

    //Decode
    string bs =bodyBytes;
    MsgBase*k;
    MsgMove* msg2_p;
    k=MsgBase::Decode("MsgMove",bs,0,bs.length());
    int k3=0;
    string k2=MsgBase::DecodeName(sendBytes,2,k3);

    msg2_p=(MsgMove*)k;
    cout<< "protoname:" <<k2<<endl;
    cout<< "x:" <<msg2_p->x<<endl;
    cout<< "y:" <<msg2_p->y<<endl;
    cout<< "z:" <<msg2_p->z<<endl;
}
*/