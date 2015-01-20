#include"TH264Encoder.h"



TH264Encoder::TH264Encoder() {

}

void TH264Encoder::initEncode(){
	frame_count = 0;
	handle = mfc_encoder_init(VIDEO_WIDTH, VIDEO_HEIGHT, 15, 1000, 15);
	if (handle == 0) {
		throw TError("cannot init mfc encoder");
	}
	encoded_fp = fopen(MEDIA_FILE_NAME, "wb+");
	if (encoded_fp == 0) {
		throw TError("cannot open /tmp/cam_encoding.264");
	}

	// Post processor open
	pp_fd1 = open(PP_DEV_NAME, O_RDWR);
	if(pp_fd1 < 0)
	{
		printf("Post processor open error\n");
	}

	pp_param.src_full_width	    = VIDEO_WIDTH;
	pp_param.src_full_height	= VIDEO_HEIGHT;
	pp_param.src_start_x		= 0;
	pp_param.src_start_y		= 0;
	pp_param.src_width			= pp_param.src_full_width;
	pp_param.src_height			= pp_param.src_full_height;
	pp_param.src_color_space	= RGB16;
	pp_param.dst_start_x		= 0;
	pp_param.dst_start_y		= 0;
	pp_param.dst_full_width	    = VIDEO_WIDTH;		// destination width
	pp_param.dst_full_height	= VIDEO_HEIGHT;		// destination height
	pp_param.dst_width			= pp_param.dst_full_width;
	pp_param.dst_height			= pp_param.dst_full_height;
	pp_param.dst_color_space	= YC420;
	pp_param.out_path           = DMA_ONESHOT;
	ioctl(pp_fd1, S3C_PP_SET_PARAMS, &pp_param);

	bufSize1 = ioctl(pp_fd1, S3C_PP_GET_SRC_BUF_SIZE);
	alloc_info1.size = bufSize1;
	if ( -1 == ioctl(pp_fd1, S3C_PP_ALLOC_KMEM, &alloc_info1) )
	{
		printf("ioctl S3C_PP_ALLOC_KMEM failed1\n");
	}
	pp_param.src_buf_addr_phy = alloc_info1.phy_addr;
	ioctl(pp_fd1,S3C_PP_SET_SRC_BUF_ADDR_PHY, &pp_param);
	bufSize2 = ioctl(pp_fd1, S3C_PP_GET_SRC_BUF_SIZE);
	alloc_info2.size = bufSize2;
	if ( -1 == ioctl(pp_fd1, S3C_PP_ALLOC_KMEM, &alloc_info2) )
	{
		printf("ioctl S3C_PP_ALLOC_KMEM failed2\n");
	}
	pp_param.dst_buf_addr_phy = alloc_info2.phy_addr;
	ioctl(pp_fd1,S3C_PP_SET_DST_BUF_ADDR_PHY,&pp_param);
}

void TH264Encoder::finishEncode(){
	mfc_encoder_free(handle);
	fclose(encoded_fp);
	close(pp_fd1);
}

TH264Encoder::~TH264Encoder() {

}

unsigned char *TH264Encoder::Encode(TRect &rect, long& size)
{
	frame_count++;
	unsigned char* pRgbData = rect.getAddr();
	//convert_rgb16_to_yuv420(pRgbData, g_yuv, VIDEO_WIDTH, VIDEO_HEIGHT);
	memcpy((char *)alloc_info1.vir_addr,pRgbData,YUV_FRAME_BUFFER_SIZE);
	ioctl(pp_fd1, S3C_PP_START);
	memcpy(g_yuv,(char *)alloc_info2.vir_addr,YUV_FRAME_BUFFER_SIZE);
	if(frame_count == 1)
		encoded_buf = (unsigned char*)mfc_encoder_exe(handle, g_yuv, YUV_FRAME_BUFFER_SIZE, 1, &encoded_size);
	else
		encoded_buf = (unsigned char*)mfc_encoder_exe(handle, g_yuv, YUV_FRAME_BUFFER_SIZE, 0, &encoded_size);
	fwrite(encoded_buf, 1, encoded_size, encoded_fp);
	size = encoded_size;
	return encoded_buf;
}

void * TH264Encoder::mfc_encoder_init(int width, int height, int frame_rate, int bitrate, int gop_num)
{
	int frame_size;
	void  *handle;
	int  ret;
	frame_size  = (width * height * 3) >> 1;
	handle = SsbSipH264EncodeInit(width, height, frame_rate, bitrate, gop_num);
	if (handle == NULL) {
		LOG_MSG(LOG_ERROR, "Test_Encoder", "SsbSipH264EncodeInit Failed\n");
		return NULL;
	}
	ret = SsbSipH264EncodeExe(handle);
	return handle;
}

void *TH264Encoder::mfc_encoder_exe(void *handle, unsigned char *yuv_buf, int frame_size, int first_frame, long *size)
{
	unsigned char   *p_inbuf, *p_outbuf;
	int             hdr_size;
	int             ret;
	p_inbuf = (unsigned char*)SsbSipH264EncodeGetInBuf(handle, 0);
	memcpy(p_inbuf, yuv_buf, frame_size);
	ret = SsbSipH264EncodeExe(handle);
	if (first_frame) {
		SsbSipH264EncodeGetConfig(handle, H264_ENC_GETCONF_HEADER_SIZE, &hdr_size);
	}
	p_outbuf = (unsigned char*)SsbSipH264EncodeGetOutBuf(handle, size);
	return p_outbuf;
}

void TH264Encoder::mfc_encoder_free(void *handle)
{
	SsbSipH264EncodeDeInit(handle);
}
