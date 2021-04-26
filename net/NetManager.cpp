//
// Created by root on 8/11/19.
//


#include <iostream>
#include <map>
#include "NetManager.h"
#include "../logic/MsgHandler.h"
#include "../ipfs/IPFSCtrl.h"
using namespace std;

//const int PORT=2248;           //服务器端口
const int BACKLOG=5;           //listen队列中等待的连接数
const int MAXDATASIZE=DeFAULT_SIZE;    //缓冲区大小
//const char defaultHostName[] = "127.0.0.1";

ClientState NetManager::clients[FD_SETSIZE];
int NetManager::max_clientIdx;
int NetManager::listenFd;
int NetManager::maxFd;
fd_set NetManager::allSet;
int NetManager::sin_size;
map<string, OnFunc> *NetManager::methodMsgHandlerMap= new map<string, OnFunc>();
string NetManager::m_ipfsHash;


void NetManager::NetManagerAwake()
{
    //TODO add  MsgHandler bakcall_func
    RegistMsgHandler("MsgMove", MsgHandler::OnMsgMove);
    RegistMsgHandler("MsgLogin", MsgHandler::OnMsgLogin);
    RegistMsgHandler("MsgPut", MsgHandler::OnMsgPut);
    RegistMsgHandler("MsgGet", MsgHandler::OnMsgGet);
    RegistMsgHandler("MsgCallFor", MsgHandler::OnMsgCallFor);
    RegistMsgHandler("MsgAnswer", MsgHandler::OnMsgAnswer);

    //TODO get IPFS ID/HASH
    m_ipfsHash.assign(IPFSCtrl::Getid());
}

OnFunc NetManager::GetMethodMsgHandlerByName(string name)
{

    map<string, OnFunc>::iterator it=methodMsgHandlerMap->find(name);

    if(it ==methodMsgHandlerMap->end()){
        cout<<"ERR GetMethodMsgHandlerByName NO FIND "<<name<<endl;
        return NULL;}

    OnFunc func= it->second;
    if(!func){return NULL;}

    return func;
}

