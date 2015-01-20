#-------------------------------------------------
#
# Project created by QtCreator 2013-04-24T19:54:08
#
#-------------------------------------------------

QT       += core gui
QT       += network

TARGET = Receive
TEMPLATE = app


SOURCES += main.c++\
        widget.c++ \
    Common/performance.c \
    Common/LogMsg.c \
    FrameExtractor/VC1Frames.c \
    FrameExtractor/MPEG4Frames.c \
    FrameExtractor/H264Frames.c \
    FrameExtractor/H263Frames.c \
    FrameExtractor/FrameExtractor.c \
    FrameExtractor/FileRead.c \
    MFC_API/SsbSipVC1Decode.c \
    MFC_API/SsbSipMpeg4Encode.c \
    MFC_API/SsbSipMpeg4Decode.c \
    MFC_API/SsbSipMfcDecode.c \
    MFC_API/SsbSipH264Encode.c \
    MFC_API/SsbSipH264Decode.c \
    Decoder.c++ \
    Play.c++ \
    TRect.c++ \
    TError.c++

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
    FrameExtractor/FrameExtractor.h \
    FrameExtractor/FileRead.h \
    MFC_API/SsbSipVC1Decode.h \
    MFC_API/SsbSipMpeg4Encode.h \
    MFC_API/SsbSipMpeg4Decode.h \
    MFC_API/SsbSipMfcDecode.h \
    MFC_API/SsbSipH264Encode.h \
    MFC_API/SsbSipH264Decode.h \
    s3c_pp.h \
    Decoder.h \
    Play.h \
    header.h \
    TRect.h \
    TError.h

FORMS    += widget.ui

OTHER_FILES += \
    Common/performance.o \
    Common/Makefile \
    Common/LogMsg.o \
    FrameExtractor/MPEG4Frames.o \
    FrameExtractor/Makefile \
    FrameExtractor/H264Frames.o \
    FrameExtractor/H263Frames.o \
    FrameExtractor/FrameExtractor.o \
    FrameExtractor/FileRead.o \
    MFC_API/SsbSipVC1Decode.o \
    MFC_API/SsbSipMpeg4Encode.o \
    MFC_API/SsbSipMpeg4Decode.o \
    MFC_API/SsbSipMfcDecode.o \
    MFC_API/SsbSipH264Encode.o \
    MFC_API/SsbSipH264Decode.o \
    MFC_API/Makefile

RESOURCES += \
    images.qrc
