//
// Created by root on 8/26/19.
//

#ifndef SELECTLEARN_IPFSCTRL_H
#define SELECTLEARN_IPFSCTRL_H

#include <string>

using namespace std;


class IPFSCtrl {
public:
    static string Getid();
    static string Connect(string ip, string ipfsHash);
    static string PeerList();

    static string Put(string src);
    static string Cat(string src);
    static bool Download(string src,string outPath);
public:
    static string cmd_Path;
};


#endif //SELECTLEARN_IPFSCTRL_H


