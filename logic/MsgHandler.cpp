//
// Created by root on 8/11/19.
//

#include "MsgHandler.h"
#include "../net/NetManager.h"
#include "../net/MsgBase.h"
#include "../json/MsgMove.h"
#include "../json/MsgIPFS.h"
#include "../json/MsgFFmpeg.h"
#include "../ipfs/IPFSCtrl.h"
#include "../db/MySQL.h"
#include <time.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

void MsgHandler::OnMsgMove(ClientState* cs, MsgBase* msg)
{
    MsgMove *msg2_p =(MsgMove*) msg;
    cout<< "MsgHandler: On"<<msg2_p->protoName<<endl;

    cout<< "{x:" <<msg2_p->x<<" my_Attrs[1]: "<<(msg2_p->m_Attrs)[1];
    cout<<" }"<<endl;

    msg2_p->x++;

    NetManager::Send(cs,msg2_p);
    delete msg2_p;
}

void MsgHandler::OnMsgLogin(ClientState* cs, MsgBase* msg)
{
    MsgLogin *msg2_p =(MsgLogin*) msg;

    cout<< "MsgHandler: On"<<msg2_p->protoName<<endl;
    if( msg2_p->result!=0){cout<<"err login"<<endl; delete msg2_p;return ;}
    // TODO save!!!
    cs->id=msg2_p->id;
    cs->identity=msg2_p->identity;
    cs->ipfsHash=msg2_p->ipfsHash;
    msg2_p->result=1;

    cout<<IPFSCtrl::Connect(cs->m_addr,cs->ipfsHash)<<endl;

    NetManager::Send(cs,msg2_p);

    cout<<"now regist clients"<<endl;
    for(int i=0;i<=NetManager::max_clientIdx;i++){
        if(NetManager::clients[i].fd==-1)continue;
        cout<<"{"<<endl;
        cout<<"    fd "<<NetManager::clients[i].fd<<endl;
        cout<<"    ip "<<NetManager::clients[i].m_addr<<": "<<NetManager::clients[i].m_port<<endl;
        cout<<"    id "<<NetManager::clients[i].id;
        if(NetManager::clients[i].identity==1) cout<<": "<<"admin"<<endl;
        if(NetManager::clients[i].identity==2) cout<<": "<<"gateway"<<endl;
        if(NetManager::clients[i].identity==3) cout<<": "<<"edge"<<endl;
        cout<<"    ipfsHash "<<NetManager::clients[i].ipfsHash<<endl;
        cout<<"}"<<endl;
    }
    cout<<"max_clientIdx "<<NetManager::max_clientIdx<<endl;
    delete msg2_p;

}

void MsgHandler::OnMsgPut(ClientState* cs, MsgBase* msg){
    MsgPut *msg2_p =(MsgPut*) msg;

    cout<< "MsgHandler: On"<<msg2_p->protoName<<endl;

    FileState fs (msg2_p->id,msg2_p->ipfsHash,msg2_p->time);

    // TODO  MySQL::files_video-> MySQL::insent&query
   // MySQL::files_video->push_back(*fs);
    MySQL::insent(&fs);

    msg2_p->result=1;
    NetManager::Send(cs,msg2_p);

    vector<FileState> files_video;
    if(!MySQL::query_All(&files_video)){
        cout<<"ERR MsgHandler::OnMsgPut - MySQL::query_All"<<endl;
        delete msg2_p;
        return ;
    };

    int num = files_video.size();
    cout<<"now regist files: "<<num<<endl;
    for(int i=0;i<num;i++){
        FileState temp = files_video[i];

        cout<<"{"<<endl;
        cout<<"    id "<<temp.id<<endl;
        cout<<"    ipfshash "<<temp.ipfsHash<<endl;
        tm *ltm = localtime(&temp.time);
        cout<<"    time "<<1900+ltm->tm_year<<"//";
        cout<<ltm->tm_mon<<"//";
        cout<<ltm->tm_mday<<" ";
        cout<<ltm->tm_hour<<":";
        cout<<ltm->tm_min<<":";
        cout<<ltm->tm_sec<<endl;

        cout<<"}"<<endl;
    }
    delete msg2_p;
    return ;
}

void MsgHandler::OnMsgGet(ClientState* cs, MsgBase* msg){

    MsgGet *msg2_p =(MsgGet*) msg;
    cout<< "MsgHandler: On"<<msg2_p->protoName<<endl;
    if(cs->identity!=1)
    {
        cout<<"err Get you are not admin"<<endl;
        msg2_p->QA=0;
        NetManager::Send(cs,msg2_p);
        delete msg2_p;
        return ;
    }
    int request_int=std::atoi(msg2_p->request.c_str());
    //string request_str =msg2_p->request;

    vector<FileState> find_ans;
    if(msg2_p->QA==1){
        if(!MySQL::query_id(&find_ans,request_int)){
            cout<<"ERR MsgHandler::OnMsgPut - MySQL::query_id"<<endl;
            delete msg2_p;
            return ;
        }
    }
    // else if(msg2_p->QA==2)// ipfshash
    else if(msg2_p->QA==3)// timestamp
    {

    }
    else if(msg2_p->QA==4)// query_All
    {
        if(!MySQL::query_All(&find_ans)){
            cout<<"ERR MsgHandler::OnMsgPut - MySQL::query_All"<<endl;
            delete msg2_p;
            return ;
        }
    }

    if(find_ans.size()==0)
    {
        cout<<"err Get file not be finded"<<endl;
        msg2_p->QA=0;
        NetManager::Send(cs,msg2_p);
        delete msg2_p;
        return ;
    }
    // TODO 1. save FileState /FileStates tofile
    string file_Tags;
    file_Tags="../ipfs/tempIP/"+cs->m_addr+":"+cs->m_port+".txt";
    ofstream output(file_Tags.c_str(), ios::out );
    if(!output)
    {
        cerr <<"OnMsgGet ifstream output Open fail";
        delete msg2_p;
        return ;
    }
    cout<<" Get file find "<<find_ans.size()<<" files"<<endl;
    for(int i=0;i<find_ans.size();i++)
    {

        //cout<<(find_ans[i])->id;
        //cout<<"|"<<(find_ans[i])->ipfsHash;
        //cout<<(find_ans[i])->time<<endl;

        char temp[100];
        //string k;
        sprintf(temp,"%d",find_ans[i].id);
        output<<temp;
      //  cout<<temp;
        output<<"|"<<find_ans[i].ipfsHash;
       // cout<<"|"<<find_ans[i]->ipfsHash;
        cout<<find_ans[i].ipfsHash<<endl;
        sprintf(temp,"%d",(int)find_ans[i].time);
        output<<"|"<<temp<<"\n";
      //  cout<<"|"<<temp<<"\n";

    }
    output<<";";
    output.close();

    // TODO 2. put file to ipfs
    file_Tags=IPFSCtrl::Put(file_Tags);

    // TODO 3. send file ipfs_hash
    msg2_p->QA=1;
    msg2_p->ansList_IpfsHash=file_Tags;
    NetManager::Send(cs,msg2_p);
    delete msg2_p;
    return ;
}

