#ifndef PLAY_H
#define PLAY_H

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <linux/fb.h>
#include <signal.h>
#include <sys/stat.h>


#include "header.h"
#include "TRect.h"

#include "MFC_API/SsbSipH264Decode.h"
#include "FrameExtractor/FrameExtractor.h"
#include "FrameExtractor/H264Frames.h"
#include "Common/LogMsg.h"
#include "Common/lcd.h"
#include "s3c_pp.h"



class Play{
public:
	Play();
	~Play();
	void FBOpen();
	int playback();
	static void sig_del_h264(int signo);
private:
	int FriendlyARMWidth, FriendlyARMHeight;

	unsigned char delimiter_h264[4];
};
#endif // PLAY_H
