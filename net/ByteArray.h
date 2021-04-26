//
// Created by root on 8/5/19.
//

#ifndef SELECTLEARN_BYTEARRAY_H
#define SELECTLEARN_BYTEARRAY_H

#define DeFAULT_SIZE 1024

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
using namespace std;


class ByteArray {

private:
    //const int DeFAULT_SIZE =1024;
    int initSize;
    int capacity;

public:
 unsigned char  * bytes;
 int readIdx;
 int writeIdx;
 int length(){return writeIdx -readIdx;}
 int remain(){return capacity - writeIdx;}
 void CheckAndMovBytes(){if(length()<8){MoveBytes();}}

 void StrCopy(unsigned char* des,int desoffset, unsigned char *src, int srcoffset,int len);

 ByteArray();
 ByteArray(int size);
 ByteArray(unsigned char* defaultBytes,int defaultsize);
 ~ByteArray(){if(bytes!=NULL){delete[] bytes;bytes=NULL;}}
 void SetActive(bool act,int size=DeFAULT_SIZE);

 void Resize(int size);
 void MoveBytes();
 int WirteBA(unsigned char  * bs,int offset,int count);
 int ReadBA(unsigned char  * bs,int offset,int count);
 void Debug();

 void  Tostring2();
 int ReadInt16();
 static int ReadInt16(char *bs);


};


#endif //SELECTLEARN_BYTEARRAY_H
