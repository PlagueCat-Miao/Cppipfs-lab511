//
// Created by root on 9/19/19.
//
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <mysql/mysql.h>
using namespace std;
#include <fstream>
#include <cstdlib>

#ifndef SELECTLEARN_MYSQL_H
#define SELECTLEARN_MYSQL_H

//TODO :replace
// define NetManager.hpp -> static vector<FileState> *files_video; OK
// usaged MsgHandler -> files_video
//

class FileState{

public:
    //TODO :regist Membership in
    // 1. enum Membership_Number
    // 2. FileState::Equal()-> switch
    int id; //device
    std::string ipfsHash;
    time_t time;

    enum Membership_Number{
        asId=0,
        asIpfsHash,
        asTime
    };

    FileState(int Id,string IpfsHash,int T)
            : id(Id), ipfsHash(IpfsHash)
    {
        time = T;
    }

public:
    bool Equal(int valueid,void * valueAddr);
};



class  MySQL {
public:
    static char db_Name[20];
    static char sever_IP[20];
    static char user_Name[20];
    static char password[44];
    static MYSQL *conn;
public:


    static bool query_id(vector<FileState> *ans,int id);
    static bool query_All(vector<FileState> *ans);
    static bool delAll();
    static bool insent(FileState *val_file);
   // bool change();
    static  bool connect();
   // bool close();
   // public:
   //  static vector<FileState> *files_video;
};


#endif //SELECTLEARN_MYSQL_H
