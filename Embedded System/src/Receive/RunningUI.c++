#include "Play.h"

static void *handle;
static int in_fd;
static int file_size;
static char *in_addr;
static int fb_size;
static int pp_fd, fb_fd;
static char *fb_addr;

Play::Play(){

}

Play::~Play(){
}

void Play::sig_del_h264(int signo)
{
	printf("[H.264 display] signal handling\n");
	ioctl(fb_fd, SET_OSD_STOP);
	SsbSipH264DecodeDeInit(handle);
	munmap(in_addr, file_size);
	munmap(fb_addr, fb_size);
	close(pp_fd);
	close(fb_fd);
	close(in_fd);
	exit(1);
}



int Play::playback()
{
	delimiter_h264[0] = 0x00;
	delimiter_h264[1] = 0x00;
	delimiter_h264[2] = 0x00;
	delimiter_h264[3] = 0x01;
	//int FB0_WIDTH=FriendlyARMWidth;
	//int FB0_HEIGHT=FriendlyARMHeight;

	int FB0_WIDTH=VIDEO_WIDTH;
	int FB0_HEIGHT=VIDEO_HEIGHT;

	FILE* f = fopen(MEDIA_FILE_NAME,"r");
	if (f == 0) {
		printf("please record first!");
		return -1;
	}
	fclose(f);

	void			*pStrmBuf;
	int				nFrameLeng = 0;
	unsigned int	pYUVBuf[2];

	struct stat				s;
	FRAMEX_CTX				*pFrameExCtx;	// frame extractor context
	FRAMEX_STRM_PTR 		file_strm;
	SSBSIP_H264_STREAM_INFO stream_info;

	s3c_pp_params_t	pp_param;
	s3c_win_info_t	osd_info_to_driver;

	struct fb_fix_screeninfo	lcd_info;

#ifdef FPS
	struct timeval	start, stop;
	unsigned int	time = 0;
	int				frame_cnt = 0;
	int				mod_cnt = 0;
#endif

	if(signal(SIGINT,sig_del_h264) == SIG_ERR) {
		printf("Sinal Error\n");
	}

	// in file open
	in_fd	= open(MEDIA_FILE_NAME, O_RDONLY);
	if(in_fd < 0) {
		printf("Input file open failed\n");
		return -1;
	}

	// get input file size
	fstat(in_fd, &s);
	file_size = s.st_size;

	// mapping input file to memory
	in_addr = (char *)mmap(0, file_size, PROT_READ, MAP_SHARED, in_fd, 0);
	if(in_addr == NULL) {
		printf("input file memory mapping failed\n");
		return -1;
	}

	// Post processor open
	pp_fd = open(PP_DEV_NAME, O_RDWR);
	if(pp_fd < 0)
	{
		printf("Post processor open error\n");
		return -1;
	}

	// LCD frame buffer open
	fb_fd = open("/dev/fb1", O_RDWR|O_NDELAY);
	if(fb_fd < 0)
	{
		printf("LCD frame buffer open error\n");
		return -1;
	}

	///////////////////////////////////
	// FrameExtractor Initialization //
	///////////////////////////////////
	pFrameExCtx = FrameExtractorInit(FRAMEX_IN_TYPE_MEM, delimiter_h264, sizeof(delimiter_h264), 1);
	file_strm.p_start = file_strm.p_cur = (unsigned char *)in_addr;
	file_strm.p_end = (unsigned char *)(in_addr + file_size);
	FrameExtractorFirst(pFrameExCtx, &file_strm);


	//////////////////////////////////////
	///    1. Create new instance      ///
	///      (SsbSipH264DecodeInit)    ///
	//////////////////////////////////////
	handle = SsbSipH264DecodeInit();
	if (handle == NULL) {
		printf("H264_Dec_Init Failed.\n");
		return -1;
	}

	/////////////////////////////////////////////
	///    2. Obtaining the Input Buffer      ///
	///      (SsbSipH264DecodeGetInBuf)       ///
	/////////////////////////////////////////////
	pStrmBuf = SsbSipH264DecodeGetInBuf(handle, nFrameLeng);
	if (pStrmBuf == NULL) {
		printf("SsbSipH264DecodeGetInBuf Failed.\n");
		SsbSipH264DecodeDeInit(handle);
		return -1;
	}

	////////////////////////////////////
	//  H264 CONFIG stream extraction //
	////////////////////////////////////
	nFrameLeng = ExtractConfigStreamH264(pFrameExCtx, &file_strm, (unsigned char*)pStrmBuf, INPUT_BUFFER_SIZE, NULL);


	////////////////////////////////////////////////////////////////
	///    3. Configuring the instance with the config stream    ///
	///       (SsbSipH264DecodeExe)                             ///
	////////////////////////////////////////////////////////////////
	if (SsbSipH264DecodeExe(handle, nFrameLeng) != SSBSIP_H264_DEC_RET_OK) {
		printf("H.264 Decoder Configuration Failed.\n");
		return -1;
	}


	/////////////////////////////////////
	///   4. Get stream information   ///
	/////////////////////////////////////
	SsbSipH264DecodeGetConfig(handle, H264_DEC_GETCONF_STREAMINFO, &stream_info);

//	printf("\t<STREAMINFO> width=%d   height=%d.\n", stream_info.width, stream_info.height);


	// set post processor configuration
	pp_param.src_full_width	    = stream_info.buf_width;
	pp_param.src_full_height	= stream_info.buf_height;
	pp_param.src_start_x		= 0;
	pp_param.src_start_y		= 0;
	pp_param.src_width			= pp_param.src_full_width;
	pp_param.src_height			= pp_param.src_full_height;
	pp_param.src_color_space	= YC420;
	pp_param.dst_start_x		= 0;
	pp_param.dst_start_y		= 0;
	pp_param.dst_full_width	    = FB0_WIDTH;		// destination width
	pp_param.dst_full_height	= FB0_HEIGHT;		// destination height
	pp_param.dst_width			= pp_param.dst_full_width;
	pp_param.dst_height			= pp_param.dst_full_height;
	pp_param.dst_color_space	= FB0_COLOR_SPACE;
	pp_param.out_path           = DMA_ONESHOT;

	ioctl(pp_fd, S3C_PP_SET_PARAMS, &pp_param);

	// get LCD frame buffer address
	fb_size = pp_param.dst_full_width * pp_param.dst_full_height * 2;	// RGB565
	fb_addr = (char *)mmap(0, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	if (fb_addr == NULL) {
		printf("LCD frame buffer mmap failed\n");
		return -1;
	}

	osd_info_to_driver.Bpp			= FB0_BPP;	// RGB16
	//osd_info_to_driver.LeftTop_x	= 0;
	//osd_info_to_driver.LeftTop_y	= 0;
	osd_info_to_driver.LeftTop_x	= 15;
	osd_info_to_driver.LeftTop_y	= 32;
	osd_info_to_driver.Width		= FB0_WIDTH;	// display width
	osd_info_to_driver.Height		= FB0_HEIGHT;	// display height

	// set OSD's information
	if(ioctl(fb_fd, SET_OSD_INFO, &osd_info_to_driver)) {
		printf("Some problem with the ioctl SET_OSD_INFO\n");
		return -1;
	}

	ioctl(fb_fd, SET_OSD_START);

	while(1)
	{

	#ifdef FPS
		gettimeofday(&start, NULL);
	#endif

		//////////////////////////////////
		///       5. DECODE            ///
		///    (SsbSipH264DecodeExe)   ///
		//////////////////////////////////
		if (SsbSipH264DecodeExe(handle, nFrameLeng) != SSBSIP_H264_DEC_RET_OK)
			break;

		//////////////////////////////////////////////
		///    6. Obtaining the Output Buffer      ///
		///      (SsbSipH264DecodeGetOutBuf)       ///
		//////////////////////////////////////////////
		SsbSipH264DecodeGetConfig(handle, H264_DEC_GETCONF_PHYADDR_FRAM_BUF, pYUVBuf);


		/////////////////////////////
		// Next H.264 VIDEO stream //
		/////////////////////////////
		nFrameLeng = NextFrameH264(pFrameExCtx, &file_strm, (unsigned char*)pStrmBuf, INPUT_BUFFER_SIZE, NULL);
		if (nFrameLeng < 4)
			break;

		// Post processing

		pp_param.src_buf_addr_phy		= pYUVBuf[0];	// MFC output buffer
		ioctl(pp_fd, S3C_PP_SET_SRC_BUF_ADDR_PHY, &pp_param);

		ioctl(fb_fd, FBIOGET_FSCREENINFO, &lcd_info);
		pp_param.dst_buf_addr_phy		= lcd_info.smem_start;			// LCD frame buffer
		ioctl(pp_fd, S3C_PP_SET_DST_BUF_ADDR_PHY, &pp_param);
		ioctl(pp_fd, S3C_PP_START);


	#ifdef FPS
		gettimeofday(&stop, NULL);
		time += measureTime(&start, &stop);
		frame_cnt++;
		mod_cnt++;
		if (mod_cnt == 50) {
			printf("Average FPS : %u\n", (float)mod_cnt*1000/time);
			mod_cnt = 0;
			time = 0;
		}
	#endif


	}

#ifdef FPS
	printf("Display Time : %u, Frame Count : %d, FPS : %f\n", time, frame_cnt, (float)frame_cnt*1000/time);
#endif

	ioctl(fb_fd, SET_OSD_STOP);
	SsbSipH264DecodeDeInit(handle);

	munmap(in_addr, file_size);
	munmap(fb_addr, fb_size);
	close(pp_fd);
	close(fb_fd);
	close(in_fd);
}


void Play::FBOpen(void)
{

	struct fb_fix_screeninfo FBFix;
	struct fb_var_screeninfo FBVar;
	int FBHandle = -1;

	FBHandle = open("/dev/fb0", O_RDWR);
	if (ioctl(FBHandle, FBIOGET_FSCREENINFO, &FBFix) == -1 ||
	    ioctl(FBHandle, FBIOGET_VSCREENINFO, &FBVar) == -1) {
		fprintf(stderr, "Cannot get Frame Buffer information");
		exit(1);
	}

	FriendlyARMWidth  = FBVar.xres;
	FriendlyARMHeight = FBVar.yres;
	close(FBHandle);
}
