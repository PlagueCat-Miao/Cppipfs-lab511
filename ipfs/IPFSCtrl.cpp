//
// Created by root on 8/26/19.
//

#include "IPFSCtrl.h"
#include <iostream>
#include <stdio.h>
#include <string>

#include <fstream>

using namespace std;

string IPFSCtrl::cmd_Path="../ipfs/ipfs-ctrl.sh ";//I'm cmake-build-debug

string IPFSCtrl::Getid(){
    FILE * fp;
    char buffer[1024];
    string cmd,ans;

    cmd.assign(cmd_Path);
    cmd.append("id");

    fp=popen(cmd.c_str(),"r");//"pwd" /root/miao/clion/Pro/Pro1/Tcp/cmake-build-debug
    fseek(fp,0,SEEK_END);
    long LSize=ftell(fp);
    rewind(fp);
    fread(buffer, sizeof(char),LSize,fp);

    //  fgets(buffer,sizeof(buffer),fp);//one line
    //  fgets(buffer,sizeof(buffer),fp);
    ans.append(buffer);

  //  std::cout << "miao:"<<buffer << std::endl;
  //  std::cout << "miao2:"<<ans << std::endl;
    pclose(fp);
    ans[46]='\0';//no need \n;
    return ans;
}

string IPFSCtrl::Connect(string ip, string ipfsHash){
    string cmd,ans;
    FILE * fp;
    char buffer[1024];

    cmd.assign(cmd_Path);
    cmd.append("connect ");
    cmd.append(ip+" ");
    cmd.append(ipfsHash+" ");
    //TODO

    fp=popen(cmd.c_str(),"r");
    fseek(fp,0,SEEK_END);
    long LSize=ftell(fp);
    rewind(fp);
    fread(buffer, sizeof(char),LSize,fp);
    pclose(fp);

    ans.append(buffer);
    return ans;
}


string IPFSCtrl::PeerList(){
    FILE * fp;
    char buffer[1024];
    string cmd,ans;

    cmd.assign(cmd_Path);
    cmd.append("swarm peers");

    fp=popen(cmd.c_str(),"r");//"pwd" /root/miao/clion/Pro/Pro1/Tcp/cmake-build-debug
    fseek(fp,0,SEEK_END);
    long LSize=ftell(fp);
    rewind(fp);
    fread(buffer, sizeof(char),LSize,fp);

    //  fgets(buffer,sizeof(buffer),fp);//one line
    //  fgets(buffer,sizeof(buffer),fp);
    ans.append(buffer);

    //  std::cout << "miao:"<<buffer << std::endl;
    //  std::cout << "miao2:"<<ans << std::endl;
    pclose(fp);
    return ans;
}

string IPFSCtrl::Put(string src){
    FILE * fp;
    char buffer[1024];
    string cmd,ans;

    cmd.assign(cmd_Path);
    cmd.append("add ");
    cmd.append(src);

    fp=popen(cmd.c_str(),"r");//"pwd" /root/miao/clion/Pro/Pro1/Tcp/cmake-build-debug
    fseek(fp,0,SEEK_END);
    long LSize=ftell(fp);
    rewind(fp);
    fread(buffer, sizeof(char),LSize,fp);
    ans.append(buffer);
    pclose(fp);

    ans[46]='\0';//no need \n;//no need \n; lenth is 47 (have \n)
    return ans;

}

string IPFSCtrl::Cat(string src){
    FILE * fp;
    char buffer[1024];
    string cmd,ans;

    cmd.assign(cmd_Path);
    cmd.append("cat ");
    cmd.append(src);

    fp=popen(cmd.c_str(),"r");//"pwd" /root/miao/clion/Pro/Pro1/Tcp/cmake-build-debug
    fseek(fp,0,SEEK_END);
    long LSize=ftell(fp);
    rewind(fp);
    fread(buffer, sizeof(char),LSize,fp);
    ans.append(buffer);
    ans[ans.size()-1]='\0';//no need \n;
    pclose(fp);
    return ans;

}


bool IPFSCtrl::Download(string src,string outPath){
    FILE * fp;
    char buffer[1024];
    string cmd,ans;

    cmd.assign(cmd_Path);
    cmd.append("get ");
    cmd.append(src+" ");
    cmd.append(outPath);

    fp=popen(cmd.c_str(),"r");//"pwd" /root/miao/clion/Pro/Pro1/Tcp/cmake-build-debug
    fseek(fp,0,SEEK_END);
    long LSize=ftell(fp);
    rewind(fp);
    fread(buffer, sizeof(char),LSize,fp);
    ans.append(buffer);
    pclose(fp);
    if(ans[0]=='.'){cout<<"IPFSCtrl::Download err"<<endl;return false;}

    return true;
}