void MsgHandler::OnMsgCallFor(ClientState* cs, MsgBase* msg){

    MsgCallFor *msg2_p =(MsgCallFor*) msg;
    cout<< "MsgHandler: On"<<msg2_p->protoName<<endl;

    if(cs->identity!=1)
    {
        cout<<"err MsgCallFor: Get you are not admin"<<endl;
        msg2_p->ret=0;
        NetManager::Send(cs,msg2_p);
        delete msg2_p;
        return ;
    }
    // 0 err 1 start 2 wait 3 complete 4 stop
    if(msg2_p->ret==0)
    {
        cout<<"err MsgCallFor: Unexpected error (check sever)"<<endl;
        delete msg2_p;
        return ;
    }
    else if(msg2_p->ret==1)
    {

        ClientState* toCS=NetManager::FindCSByID(msg2_p->toId);
        if(toCS==NULL)
        {
            cout<<"err MsgCallFor: device not find"<<endl;
            msg2_p->ret=0;
            NetManager::Send(cs,msg2_p);
            delete msg2_p;
            return ;
        }
        else
        {
            cout<<"world call for wetwork"<<endl;
            msg2_p->ret=2;
            NetManager::Send(cs,msg2_p);

        }
        MsgAnswer msg3(msg2_p->id,msg2_p->toId,1,"/live/live");
        NetManager::Send(toCS,&msg3);
    }
    /*
    else if(msg2_p->ret==2)
    {
        cout<<"Wait device rtmp....."<<endl;
        //TODO client count time
        return ;
    }
    else if(msg2_p->ret==3)
    {
        cout<<"rtmp Complete "<<endl;
        //TODO client open_vlc
        return ;
    }
     */
    else if(msg2_p->ret==4)
    {
        //TODO stop
        cout<<"stop device rtmp"<<endl;
        ClientState* toCS=NetManager::FindCSByID(msg2_p->toId);
        if(toCS==NULL)
        {
            cout<<"err MsgCallFor: device not find 2"<<endl;
            msg2_p->ret=0;
            NetManager::Send(cs,msg2_p);
            delete msg2_p;
            return ;
        }
        MsgAnswer msg3(msg2_p->id,msg2_p->toId,4,"");
        NetManager::Send(toCS,&msg3);
        delete msg2_p;
        return ;
    }
    else
    {
        cout<<"not for sever. ret number:"<<msg2_p->ret<<endl;
    }
    delete msg2_p;
    return ;

}
void MsgHandler::OnMsgAnswer(ClientState *cs, MsgBase *msg)
{
    MsgAnswer *msg2_p =(MsgAnswer*) msg;
    cout<< "MsgHandler: On"<<msg2_p->protoName<<endl;
    // 0 err 1 start 2 wait 3 complete 4 stop
    if(msg2_p->ret==0)
    {
        cout<<"err MsgAnswer: Unexpected error (check sever)"<<endl;
        delete msg2_p;
        return ;
    }
    /*
   else if(msg2_p->ret==1)
    {
       //TODO camera stop rtmp
       string rtmp="rtmp://";
       rtmp.append(cs->m_addr);
       rtmp.append(msg2_p->resultRtmp);
       cout<<"rtmp====>"<<rtmp<<endl;
       //TODO 推流


       msg2_p->ret=3;
       msg2_p->resultRtmp=rtmp;
       NetManager::Send(cs,msg2_p);
    }
    else if(msg2_p->ret==4)
    {
        //TODO camera stop rtmp
    }*/
    else if(msg2_p->ret==3)
    {
        MsgCallFor msg3(msg2_p->id,msg2_p->toId,3,msg2_p->resultRtmp);
        cout<<"We Answer !"<<endl;
        ClientState* toCS=NetManager::FindCSByID(msg2_p->id);
        if(toCS==NULL)
        {
            cout<<"err MsgAnswer: source not find"<<endl;
            msg2_p->ret=0;
            NetManager::Send(cs,msg2_p);
            delete msg2_p;
            return ;
        }
        NetManager::Send(toCS,&msg3);
    }
    else
    {
        cout<<"not for sever. ret number:"<<msg2_p->ret<<endl;
    }
    delete msg2_p;
    return ;
}