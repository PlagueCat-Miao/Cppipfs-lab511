//
// Created by hellcat on 2019/10/30.
//

#include "RTMPParallel.h"
#include<thread>

int RTMPParallel::ffmpeg_init(int inWidth,int inHeight,int fps)
{
    //注册所有的编解码器
    avcodec_register_all();
    //注册所有的封装器
    av_register_all();
    //注册所有网络协议
    avformat_network_init();
    ///2 初始化格式转换上下文
    vsc = sws_getCachedContext(vsc,
                               inWidth, inHeight, AV_PIX_FMT_BGR24,     //源宽、高、像素格式
                               inWidth, inHeight, AV_PIX_FMT_YUV420P,//目标宽、高、像素格式
                               SWS_BICUBIC,  // 尺寸变化使用算法
                               0, 0, 0
    );
    if (!vsc)
    {
        cout<<"sws_getCachedContext failed!"<<endl;
        return 2;
    }
    ///3 初始化输出的数据结构
    yuv = av_frame_alloc();
    yuv->format = AV_PIX_FMT_YUV420P;
    yuv->width = inWidth;
    yuv->height = inHeight;
    yuv->pts = 0;
    //分配yuv空间
    int ret = av_frame_get_buffer(yuv, 32);
    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout<<"buf"<<endl;
        return 3;
    }
    ///4 初始化编码上下文
    //a 找到编码器
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec)
    {
        cout<<"Can`t find h264 encoder!"<<endl;
        return 4;
    }
    //b 创建编码器上下文
    vc = avcodec_alloc_context3(codec);
    if (!vc)
    {
        cout<<"avcodec_alloc_context3 failed!"<<endl;
        return 4;
    }
    //c 配置编码器参数
    vc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; //全局参数
    vc->codec_id = codec->id;
    vc->thread_count = 8;

    vc->bit_rate = 50 * 1024 * 8;//压缩后每秒视频的bit位大小 50kB
    vc->width = inWidth;
    vc->height = inHeight;
    vc->time_base = { 1,fps };
    vc->framerate = { fps,1 };

    //画面组的大小，多少帧一个关键帧
    vc->gop_size = 50;
    vc->max_b_frames = 0;
    vc->pix_fmt = AV_PIX_FMT_YUV420P;
    //d 打开编码器上下文
    ret = avcodec_open2(vc, 0, 0);
    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout<<buf<<endl;
        return 4;
    }
    cout << "avcodec_open2 success!" << endl;
///5 输出封装器和视频流配置
    //a 创建输出封装器上下文
    ret = avformat_alloc_output_context2(&ic, 0, "flv", outUrl);
    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout<<buf<<endl;
        return 5;
    }

    //b 添加视频流

    // AVCodec *codec2 = avcodec_find_encoder(AV_CODEC_ID_FLV1);//查找AAC编码器,视频就查找其他格式的编码器
    // AVStream *vs = avformat_new_stream(ic,codec2);
    vs = avformat_new_stream(ic, NULL);
    if (!vs)
    {
        cout<<"avformat_new_stream failed"<<endl;
        return 5;
    }
    vs->codecpar->codec_tag = 0;
    //从编码器复制参数
    avcodec_parameters_from_context(vs->codecpar,vc);
    av_dump_format(ic, 0, outUrl, 1);
    return 0;

}
int RTMPParallel::camera_init(string inUrl)
{
    //海康相机的rtsp url
    //char *inUrl = "/home/hellcat/plague/data-play/02.mp4";

    /// 1 使用opencv打开rtsp相机
    if(inUrl=="NULL"){cam.open(0);}
    else{cam.open(inUrl);}
    namedWindow("video");
    //cam.open(0); // 本地相机
    if (!cam.isOpened())
    {
        cout<<"cam open failed!"<<endl;
        return 1;
        //throw std::logic_error("cam open failed!");
        // https://blog.csdn.net/weixin_30265171/article/details/96263581 :exception();
    }
    cout << inUrl << " cam open success" << endl;
    cam_Width = cam.get(CAP_PROP_FRAME_WIDTH);
    cam_Height = cam.get(CAP_PROP_FRAME_HEIGHT);
    cam_fps = cam.get(CAP_PROP_FPS);
    cam_fps = 25;
    return 0;
}
int RTMPParallel::open_rtmp(bool *isRun)
{
    *isRun=true;
    ///打开rtmp 的网络输出IO
    int ret;
    ret = avio_open(&ic->pb, outUrl, AVIO_FLAG_WRITE);
    //AVIOContext *pb; //输入输出上下文，输入数据的缓存
    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout<<buf<<endl;
        return 13;
    }

    //写入封装头
    ret = avformat_write_header(ic, NULL);

    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout<<buf<<endl;
        return 13;
    }

    AVPacket pack;
    memset(&pack, 0, sizeof(pack));
    int vpts = 0;
    while(1)
    {

        if(!*isRun){stop_rtmp();break;}

        ///读取rtsp视频帧，解码视频帧
        if (!cam.grab())
        {
            continue;
        }
        ///yuv转换为rgb
        if (!cam.retrieve(frame))
        {
            continue;
        }
        imshow("video", frame);
        waitKey(20);


        ///rgb to yuv
        //输入的数据结构
        uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
        //indata[0] bgrbgrbgr
        //plane indata[0] bbbbb indata[1]ggggg indata[2]rrrrr
        indata[0] = frame.data;
        int insize[AV_NUM_DATA_POINTERS] = { 0 };
        //一行（宽）数据的字节数
        insize[0] = frame.cols * frame.elemSize();
        int h = sws_scale(vsc, indata, insize, 0, frame.rows, //源数据
                          yuv->data, yuv->linesize);
        if (h <= 0)
        {
            continue;
        }
        //cout << h << " " << flush;
        ///h264编码
        yuv->pts = vpts;
        vpts++;
        ret = avcodec_send_frame(vc, yuv);

        if (ret != 0)
            continue;

        ret = avcodec_receive_packet(vc, &pack);
        if (ret != 0 || pack.size > 0)
        {
            //cout << "*" << pack.size << flush;
        }
        else
        {
            continue;
        }
        //推流
        pack.pts = av_rescale_q(pack.pts, vc->time_base, vs->time_base);
        pack.dts = av_rescale_q(pack.dts, vc->time_base, vs->time_base);
        pack.duration = av_rescale_q(pack.duration, vc->time_base, vs->time_base);
        ret = av_interleaved_write_frame(ic, &pack);
        if (ret == 0)
        {
            cout << "#" << flush;
        }

    }
    return 0;
}
int RTMPParallel::stop_rtmp()
{
    cout<<"Stop rtmp"<<endl;
    if (cam.isOpened())
        cam.release();
    if (vsc)
    {
        sws_freeContext(vsc);
        vsc = NULL;
    }

    if (vc)
    {
        avio_closep(&ic->pb);
        avcodec_free_context(&vc);
    }
    //av_freep(ic);
    av_free(yuv);
    av_free(vs);
    //av_free(codec);
    return 0;

}


