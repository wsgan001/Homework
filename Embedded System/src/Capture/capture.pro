#-------------------------------------------------
#
# Project created by QtCreator 2013-04-17T17:21:33
#
#-------------------------------------------------

QT       += core gui
QT  +=network

TARGET = capture
TEMPLATE = app


SOURCES += main.c++\
        widget.c++ \
    Common/performance.c \
    Common/LogMsg.c \
    FrameExtractor/FileRead.c \
    MFC_API/SsbSipVC1Decode.c \
    MFC_API/SsbSipMpeg4Encode.c \
    MFC_API/SsbSipMpeg4Decode.c \
    MFC_API/SsbSipMfcDecode.c \
    MFC_API/SsbSipH264Encode.c \
    MFC_API/SsbSipH264Decode.c \
    TRect.c++ \
    TH264Encoder.c++ \
    TFrameBuffer.c++ \
    TVideo.c++ \
    TError.c++ \
    Play.c++ \
    FrameExtractor/VC1Frames.c \
    FrameExtractor/H263Frames.c \
    FrameExtractor/H264Frames.c \
    FrameExtractor/FrameExtractor.c \
    FrameExtractor/MPEG4Frames.c \
    Mythread.c++

HEADERS  += widget.h \
    Common/videodev2_s3c.h \
    Common/videodev2.h \
    Common/post.h \
    Common/performance.h \
    Common/MfcDrvParams.h \
    Common/MfcDriver.h \
    Common/mfc.h \
    Common/LogMsg.h \
    Common/lcd.h \
    FrameExtractor/VC1Frames.h \
    FrameExtractor/MPEG4Frames.h \
    FrameExtractor/H264Frames.h \
    FrameExtractor/H263Frames.h \
    MFC_API/SsbSipVC1Decode.h \
    MFC_API/SsbSipMpeg4Encode.h \
    MFC_API/SsbSipMpeg4Decode.h \
    MFC_API/SsbSipMfcDecode.h \
    MFC_API/SsbSipH264Encode.h \
    MFC_API/SsbSipH264Decode.h \
    TRect.h \
    TH264Encoder.h \
    TFrameBuffer.h \
    TVideo.h \
    TError.h \
    Play.h \
    header.h \
    FrameExtractor/FileRead.h \
    FrameExtractor/FrameExtractor.h \
    Mythread.h

FORMS    += widget.ui

OTHER_FILES += \
    Common/Makefile

RESOURCES += \
    images.qrc
