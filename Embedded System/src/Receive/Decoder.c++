#include "Decoder.h"

Decoder::Decoder(){

}

Decoder::~Decoder(){

}

void Decoder::closeDevice(){
	close(pp_fd);
	/*
	/////////////////////////////////////
	munmap(fb_addr, fb_size);
	close(fb_fd);
	////////////////////////////////////
	*/
}

int Decoder::openDevice(){
	// Post processor open
	pp_fd = open(PP_DEV_NAME, O_RDWR);
	if(pp_fd < 0)
	{
		printf("Post processor open error\n");
		return -1;
	}
	bufSize = ioctl(pp_fd, S3C_PP_GET_SRC_BUF_SIZE);
	//decoder_buff.size = bufSize;
	decoder_buff.size = INPUT_BUFFER_SIZE;
	if ( -1 == ioctl(pp_fd, S3C_PP_ALLOC_KMEM, &decoder_buff) )
	{
		printf("ioctl S3C_PP_ALLOC_KMEM failed1\n");
		return -1;
	}
	/*
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// LCD frame buffer open
	fb_fd = open("/dev/fb1", O_RDWR|O_NDELAY);
	if(fb_fd < 0)
	{
		printf("LCD frame buffer open error\n");
		return -1;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	*/
	return 0;
}

int Decoder::predecode(unsigned char* buff,long nFrameLeng){
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
	pStrmBuf = SsbSipH264DecodeGetInBuf(handle, 0);
	if (pStrmBuf == NULL) {
		printf("SsbSipH264DecodeGetInBuf Failed.\n");
		SsbSipH264DecodeDeInit(handle);
		return -1;
	}

	memcpy(pStrmBuf,buff,nFrameLeng);

	////////////////////////////////////////////////////////////////
	///    3. Configuring the instance with the config stream    ///
	///       (SsbSipH264DecodeExe)                             ///
	////////////////////////////////////////////////////////////////
	if (SsbSipH264DecodeExe(handle, nFrameLeng) != SSBSIP_H264_DEC_RET_OK) {
		printf("H.264 Decoder Configuration Failed.\n");
		return -1;
	}

	/*
	/////////////////////////////////////
	///   4. Get stream information   ///
	/////////////////////////////////////
	SsbSipH264DecodeGetConfig(handle, H264_DEC_GETCONF_STREAMINFO, &stream_info);
	*/

	// set post processor configuration
	pp_param.src_full_width	    = VIDEO_WIDTH;
	pp_param.src_full_height	= VIDEO_HEIGHT;
	pp_param.src_start_x		= 0;
	pp_param.src_start_y		= 0;
	pp_param.src_width			= pp_param.src_full_width;
	pp_param.src_height			= pp_param.src_full_height;
	pp_param.src_color_space	= YC420;
	pp_param.dst_start_x		= 0;
	pp_param.dst_start_y		= 0;
	pp_param.dst_full_width	    = VIDEO_WIDTH;		// destination width
	pp_param.dst_full_height	= VIDEO_HEIGHT;		// destination height
	pp_param.dst_width			= pp_param.dst_full_width;
	pp_param.dst_height			= pp_param.dst_full_height;
	pp_param.dst_color_space	= FB0_COLOR_SPACE;
	pp_param.out_path           = DMA_ONESHOT;

	ioctl(pp_fd, S3C_PP_SET_PARAMS, &pp_param);
/*
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// get LCD frame buffer address
	fb_size = pp_param.dst_full_width * pp_param.dst_full_height * 2;	// RGB565
	fb_addr = (char *)mmap(0, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	if (fb_addr == NULL) {
		printf("LCD frame buffer mmap failed\n");
		return -1;
	}

	osd_info_to_driver.Bpp			= FB0_BPP;	// RGB16
	osd_info_to_driver.LeftTop_x	= 4;
	osd_info_to_driver.LeftTop_y	= 34;
	osd_info_to_driver.Width		= VIDEO_WIDTH;	// display width
	osd_info_to_driver.Height		= VIDEO_HEIGHT;	// display height

	// set OSD's information
	if(ioctl(fb_fd, SET_OSD_INFO, &osd_info_to_driver)) {
		printf("Some problem with the ioctl SET_OSD_INFO\n");
		return -1;
	}

	ioctl(fb_fd, SET_OSD_START);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/
	return 0;
}

int Decoder::finishdecode(){
	SsbSipH264DecodeDeInit(handle);
	return 0;
}

int Decoder::decode(long nFrameLeng){
	//////////////////////////////////
	///       5. DECODE            ///
	///    (SsbSipH264DecodeExe)   ///
	//////////////////////////////////
	if (SsbSipH264DecodeExe(handle, nFrameLeng) != SSBSIP_H264_DEC_RET_OK)
		return -1;

	//////////////////////////////////////////////
	///    6. Obtaining the Output Buffer      ///
	///      (SsbSipH264DecodeGetOutBuf)       ///
	//////////////////////////////////////////////
	SsbSipH264DecodeGetConfig(handle, H264_DEC_GETCONF_PHYADDR_FRAM_BUF, pYUVBuf);

	pp_param.src_buf_addr_phy		= pYUVBuf[0];	// MFC output buffer
	ioctl(pp_fd, S3C_PP_SET_SRC_BUF_ADDR_PHY, &pp_param);


	pp_param.dst_buf_addr_phy		= decoder_buff.phy_addr;			// LCD frame buffer
	ioctl(pp_fd, S3C_PP_SET_DST_BUF_ADDR_PHY, &pp_param);


	////////////////////////////////////////////////////////////////////////////////////////////
	//ioctl(fb_fd, FBIOGET_FSCREENINFO, &lcd_info);
	//pp_param.dst_buf_addr_phy		= lcd_info.smem_start;			// LCD frame buffer
	//ioctl(pp_fd, S3C_PP_SET_DST_BUF_ADDR_PHY, &pp_param);

	/////////////////////////////////////////////////////////////////////////////////////////
	ioctl(pp_fd, S3C_PP_START);
	//memcpy((char*)decoder_buff.phy_addr,(char*)lcd_info.smem_start,fb_size);
	return 0;
}

void Decoder::nextframe(unsigned char* buff,long nFrameLeng){
	/////////////////////////////
	// Next H.264 VIDEO stream //
	/////////////////////////////
	memcpy(pStrmBuf,buff,nFrameLeng);
}

unsigned char * Decoder::getBuf(){
	return (unsigned char *)decoder_buff.vir_addr;
}