void NetManager::StartLoop(int port){

//地址信息结构体大小
    if ((listenFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {    //调用socket创建用于监听客户端的socket
        std::cout<<"Creating socket failed."<<std::endl;
        return;
    }
    cout<<" hellcat .>"<<"same processes,same sockfd-stack. ";
    cout<<"This listen-sockfd :"<<listenFd<<endl;

    int opt = SO_REUSEADDR;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));    //设置socket属性
    struct sockaddr_in serverAddr;     //服务器地址信息结构体
    bzero(&serverAddr,sizeof(serverAddr));
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(port);
    serverAddr.sin_addr.s_addr = htonl (INADDR_ANY);
    if (bind(listenFd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) == -1) {    //调用bind绑定地址
        std::cout<<"Bind error."<<std::endl;
        return;
    }
    if(listen(listenFd,BACKLOG) == -1) {
//调用listen开始监听
        std::cout << "listen() error"<<std::endl;
        return;
    }
    //初始化select

    sin_size=sizeof(struct sockaddr_in);
    //ssize_t      n;

    for (int i = 0; i < FD_SETSIZE; i++) {
        clients[i].fd = -1;
        clients[i].readBuff.SetActive(false);
    }
    maxFd= listenFd;
    FD_ZERO(&allSet);        //清空
    FD_SET(listenFd, &allSet);    //将监听socket加入select检测的描述符集合

    std::cout << "sever listen at: "<< port<<endl;
    while(1){


        int nReady;// how many fd ready to duel
        fd_set readSet;//可读socket的文件描述符的集合



        readSet = allSet;
        nReady = select(maxFd+1, &readSet, NULL,NULL, NULL);    //调用select
        ReadListenfd(readSet,nReady);
        if (nReady <= 0)//如果没有新客户端连接，继续循环
        {continue;}
        ReadClientfd(readSet,nReady);
    }
    //never
    close(listenFd);


}
void NetManager::ReadListenfd(fd_set &readset,int &n_ready){

    int connectFd=0;     //socket文件描述符//new client accpect
    struct sockaddr_in addr;

    if (FD_ISSET(listenFd, &readset)) {      //检测是否有新客户端请求
        //调用accept，返回服务器与客户端连接的socket描述符
        if ((connectFd = accept(listenFd,(struct sockaddr *)&addr,(socklen_t *)&sin_size))==-1) {
            std::cout<<"accept() error"<<std::endl;
            return;//exit()
        }
        //将新客户端的加入数组
        int i = 0;
        for (i = 0; i < FD_SETSIZE; i++) {
            if (clients[i].fd < 0) {
                clients[i].fd = connectFd;    //保存客户端描述符
                clients[i].readBuff.SetActive(true);
                char addrBuf[2][32];
                sprintf(addrBuf[0],"%s",inet_ntoa(addr.sin_addr));
                sprintf(addrBuf[1],"%d",addr.sin_port);

                // clientCount++;
                clients[i].m_addr=std::string(addrBuf[0]);
                clients[i].m_port=std::string(addrBuf[1]);

                break;
            }
        }
        if (i >= FD_SETSIZE){std::cout<<"too many clients"<<std::endl; return;}

        if (i > max_clientIdx)//数组最大元素值
        { max_clientIdx = i;}

        FD_SET(connectFd, &allSet);   //将新socket连接放入select监听集合
        if (connectFd > maxFd)
        {
            maxFd = connectFd;   //确认maxfd是最大描述符
        }
        n_ready--;
    }
    else{n_ready=n_ready;}
    return ;
}
void NetManager::ReadClientfd(fd_set &readset,int &n_ready){
    int client_fd;//  client's fd

    for (int i = 0; i <= max_clientIdx; i++)
    {
        if ( (client_fd = clients[i].fd) < 0)       //如果客户端描述符小于0，则没有客户端连接，检测下一个
        {
            continue;
        }
        if (FD_ISSET(client_fd, &readset))
        {
            //clear
            ClientState *client =&clients[i];
            ByteArray *readBuff = &(client->readBuff);
            if(readBuff->remain()<=0){OnReceiveData(client);readBuff->MoveBytes();}
            if(readBuff->remain()<=0){ cout<<"ERR ReadClientfd->readBuff->remain()"<<endl;return;}

            //检测此客户端socket是否有数据
            //TODD check
            char RecvBuff[MAXDATASIZE];
            int nReadLength = recv(client_fd,RecvBuff,MAXDATASIZE,0);
            RecvBuff[nReadLength]='\0';
            if (nReadLength == 0)//Close
            { //从客户端socket读数据，等于0表示网络中断
                close(client_fd);        //关闭socket连接
                FD_CLR(client_fd, &allSet);    //从监听集合中删除此socket连接
                clients[i].fd = -1;        //数组元素设初始值，表示没客户端连接
                clients[i].readBuff.SetActive(false);
                std::cout<<"Client Addr: "<< clients[i].m_addr <<": "<<  clients[i].m_port<<"ID :"<< clients[i].id<<" Close "<<std::endl;

            }
            else
            {

                std::cout<<"RecvData From Client:"<< client->m_addr <<": "<<  clients[i].m_port<<"     length:"<<nReadLength<<"   Data:"<<RecvBuff<<std::endl;

                client->readBuff.WirteBA((unsigned char*)RecvBuff,0,nReadLength);
                // duel
                //Earlier BUG: OnReceiveData(&client[i]);
                OnReceiveData(client);
                readBuff->CheckAndMovBytes();
            }

            //如果没有新客户端有数据，跳出for循环回到while循环
            if (--n_ready <= 0){break;}
        }
    }

    return ;
}

