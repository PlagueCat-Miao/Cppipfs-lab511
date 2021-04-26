//
// Created by root on 8/28/19.
//

#include "MsgIPFS.h"


void* create_MsgLogin(){MsgBase *msg= new MsgLogin(); return (msg==NULL)?NULL:msg;}

void* create_MsgPut(){MsgBase *msg= new MsgPut(); return (msg==NULL)?NULL:msg;}

void* create_MsgGet(){MsgBase *msg= new MsgGet(); return (msg==NULL)?NULL:msg;}

