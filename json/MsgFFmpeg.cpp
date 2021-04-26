//
// Created by hellcat on 2019/10/29.
//

#include "MsgFFmpeg.h"

void* create_MsgCallFor(){MsgBase *msg= new MsgCallFor(); return (msg==NULL)?NULL:msg;}

void* create_MsgAnswer(){MsgBase *msg= new MsgAnswer(); return (msg==NULL)?NULL:msg;}