RTMPParallel::RTMPParallel()
{
    //camera_init( "/home/hellcat/plague/data-play/02.mp4");
    camera_init( "NULL");
    cam_fps=25;
    ffmpeg_init( cam_Width, cam_Height, cam_fps);
}
RTMPParallel::RTMPParallel(string in_Url)
{
    camera_init( "/home/hellcat/plague/data-play/02.mp4");
    cam_fps=25;
    ffmpeg_init( cam_Width, cam_Height, cam_fps);
}
RTMPParallel::RTMPParallel(string in_Url,string out_Url)
{
    camera_init( in_Url);
    cam_fps=25;
    strcpy(outUrl,out_Url.c_str());
    ffmpeg_init( cam_Width, cam_Height, cam_fps);
}

RTMPParallel::RTMPParallel( int in_Width , int in_Height ,int in_fps)
{
    ffmpeg_init( in_Width, in_Height, in_fps);
    ///打开rtmp 的网络输出IO
    int ret;
    ret = avio_open(&ic->pb, outUrl, AVIO_FLAG_WRITE);
    //AVIOContext *pb; //输入输出上下文，输入数据的缓存
    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout<<buf<<endl;
        return ;
    }

    //写入封装头
    ret = avformat_write_header(ic, NULL);

    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout<<buf<<endl;
        return ;
    }

}
int RTMPParallel::frame_rtmp( Mat frame2,int *vpts,bool *isRun)
{
    *isRun=true;
    int ret;
    AVPacket pack;
    memset(&pack, 0, sizeof(pack));
    {
        if(!*isRun)
        {
            stop_rtmp();return 1;//break;
        }
        ///rgb to yuv
        //输入的数据结构
        uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
        //indata[0] bgrbgrbgr
        //plane indata[0] bbbbb indata[1]ggggg indata[2]rrrrr
        indata[0] = frame2.data;
        int insize[AV_NUM_DATA_POINTERS] = { 0 };
        //一行（宽）数据的字节数
        insize[0] = frame2.cols * frame2.elemSize();
        int h = sws_scale(vsc, indata, insize, 0, frame2.rows, //源数据
                          yuv->data, yuv->linesize);
        if (h <= 0)
        {
            return  2;
            //continue;
        }
        //cout << h << " " << flush;
        ///h264编码
        yuv->pts = *vpts;
        (*vpts)++;
        ret = avcodec_send_frame(vc, yuv);

        if (ret != 0) return  2;
            //continue;

        ret = avcodec_receive_packet(vc, &pack);
        if (ret != 0 || pack.size > 0)
        {
            //cout << "*" << pack.size << flush;
        }
        else
        {
            return  2;
            //continue;
        }
        //推流
        pack.pts = av_rescale_q(pack.pts, vc->time_base, vs->time_base);
        pack.dts = av_rescale_q(pack.dts, vc->time_base, vs->time_base);
        pack.duration = av_rescale_q(pack.duration, vc->time_base, vs->time_base);
        ret = av_interleaved_write_frame(ic, &pack);
        if (ret == 0)
        {
            cout << "#" << flush;
        }

    }
    return 0;
}

/*
int main()
{
    RTMPParallel Rpp;
    bool isRuning=true;
   // Rpp.open_rtmp();
    std::thread* t = new thread( &RTMPParallel::open_rtmp, Rpp,&isRuning);
    cout<<"wait for stop"<<endl;
    getchar();
    isRuning=false;
    cout<<"miao???"<<endl;
    t->join();
    return 0;
}
*/