#ifndef DECODER_H
#define DECODER_H


#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <linux/fb.h>
#include <string.h>
#include <unistd.h>

#include "MFC_API/SsbSipH264Decode.h"
#include "FrameExtractor/FrameExtractor.h"
#include "FrameExtractor/H264Frames.h"
#include "Common/LogMsg.h"
#include "Common/lcd.h"
#include "s3c_pp.h"
#include "header.h"


class Decoder{
private:
	int pp_fd;
	void         *handle;
	//unsigned char *decoder_buff;
	s3c_pp_mem_alloc_t decoder_buff;
	int bufSize;
	s3c_pp_params_t	pp_param;
	void			*pStrmBuf;
	SSBSIP_H264_STREAM_INFO stream_info;
	unsigned int	pYUVBuf[2];
/*
	///////////////////////////////////////////
	int fb_fd;
	int fb_size;
	char *fb_addr;
	s3c_win_info_t	osd_info_to_driver;
	struct fb_fix_screeninfo	lcd_info;
	//////////////////////////////////////////
	*/
public:
	Decoder();
	~Decoder();
	int decode(long nFrameLeng);
	void nextframe(unsigned char* buff,long nFrameLeng);
	int predecode(unsigned char* buff,long nFrameLeng);
	int finishdecode();
	unsigned char* getBuf();
	int openDevice();
	void closeDevice();
};

#endif // DECODER_H
