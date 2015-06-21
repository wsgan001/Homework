#include "TVideo.h"





TVideo::TVideo(){
	Width =  VIDEO_WIDTH;
	Height = VIDEO_HEIGHT;
	BPP = 16;
	LineLen = Width * BPP / 8;
	Size = LineLen * Height;
	Addr = 0;
	is_close = true;
//	is_stop = true;
}

TVideo::~TVideo(){
	if(!is_close){
		::close(fd);
		fd = -1;
		delete[] Addr;
		Addr = 0;
	}
}



int TVideo::opencamera(){
	fd = ::open(DeviceNameCam, O_RDONLY);
	if (fd < 0) {
		TryAnotherCamera();
		//return 0;
	}
	is_close = false;
	Addr = new unsigned char[Size];
	Clear();
	return 1;
}

int TVideo::closecamera(){
	::close(fd);
	fd = -1;
	delete[] Addr;
	Addr = 0;
	is_close = true;
	return 1;
}

bool TVideo::FetchPicture(){
	int count = ::read(fd, Addr, Size);
	if (count != Size) {
		throw TError("error in fetching picture from video");
	}
	return true;
}

/*
int TVideo::stop_capturing(){
	is_stop = true;
	return 0;
}

bool TVideo::Is_stop(){
	return is_stop;
}

void TVideo::set_stop(bool st){
	is_stop = st;
}

*/

bool TVideo::Is_close(){
	return is_close;
}

void TVideo::TryAnotherCamera()
{
	int ret, start, found;
	struct v4l2_input chan;
	struct v4l2_framebuffer preview;
	fd = ::open("/dev/video0", O_RDWR);
	if (fd < 0) {
		throw TError("cannot open video device");
	}

	struct v4l2_capability cap;
	ret = ::ioctl(fd , VIDIOC_QUERYCAP, &cap);
	if (ret < 0) {
		throw TError("not available device");
	}

	/* Check the type - preview(OVERLAY) */
	if (!(cap.capabilities & V4L2_CAP_VIDEO_OVERLAY)) {
		throw TError("not available device");
	}
	chan.index = 0;
	found = 0;
	while(1) {
		ret = ::ioctl(fd, VIDIOC_ENUMINPUT, &chan);
		if (ret < 0) {
			throw TError("not available device");
		}

		if (chan.type &V4L2_INPUT_TYPE_CAMERA ) {
			found = 1;
			break;
		}
		chan.index++;
	}

	if (!found) {
		throw TError("not available device");
	}

	chan.type = V4L2_INPUT_TYPE_CAMERA;
	ret = ::ioctl(fd, VIDIOC_S_INPUT, &chan);
	if (ret < 0) {
		throw TError("not available device");
	}

	memset(&preview, 0, sizeof preview);
	preview.fmt.width = Width;
	preview.fmt.height = Height;
	preview.fmt.pixelformat = V4L2_PIX_FMT_RGB565;
	preview.capability = 0;
	preview.flags = 0;

	 /* Set up for preview */
	ret = ioctl(fd, VIDIOC_S_FBUF, &preview);
	if (ret< 0) {
		throw TError("not available device");
	}

	  /* Preview start */
	start = 1;
	ret = ioctl(fd, VIDIOC_OVERLAY, &start);
	if (ret < 0) {
		throw TError("not available device");
	}
}
