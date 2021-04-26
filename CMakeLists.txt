    cmake_minimum_required(VERSION 2.8)
    ## Use the variable PROJECT_NAME for changing the target name
    set( PROJECT_NAME "IPFS_demo" )
    set(CMAKE_CXX_STANDARD 11)
    ## Set our project name
    project(${PROJECT_NAME})
    ## Use all the *.cpp files we found under this folder for the project
  #  SET(CLIENT_SOURCE Client.cpp common.h)
    MESSAGE(STATUS )
    SET(NET_SOURCE
            net/NetManager.h net/NetManager.cpp
            net/MsgBase.h net/MsgBase.cpp
            net/ClientState.h net/ClientState.cpp
            net/ByteArray.h net/ByteArray.cpp


            )
    SET(NET_JSON
            net/cJSON.h net/cJSON.c
            net/CJsonObject.hpp net/CJsonObject.cpp
    )

    SET(JSON_SOURCE
            json/MsgMove.h json/MsgMove.cpp
            json/MsgIPFS.h json/MsgIPFS.cpp
            json/MsgFFmpeg.cpp json/MsgFFmpeg.h


            )
    SET(LOGIC_SOURCE
            logic/MsgHandler.h logic/MsgHandler.cpp


            )
    SET(IPFS_SOURCE
            ipfs/IPFSCtrl.h ipfs/IPFSCtrl.cpp


            )
    SET(DATEBASE_SOURCE
            db/MySQL.h  db/MySQL.cpp


            )
    SET(FFMPEG_SOURCE
            ffmpeg/RTMPParallel.cpp
            ffmpeg/RTMPParallel.h

            )
## ## ffmpeg
    SET(ORIGINAL_SOURCE
            ${NET_SOURCE} ${NET_JSON} ${JSON_SOURCE} ${LOGIC_SOURCE} ${IPFS_SOURCE} ${DATEBASE_SOURCE}

            )
    SET(FFMPEG_LIB
            ${PROJECT_SOURCE_DIR}/ffmpeg/ffmpeglib/libavcodec.so.57
            ${PROJECT_SOURCE_DIR}/ffmpeg/ffmpeglib/libavcodec.so.57.107.100
            ${PROJECT_SOURCE_DIR}/ffmpeg/ffmpeglib/libavformat.so.57
            ${PROJECT_SOURCE_DIR}/ffmpeg/ffmpeglib/libavutil.so.55
            ${PROJECT_SOURCE_DIR}/ffmpeg/ffmpeglib/libswscale.so.4
            )
    ## Define the executable
    # 1 sever
    add_executable(server  severmain.cpp ${NET_SOURCE} ${NET_JSON}  ${JSON_SOURCE} ${LOGIC_SOURCE} ${IPFS_SOURCE} ${DATEBASE_SOURCE} )
    target_link_libraries(server libmysqlclient.so)
    # 2 client-putfileToSever_byIPFS
    add_executable(client-put  clienttest.cpp  ${NET_SOURCE} ${NET_JSON} ${JSON_SOURCE} ${LOGIC_SOURCE} ${IPFS_SOURCE} ${DATEBASE_SOURCE})
    target_link_libraries(client-put libmysqlclient.so)
    # 3 client-getfileFromSever_byIPFS
    add_executable(client-get client-get.cpp  ${NET_SOURCE} ${NET_JSON} ${JSON_SOURCE} ${LOGIC_SOURCE} ${IPFS_SOURCE} ${DATEBASE_SOURCE})
    target_link_libraries(client-get libmysqlclient.so)

    # 3.4 client-normal
    add_executable(client-normal client-normal.h client-normal.cpp  ${NET_SOURCE} ${NET_JSON} ${JSON_SOURCE} ${LOGIC_SOURCE} ${IPFS_SOURCE} ${DATEBASE_SOURCE})
    target_link_libraries(client-normal libmysqlclient.so)


    ## add ffmpeg
    find_package( OpenCV REQUIRED )
    find_package( OpenCV QUIET COMPONENTS core highgui imgproc imgcodecs videoio)
    #LINK_LIBRARIES( ${PROJECT_SOURCE_DIR}/ffmpeg/ffmpeglib )
    # 4 ForSUNSEV_IoT
    add_executable(ForSUNSEV_IoTtest2 ffmpeg/ForSUNSEV_IoT.cpp ${FFMPEG_SOURCE} ${ORIGINAL_SOURCE})
    target_link_libraries(ForSUNSEV_IoTtest2  ${OpenCV_LIBS})## no SET ${OpenCV_LIBS}
    target_link_libraries(ForSUNSEV_IoTtest2  ${FFMPEG_LIB})
    target_link_libraries(ForSUNSEV_IoTtest2  libmysqlclient.so)
    target_link_libraries(ForSUNSEV_IoTtest2  pthread  )##https://www.cnblogs.com/jiu0821/p/5855827.html
    # 5 rtmp_cam_client
    add_executable(rtmp_cam_client rtmp_cam_client.cpp  ${FFMPEG_SOURCE} ${ORIGINAL_SOURCE})
    target_link_libraries(rtmp_cam_client  ${OpenCV_LIBS})## no SET ${OpenCV_LIBS}
    target_link_libraries(rtmp_cam_client  ${FFMPEG_LIB})
    target_link_libraries(rtmp_cam_client libmysqlclient.so)
    target_link_libraries(rtmp_cam_client  pthread  )##https://www.cnblogs.com/jiu0821/p/5855827.html
    # 6 rtmp_admin.cpp
    add_executable(client_rtmp_admin client_rtmp_admin.cpp  ${FFMPEG_SOURCE} ${ORIGINAL_SOURCE})
    target_link_libraries(client_rtmp_admin  ${OpenCV_LIBS})## no SET ${OpenCV_LIBS}
    target_link_libraries(client_rtmp_admin  ${FFMPEG_LIB})
    target_link_libraries(client_rtmp_admin libmysqlclient.so)
    target_link_libraries(client_rtmp_admin  pthread  )##https://www.cnblogs.com/jiu0821/p/5855827.html

    # 4.4 client_cam