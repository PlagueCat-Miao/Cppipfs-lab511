//
// Created by hellcat on 2019/11/20.
//

#include "client-normal.h"
#include <iostream>
#include <fstream>

using namespace std;

#ifndef COMMON2_H_
#define COMMON2_H_
const int BACKLOG=5;           //listen队列中等待的连接数
const int MAXDATASIZE=1024;    //缓冲区大小
int PORT=2248;                 //服务器端口
char defaultHostName[20] = "127.0.0.1";

#endif

#ifndef CLIENT_ATTRIBUTE_
#define CLIENT_ATTRIBUTE_
int m_id=222;
int m_identity=1;
ClientState cs_CtoS;
easy_Netmanger eNet;
#endif

#define _DEBU

int main(int argc,char * argv[]) {

    if(argc >=2){m_id=atoi(argv[1]);}
    if(argc >=3){strcpy(defaultHostName,argv[2]);}
    if(argc >=4)
    {
        int i=0;PORT=0;
        cout<<argv[3]<<endl;
        while( argv[3][i]!='\0')
        {
            PORT*=10;
            PORT+=argv[3][i]-'0';
            i++;
        }
    }

    MsgFactory::AwakeLoad();
    NetManager::NetManagerAwake();

    if(!eNet.Init_net()){ return 0;}
    if(!Login()){ return 0;}


    while (1)
    {
        print_help();
        int cmd=0;
        cin>>cmd;
        switch (cmd)
        {
            case 0:
                cmd=-4;
                break;
            case 1:
                put_test();
                break;
            case 2:
                show_datalist();
                break;
            case 3:
                get_test();
                break;
            case 4:
                CallforWetwork(rtmp_runing);
                break;
            default:
                cout<<"Unknown command :"<<cmd<<endl;
                break;
        }
        if(cmd==-4){break;}
    }

    return 0;


}

bool easy_Netmanger::Init_net()
{
    char hostname[100];
    gethostname(hostname,sizeof(hostname));

    struct hostent *he;
    struct sockaddr_in serv_addr;
    if ((he = gethostbyname(defaultHostName)) == NULL)
    {
        std::cout<<"gethostbyname error!"<<std::endl;
        return false;;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cout<< "socket error!"<<std::endl;
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(serv_addr.sin_zero), 8);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1)
    {
        std::cout<<"connect error!"<<std::endl;
        return false;
    }

    return true;
}
MsgBase* easy_Netmanger::encode_recv_data(ClientState *client)
{
    MsgBase* ans;
    char buf[MAXDATASIZE];
    if ((numbytes = read(sockfd, buf, MAXDATASIZE)) == -1)
    {
        std::cout<<"read error!\n"<<std::endl;
        return 0;
    }
    buf[numbytes]='\0';
    std::cout<<"Client Recv: "<<buf <<std::endl;

    return NetManager::ReceiveMsg(client,buf);
}

