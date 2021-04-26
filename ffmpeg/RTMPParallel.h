//
// Created by hellcat on 2019/10/30.
//

#ifndef IPFS_DEMO_RTMPPARALLEL_H
#define IPFS_DEMO_RTMPPARALLEL_H

#include <opencv2/highgui.hpp>
#include <iostream>
#include <time.h>

extern "C"
{
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

}
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avformat.lib")
//#pragma comment(lib,"opencv_world300.lib")
using namespace std;
using namespace cv;


class RTMPParallel {
public:
    RTMPParallel();
    RTMPParallel(string in_Url);
    RTMPParallel(string out_Url,string in_Url);
    RTMPParallel(int in_Width , int in_Height ,int in_fps);
public:
    //nginx-rtmp 直播服务器rtmp推流URL
    char outUrl[45]= "rtmp://127.0.0.1/live/live";
    //像素格式转换上下文
    SwsContext *vsc = NULL;
    //输出的数据结构
    AVFrame *yuv = NULL;
    //编码器上下文
    AVCodecContext *vc = NULL;
    //rtmp flv 封装器
    AVFormatContext *ic = NULL;

    //a 找到编码器
    AVCodec *codec;
    //添加视频流
    AVStream *vs;
    //opencv
    VideoCapture cam;
    Mat frame;
    int cam_Width=0;
    int cam_Height=0;
    int cam_fps=25;

public:
    int camera_init(string inUrl);
    int ffmpeg_init(int inWidth,int inHeight,int fps);
    int open_rtmp(bool *isRun);
    int stop_rtmp();

    int frame_rtmp(Mat frame2,int *vpts,bool *isRun);



};



#endif //IPFS_DEMO_RTMPPARALLEL_H
