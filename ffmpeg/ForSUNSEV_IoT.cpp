//
// Created by hellcat on 2019/10/24.
//

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "RTMPParallel.h"
using namespace cv;
int main( int argc, char** argv )
{
    //读取视频
    //VideoCapture capture("/home/hellcat/plague/data-play/02.mp4");
    //VideoCapture capture("rtmp://127.0.0.1:1935/live/live");
    VideoCapture capture(0);

    VideoCapture *vc =&capture;
    /**************************************************/
    //VideoCapture *vc = new VideoCapture();
    // 可以成功打开本地USB摄像头
    // 参数可以是 0~n,结合自身情况
    //vc->open(0);
    /**************************************************/
    int inWidth = capture.get(CAP_PROP_FRAME_WIDTH);
    int inHeight = capture.get(CAP_PROP_FRAME_HEIGHT);
    int fps = 25;
    RTMPParallel RPP(inWidth,inHeight,fps);
    bool isRUN;
    int vpts=0;
    //循环显示每一帧
    while (1)
    {
        //frame存储每一帧图像
        Mat frame;
        //读取当前帧
        (*vc) >> frame;
        //播放完退出
        if (frame.empty()) {
            printf("播放完成\n");
            break;
        }
        //显示当前视频
        imshow("读取视频",frame);
        //延时30ms
         waitKey(20);
        //推！
        int ret=RPP.frame_rtmp(frame,&vpts,&isRUN);
        if(ret==2){ continue;}
        else if(ret==1){ break;}
    }
    return 0;
}

