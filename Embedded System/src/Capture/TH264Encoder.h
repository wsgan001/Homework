#ifndef TH264ENCODER_H
#define TH264ENCODER_H


#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>

#include "TRect.h"
#include "MFC_API/SsbSipH264Encode.h"
#include "FrameExtractor/FrameExtractor.h"
#include "FrameExtractor/H264Frames.h"
#include "Common/LogMsg.h"
#include "header.h"
#include "s3c_pp.h"


class TH264Encoder {
public:
    TH264Encoder();
    virtual ~TH264Encoder();
    unsigned char * Encode(TRect &rect,long& size);
    void initEncode();
    void finishEncode();
protected:
    TH264Encoder(const TH264Encoder&);
    TH264Encoder &operator=( const TH264Encoder&);
private:
    int frame_count;
    void* handle;
    FILE* encoded_fp;
    unsigned char   g_yuv[YUV_FRAME_BUFFER_SIZE];
    unsigned char   *encoded_buf;
    long            encoded_size;

    s3c_pp_params_t	pp_param;
    s3c_pp_mem_alloc_t alloc_info1;
    s3c_pp_mem_alloc_t alloc_info2;
    int bufSize1;
    int bufSize2;
    int pp_fd1;

    void *mfc_encoder_init(int width, int height, int frame_rate, int bitrate, int gop_num);
    static void *mfc_encoder_exe(void *handle, unsigned char *yuv_buf, int frame_size, int first_frame, long *size);
    static void mfc_encoder_free(void *handle);


};

#endif // TH264ENCODER_H
