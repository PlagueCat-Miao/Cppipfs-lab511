//
// Created by root on 9/11/19.
//

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

using namespace std;

#ifndef COMMON2_H_
#define COMMON2_H_
const int BACKLOG=5;           //listen队列中等待的连接数
const int MAXDATASIZE=1024;    //缓冲区大小
int PORT=2248;           //服务器端口
char defaultHostName[20] = "127.0.0.1";

#endif

#ifndef CLIENT_ATTRIBUTE_
#define CLIENT_ATTRIBUTE_
int m_id=222;
int m_identity=1;
#endif



void get_test(ClientState cs,string src);
vector<string> Split(string list_src, char A);

int main(int argc,char * argv[]){

    if(argc >=2){strcpy(defaultHostName,argv[1]);}
    if(argc >=3)
    {
        int i=0;PORT=0;
        cout<<argv[2]<<endl;
        while( argv[2][i]!='\0')
        {
            PORT*=10;
            PORT+=argv[2][i]-'0';
            i++;
        }
    }

    char hostname[100];
    gethostname(hostname,sizeof(hostname));
    int sockfd, numbytes;
    struct hostent *he;
    struct sockaddr_in serv_addr;
    if ((he = gethostbyname(defaultHostName)) == NULL)
    {
        std::cout<<"gethostbyname error!"<<std::endl;
        return 0;;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cout<< "socket error!"<<std::endl;
        return 0;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(serv_addr.sin_zero), 8);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1)
    {
        std::cout<<"connect error!"<<std::endl;
        return 0;
    }

    MsgFactory::AwakeLoad();
    NetManager::NetManagerAwake();

    MsgLogin msg;
    MsgLogin *ans;
    msg.id = m_id;
    msg.identity = m_identity;
    msg.ipfsHash=NetManager::m_ipfsHash;
    msg.result = 0;//send not ack

    ClientState cs;
    cs.fd=sockfd;
    cs.m_addr=defaultHostName;
    cs.m_port=to_string(PORT);

    cout<<" different processes,different sockfd-stack. This sockfd():"<<sockfd<<endl;
    cout<<" so in different processes,same sockfd(int),different Net-System-File. "<<endl;
    cout<<" MyID:"<<msg.id;
    if(msg.identity==1) cout<<" : "<<"admin"<<endl;
    if(msg.identity==2) cout<<" : "<<"gateway"<<endl;
    if(msg.identity==3) cout<<" : "<<"edge"<<endl;

    NetManager::Send(&cs,&msg);
    char buf[MAXDATASIZE];
    if ((numbytes = read(sockfd, buf, MAXDATASIZE)) == -1)
    {
        std::cout<<"read error!\n"<<std::endl;
        return 0;
    }
    buf[numbytes]='\0';
    std::cout<<"Client Recv: "<<buf <<std::endl;
    ans=(MsgLogin*)NetManager::ReceiveMsg(&cs,buf);
    if(ans->result==1){cout<<"login OK"<<endl;}

    get_test(cs,"../ipfs/file/Hellcat.txt");


    while (1);
    close(sockfd);
    return 0;
}

void get_test(ClientState cs,string file_source)
{
    int numbytes;
    MsgGet msg;
    MsgGet *ans;

    msg.id=m_id;
    msg.QA=1;
    msg.request="251";

    NetManager::Send(&cs,&msg);
    char buf[MAXDATASIZE];
    if ((numbytes = read(cs.fd, buf, MAXDATASIZE)) == -1)
    {
        std::cout<<"read error!\n"<<std::endl;
        return ;
    }
    buf[numbytes]='\0';
    std::cout<<"Client Recv: "<<buf <<std::endl;
    ans=(MsgGet*)NetManager::ReceiveMsg(&cs,buf);

    if(ans->QA!=1){cout<<"Get not find"<<endl; return;}

    string list_src=IPFSCtrl::Cat(ans->ansList_IpfsHash);
    cout<<"Get Cat :"<<endl<<list_src<<endl;
    vector<string> list=Split(list_src,'\n');
    if(list.empty()){cout<<"Get Split err"<<endl; return;}
    // I need ALL;
    for(int i=0;i<list.size();i++){
        if(list[i][0]==';')break;
        cout<<"index :"<<i<<" >"<<list[i]<<endl;

        vector<string> Attr=Split(list[i],'|');


        cout<<"Download :"<<Attr[1]<<";";
        cout<<Attr[0]+":"+Attr[1]+":"+Attr[2]+".txt"<<endl;


        if(!IPFSCtrl::Download(Attr[1],Attr[0]+"-"+Attr[1]+"-"+Attr[2]+".txt"))
        {
            cout<<"Get Download err :"<<Attr[0];
            cout<<": "<<Attr[1]<<": "<<Attr[2] <<endl;
        }
    }
    cout<<"Get OK"<<endl;
    return;

}

vector<string> Split(string list_src, char A){
    vector<string> res;
   // cout<<"Split :"<<list_src<<endl;


    char *str= new char[list_src.size()+1];
    strcpy(str,list_src.c_str());

    //char *strk=(char*)list_src.c_str();

    //cout<<"!!!!"<<strk<<endl;
    //strk[0]='@';
    //cout<<"!!!!"<<strk<<endl;
    //cout<<"!!!!"<<list_src<<endl;

    char *p=strtok(str,&A);
    while(p)
    {
        string s=p;
      //  cout<<">"<<s<<endl;
        res.push_back(s);
        p=strtok(NULL,&A);
    }
    delete str;

    return res;
}

