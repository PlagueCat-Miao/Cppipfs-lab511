//
// Created by root on 8/26/19.
//

#include <iostream>
#include <stdio.h>
#include <string>

int main() {
    FILE * fp;
    char buffer[1024];

    std::string cmd="../ipfs/ipfs-ctrl.sh ";//I'm cmake-build-debug
    std::string cmd2="id";
    std::string ans;
    cmd.append(cmd2);

    fp=popen(cmd.c_str(),"r");//"pwd" /root/miao/clion/Pro/Pro1/Tcp/cmake-build-debug
    fseek(fp,0,SEEK_END);
    long LSize=ftell(fp);
    rewind(fp);
    fread(buffer, sizeof(char),LSize,fp);

    //  fgets(buffer,sizeof(buffer),fp);//one line
    //  fgets(buffer,sizeof(buffer),fp);
    ans.append(buffer);

    std::cout << "miao:"<<buffer << std::endl;
    std::cout << "miao2:"<<ans << std::endl;
    pclose(fp);

    return 0;
}
