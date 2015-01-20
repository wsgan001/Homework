#ifndef HEADER_H
#define HEADER_H

#define MEDIA_FILE_NAME "/tmp/cam_encoding.264"
#define LCD_BPP_V4L2        V4L2_PIX_FMT_RGB565
#define VIDEO_WIDTH   320
#define VIDEO_HEIGHT  240
#define YUV_FRAME_BUFFER_SIZE   VIDEO_WIDTH*VIDEO_HEIGHT*2
#define PP_DEV_NAME     "/dev/s3c-pp"
#define INPUT_BUFFER_SIZE       (204800)
#define FB0_BPP         16
#define FB0_COLOR_SPACE RGB16




#endif // HEADER_H