void NetManager::OnReceiveData(ClientState *client){

    ByteArray *readBuff = &(client->readBuff);
    if(readBuff->length()<=2){std::cout<<"ERR OnReceiveData1"<< readBuff->length()<<endl;return ;}
    int bodyLength=readBuff->ReadInt16();

    if(readBuff->length()<bodyLength){std::cout<<"ERR OnReceiveData2"<< readBuff->length()<<"<"<<bodyLength<<endl;return ;}

    string bs((char*)readBuff->bytes,0,readBuff->writeIdx);
    int nameCount=0;
    string protoName=MsgBase::DecodeName(bs,readBuff->readIdx,nameCount);
    if(protoName==""){std::cout<<"ERR OnReceiveData->DecodeName"<<endl;}

    readBuff->readIdx+=nameCount;
    bodyLength-=nameCount;

    MsgBase *msgBase = MsgBase::Decode(protoName,bs,readBuff->readIdx,bodyLength);
    if(msgBase==NULL){std::cout<<"ERR OnReceiveData->Decode"<<endl;}

    readBuff->readIdx+=bodyLength;
    readBuff->CheckAndMovBytes();

    //TODO broadcast && deal && callback
    OnFunc method= GetMethodMsgHandlerByName(protoName);
    if(method==NULL){std::cout<<"ERR OnReceiveData->GetMethodMsgHandlerByName"<<endl;}
    method(client,msgBase);

    if(readBuff->length()>2){ OnReceiveData(client);}
}
void NetManager::Send(ClientState *client, MsgBase  *msg){


    if(client==NULL){cout<<"ERR Send client";return ;}
    if(msg==NULL){cout<<"ERR Send msg"; return ;}

    string nameBytes =MsgMove::EncodeName(*msg);
    string bodyBytes=MsgMove::Encode(*msg);

    int len =nameBytes.length()+bodyBytes.length();

    string sendBytes="  ";//force 2 lentgh &&not stop at '\0'
    sendBytes[0]=len%256+1;
    sendBytes[1]=len/256+1;

    sendBytes+=nameBytes;
    sendBytes+=bodyBytes;

   // char sendBuff[MAXDATASIZE];
   // sendBytes.copy(sendBuff,sendBytes.size(),0);


    cout<<"Send:    "<<client->m_addr <<": "<<client->m_port<<"     length:"<<sendBytes.size()<<"   Data:"<<msg->protoName<<endl;
    //for(int i=0;i<sendBytes.size();i++){
    //    cout<<sendBuff[i];
    // }

    send(client->fd,sendBytes.c_str(),sendBytes.length(),0);

}
MsgBase* NetManager::ReceiveMsg(ClientState *client,char* byte){

    int capacity=strlen(byte);
    int readIdx=0;
    if((capacity-readIdx)<=2){std::cout<<"ERR OnReceiveData1"<< (capacity-readIdx)<<endl;return NULL;}
    int bodyLength=ByteArray::ReadInt16(byte);
    readIdx+=2;
    if((capacity-readIdx)<bodyLength){std::cout<<"ERR OnReceiveData2"<< (capacity-readIdx)<<"<"<<bodyLength<<endl;return NULL;}

    string bs(byte,0,capacity);
    int nameCount=0;
    string protoName=MsgBase::DecodeName(bs,readIdx,nameCount);
    if(protoName==""){std::cout<<"ERR OnReceiveData->DecodeName"<<endl;}
    readIdx+=nameCount;
    bodyLength-=nameCount;

    MsgBase *msgBase = MsgBase::Decode(protoName,bs,readIdx,bodyLength);
    if(msgBase==NULL){std::cout<<"ERR OnReceiveData->Decode"<<endl;}

    cout<<"plague .> "<<msgBase->protoName<<endl;//Earlier BUG: msgBase=NULL;

    readIdx+=bodyLength;
    /*
     //TODO broadcast && deal && Callback
     MsgMove * msg2=(MsgMove *)msgBase;
     cout<<msg2->protoName<<" :"<<endl;
     cout<<"{x:"<<msg2->x<<"  y: "<<msg2->y<<"  z: "<<msg2->z<<"}"<<endl;


     if((capacity-readIdx)>2){ ReceiveMsg(client,byte+readIdx);}
    */
    return msgBase;
}