void print_help()
{
    for(int i=0;i<5;i++)
        cout << endl;
    cout<<"MyID:"<<m_id;
    if(m_identity==1) cout<<" : "<<"admin"<<endl;
    if(m_identity==2) cout<<" : "<<"gateway"<<endl;
    if(m_identity==3) cout<<" : "<<"edge"<<endl;
    cout <<"<=======================================>"<<endl;
    cout <<"0. Exit"<<endl;
    cout <<"1. Put files to IPFS-Net"<<endl;
    cout <<"2. Show all files in IPFS-Net"<<endl;
    cout <<"3. Get files from IPFS-Net by device-ID"<<endl;
    if(rtmp_runing==false){cout <<"4. Call for wetwork by device-ID"<<endl;}
    else{cout <<"4. Stop wetwork"<<endl;}

    cout <<"Please enter the command number"<<endl;
    return ;
}
bool Login()
{
    MsgLogin msg;
    msg.id = m_id;
    msg.identity = m_identity;
    msg.ipfsHash=NetManager::m_ipfsHash;
    msg.result = 0;//send not ack

    cs_CtoS.fd=eNet.sockfd;
    cs_CtoS.m_addr=defaultHostName;
    cs_CtoS.m_port=to_string(PORT);

    MsgLogin *ans;
    cout<<" MyID:"<<msg.id;
    if(msg.identity==1) cout<<" : "<<"admin"<<endl;
    if(msg.identity==2) cout<<" : "<<"gateway"<<endl;
    if(msg.identity==3) cout<<" : "<<"edge"<<endl;

    NetManager::Send(&cs_CtoS,&msg);

    ans=(MsgLogin*)eNet.encode_recv_data(&cs_CtoS);
    if(ans->result==1)
    {
        cout<<"login OK"<<endl;
        delete ans;
        return true;
    }
    return false;
}
bool put_test()
{
    string file_source;
    cout <<"Please enter the file Path"<<endl;
    cin>>file_source;
    fstream _file;
    _file.open(file_source, ios::in);
    if(!_file) {
        cout<<"\""<<file_source<<"\""<<"没有被创建!"<<endl;
        cout<<"使用测试文件 "<<"\"../ipfs/file/Hellcat.txt\""<<endl;
        file_source="../ipfs/file/Hellcat.txt";
        _file.close();
        _file.clear();
    }
    else{
        _file.close();
        _file.clear();
    }

    MsgPut msg;
    MsgPut *ans;

    string putans=IPFSCtrl::Put(file_source);
    msg.id=m_id;
    msg.ipfsHash=putans;
    msg.time=time(0);

    NetManager::Send(&cs_CtoS,&msg);
    ans=(MsgPut*)eNet.encode_recv_data(&cs_CtoS);//have new
    if(ans->result==1)
    {
        cout<<"Put OK"<<endl;
        delete ans;
        return true;
    }

    return false;
}
bool show_datalist()
{
    MsgGet msg;
    MsgGet *ans;

    msg.id=m_id;
    msg.QA=4;
    msg.request="";

    NetManager::Send(&cs_CtoS,&msg);
    ans=(MsgGet*)eNet.encode_recv_data(&cs_CtoS);//have new

    if(ans->QA!=1)
    {
        cout<<"Get not find"<<endl;
        delete ans;
        return false;
    }

    string list_src=IPFSCtrl::Cat(ans->ansList_IpfsHash);

    vector<string> list=Split(list_src,'\n');
    if(list.empty())
    {
        cout<<"Get Split err"<<endl;
        delete ans;
        return false;
    }
    // I need ALL;
    for(int i=0;i<list.size();i++)
    {
        if(list[i][0]==';')break;
        cout<<"index :"<<i<<" >"<<list[i]<<endl;
        vector<string> Attr=Split(list[i],'|');

        cout<<"{"<<endl;
        cout<<"    id "<<Attr[0]<<endl;
        cout<<"    ipfshash "<<Attr[1]<<endl;
        time_t time=atoi(Attr[2].c_str());
        tm *ltm = localtime(&time);
        cout<<"    time "<<1900+ltm->tm_year<<"//";
        cout<<ltm->tm_mon<<"//";
        cout<<ltm->tm_mday<<" ";
        cout<<ltm->tm_hour<<":";
        cout<<ltm->tm_min<<":";
        cout<<ltm->tm_sec<<endl;

        cout<<"}"<<endl;

    }


    delete ans;
    return true;



}
bool get_test()
{
    MsgGet msg;
    MsgGet *ans;

    string request;
    cout <<"Please enter the device-ID"<<endl;
    cin>>request;

    msg.id=m_id;
    msg.QA=1;
    msg.request=request;

    NetManager::Send(&cs_CtoS,&msg);
    ans=(MsgGet*)eNet.encode_recv_data(&cs_CtoS);//have new

    if(ans->QA!=1)
    {
        cout<<"Get not find"<<endl;
        delete ans;
        return false;
    }

    string list_src=IPFSCtrl::Cat(ans->ansList_IpfsHash);
    cout<<"Get Cat :"<<endl<<list_src<<endl;
    vector<string> list=Split(list_src,'\n');
    if(list.empty())
    {
        cout<<"Get Split err"<<endl;
        delete ans;
        return false;
    }
    // I need ALL;
    for(int i=0;i<list.size();i++){
        if(list[i][0]==';')break;
        cout<<"index :"<<i<<" >"<<list[i]<<endl;

        vector<string> Attr=Split(list[i],'|');

        cout<<"Download :"<<Attr[1]<<";"<<endl;
        cout<<"OutPut   :"<<Attr[0]+":"+Attr[1]+":"+Attr[2]+".txt"<<endl;

        if(!IPFSCtrl::Download(Attr[1],Attr[0]+"-"+Attr[1]+"-"+Attr[2]+".txt"))
        {
            cout<<"Get Download err :"<<Attr[0];
            cout<<": "<<Attr[1]<<": "<<Attr[2] <<endl;
        }
    }
    cout<<"Get OK"<<endl;
    delete ans;
    return true;

}

vector<string> Split(string list_src, char A)
{
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

bool CallforWetwork(bool &isRun)
{
    static MsgCallFor *call;

    if(isRun){
        //stop

        cout<<"stop rtmp"<<endl;
        call->ret=4;
        NetManager::Send(&cs_CtoS,call);
        delete call;
    }
    else{
        //run
        int target_id;
        cout <<"Please enter the device-ID"<<endl;
        cin>>target_id;

        //send call
        MsgCallFor msg2(m_id,target_id,1,"");
        NetManager::Send(&cs_CtoS,&msg2);
        //get wait

        call=(MsgCallFor*)eNet.encode_recv_data(&cs_CtoS);
        if(call->ret==2) // 0 err 1 start 2 wait 3 complete 4 stop
        {cout<<"wetwork send"<<endl;}
        else
        {
            cout<<"err anwser :"<<call->ret<<endl;
            delete call;
            return false;
        }
        delete call;
        //get complete
        call=(MsgCallFor*)eNet.encode_recv_data(&cs_CtoS);
        if(call->ret==3) // 0 err 1 start 2 wait 3 complete 4 stop
        {
            cout<<"Answer! :"<<call->toId<<endl;
            cout<<"cause :"<<call->ret<<": "<<call->resultRtmp<<endl;
        }
        else
        {
            cout<<"err anwser2 :"<<call->ret<<endl;
            delete call;
            return false;
        }
    }
    isRun=!isRun;
    return true;


}