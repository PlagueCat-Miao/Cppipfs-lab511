//
// Created by root on 8/5/19.
//

#include "ByteArray.h"
#include <iostream>

ByteArray::ByteArray(int size) {

    bytes = new unsigned char[size];
    capacity=size;
    initSize=size;
    readIdx=0;
    writeIdx=0;

}
ByteArray::ByteArray(unsigned char *defaultBytes,int defaultsize) {

    bytes = defaultBytes;
    capacity=defaultsize;
    initSize=defaultsize;
    readIdx=0;
    writeIdx=defaultsize;

}
ByteArray::ByteArray(){
    bytes=NULL;
    capacity=0;
    initSize=0;
    readIdx=0;
    writeIdx=0;
}
void ByteArray::SetActive(bool act,int size){
    if(act){
        bytes = new unsigned char[size];
        capacity=size;
        initSize=size;
        readIdx=0;
        writeIdx=0;
    }
    else{
        if(bytes!=NULL){delete[] bytes;bytes=NULL;}
        capacity=0;
        initSize=0;
        readIdx=0;
        writeIdx=0;
    }
}

void ByteArray::StrCopy(unsigned char *des, int des_offset, unsigned char *src, int src_offset, int len) {
    
    int i;
    for(i=0;i<len;i++)
    {
        *(des+i+des_offset)=*(src+i+src_offset);

    }
}

void ByteArray::Resize(int size){
    if(size<length())return;
    if(size<initSize)return;
    int n=1;
    while(n<size)n*=2;
    capacity =n;
    unsigned char* newbytes = new unsigned char[capacity];
    unsigned char* d;
    StrCopy(newbytes,0,bytes,readIdx,length());
    d=bytes;
    bytes=newbytes;
    delete[] d;
    writeIdx=length();
    readIdx=0;
}
void ByteArray::MoveBytes() {
    StrCopy(bytes,0,bytes,readIdx,length());
    writeIdx=length();
    readIdx=0;
}

int ByteArray::WirteBA(unsigned char *bs, int offset, int count) {
    if(remain()< count)Resize(length()+count);
    StrCopy(bytes,writeIdx,bs,offset,count);
    writeIdx+=count;
    return count;
}
int ByteArray::ReadBA(unsigned char  *bs,int offset,int count){

    if(length()<count)count=length();

    StrCopy(bs,offset,bytes,0,count);
    readIdx+=count;
    CheckAndMovBytes();
    return count;

}

void ByteArray::Debug() {
    cout<<"readIdx:"<<readIdx<<"writeIdx:"<<writeIdx<<"bytes:"<<endl;
   // cout<<hex;
    int i;
    for(i=0;i<capacity;i++)
    {
        cout<<(int)(bytes[i]);
    }
}
void ByteArray::Tostring2() {

   // cout<<hex;
    int i;
    for(i=0;i<length();i++)
    {
        cout<<(int)(bytes[readIdx+i]);
    }
}



int ByteArray::ReadInt16() {
    if(length()<2)return 0;
    int ans = (int)(((bytes[1]-1)<<8)|(bytes[0]-1));
    readIdx+=2;
    CheckAndMovBytes();
    return ans;

}

int ByteArray::ReadInt16(char *bs) {
    if(strlen(bs)<2)return 0;
    int ans = (int)(((bs[1]-1)<<8)|(bs[0]-1));
    return ans;
}

/* test <unity for netgame P128>

int ByteArrayTest_main(int argc,char * argv[])
{
    cout<<"ByteArraytest"<<endl;
    ByteArray buff(8);

    unsigned char ww[5]={1,2,3,4,5};
    unsigned char ww2[6] ={6,7,8,9,10,11};
    unsigned char rr[4]={0,0,0,0};

    cout<<"[0 debug]-"; buff.Debug(); cout<<endl;
    cout<<"[0 string]-"; buff.Tostring2(); cout<<endl;

    buff.WirteBA(ww,0,5);
    cout<<"[1 debug]-"; buff.Debug(); cout<<endl;
    cout<<"[1 string]-"; buff.Tostring2(); cout<<endl;

    buff.ReadBA(rr,0,2);
    cout<<"[2 debug]-"; buff.Debug(); cout<<endl;
    cout<<"[2 string]-"; buff.Tostring2(); cout<<endl;
    cout<<"[2 rr]-"<<(int)rr[0]<<(int)rr[1]<<(int)rr[2]<<(int)rr[3]<< endl;

    buff.WirteBA(ww2,0,6);
    cout<<"[3 debug]-"; buff.Debug(); cout<<endl;
    cout<<"[3 string]-"; buff.Tostring2(); cout<<endl;


    return 0;
}
*/