//
// Created by root on 9/19/19.
//

#include "MySQL.h"
#include <mysql/mysql.h>
#include <iostream>
using namespace std;

char MySQL::db_Name[20]="IPFS_gateway";
char MySQL::sever_IP[20]="127.0.0.1";
char MySQL::user_Name[20]="root";
char MySQL::password[44]="12345678";
MYSQL *MySQL::conn=mysql_init(NULL);

bool MySQL::connect()
{
   if(conn == NULL){
       cout<<"MySQL Err mysql_init failed!"<<endl;
       return false;
   }
   conn=mysql_real_connect(conn,sever_IP,user_Name,password,db_Name,3306,NULL,0);
   if(!conn){
       cout<<"MySQL Err mysql_real_connect failed!"<<endl;
       return false;
   }
    cout<<"MySQL:"<<db_Name<<" connect"<<endl;
   return true;
}

bool MySQL::insent(FileState *val_file)
{
    if(conn == NULL){
       cout<<"MySQL Err insent conn NULL!"<<endl;
       return false;
    }
    string cmd="insert into videos value()";

    string val;char temp[54];
    sprintf(temp,"%d",val_file->id);
    val+=temp;
    val+=", '"+val_file->ipfsHash+"', ";
    sprintf(temp,"%ld",val_file->time);
    val+=temp;
    val+=", NULL";
    cmd.insert(25,val);
     if(mysql_query(conn,cmd.c_str())){
         cout<<"MySQL Err insent with:"<<mysql_error(conn)<<endl;
         cout<<"&by:"<<cmd<<endl;
         return false;
     }

   return true;
}

bool MySQL::delAll()
{
    if(conn == NULL){
       cout<<"MySQL Err insent conn NULL!"<<endl;
       return false;
    }
    string cmd="delete from videos where *";

    if(mysql_query(conn,cmd.c_str())){
         cout<<"MySQL Err insent with:"<<mysql_error(conn)<<endl;
         cout<<"&by:"<<cmd<<endl;
         return false;
    }

   return true;
}

bool MySQL::query_id(vector<FileState> *ans,int id)
{
    if(conn == NULL){
       cout<<"MySQL Err insent conn NULL!"<<endl;
       return false;
    }
    if(ans == NULL){
        cout<<"MySQL Err insent vector NULL!"<<endl;
        return false;
    }
    string cmd="select * from videos where id=";
    char temp[54];
    sprintf(temp,"%d",id);
    cmd+=temp;

    if(mysql_query(conn,cmd.c_str())){
         cout<<"MySQL Err insent with:"<<mysql_error(conn)<<endl;
         cout<<"&by:"<<cmd<<endl;
         return false;
    }
    MYSQL_RES *mysqlRes = NULL;
    mysqlRes= mysql_store_result(conn);
    int row_count = mysql_num_rows(mysqlRes);
    int column_count = mysql_num_fields(mysqlRes);

    MYSQL_ROW row = NULL;
    for(int i =0;i<row_count;i++){
        row=mysql_fetch_row(mysqlRes);
        if(NULL==row){
            cout<<"MySQL Err:bad data"<<endl;
            return false;
        }
        FileState temp_fs(atoi(row[0]),row[1],atoi(row[2]));
        ans->push_back(temp_fs);
    }
    mysql_free_result(mysqlRes);

   return true;
}

bool MySQL::query_All(vector<FileState> *ans)
{
    if(conn == NULL){
       cout<<"MySQL Err insent conn NULL!"<<endl;
       return false;
    }
    if(ans == NULL){
        cout<<"MySQL Err insent vector NULL!"<<endl;
        return false;
    }
    string cmd="select * from videos ";

    if(mysql_query(conn,cmd.c_str())){
         cout<<"MySQL Err insent with:"<<mysql_error(conn)<<endl;
         cout<<"&by:"<<cmd<<endl;
         return false;
    }
    MYSQL_RES *mysqlRes = NULL;
    mysqlRes= mysql_store_result(conn);
    int row_count = mysql_num_rows(mysqlRes);
    int column_count = mysql_num_fields(mysqlRes);

    MYSQL_ROW row = NULL;
    for(int i =0;i<row_count;i++){
        row=mysql_fetch_row(mysqlRes);
        if(NULL==row){
            cout<<"MySQL Err:bad data"<<endl;
            return false;
        }
        FileState temp_fs(atoi(row[0]),row[1],atoi(row[2]));
        ans->push_back(temp_fs);
    }
    mysql_free_result(mysqlRes);

   return true;
}



//vector<FileState> *MySQL::files_video = new vector<FileState>();

bool FileState::Equal(int valueid,void * valueAddr)
{
    if(valueAddr==NULL){cout<<"err ClientState.cpp FileState::Equal-null valueAddr"<<endl;return false; }
    bool ans=false;
    switch(valueid)
    {
        case asId:
            ans=id==(*(int*)valueAddr);
            break;
        case asIpfsHash:
            ans=ipfsHash==(*(string*)valueAddr);
            break;
        case asTime:
            ans=time==(*(int*)valueAddr);
            break;

        default:
            cout<<"err ClientState.cpp FileState::Equal-null type"<<endl;
            break;

    }


    return ans;
}
