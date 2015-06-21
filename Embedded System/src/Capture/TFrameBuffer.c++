#include "TFrameBuffer.h"

TFrameBuffer::TFrameBuffer(): TRect(), fd(-1) {

}

void TFrameBuffer::Bufferinit(){
	Addr = (unsigned char *)MAP_FAILED;
	fd = open(DeviceName, O_RDWR);
	if (fd < 0) {
		throw TError("cannot open frame buffer");
	}
	struct fb_fix_screeninfo Fix;
	struct fb_var_screeninfo Var;
	if (ioctl(fd, FBIOGET_FSCREENINFO, &Fix) < 0 || ioctl(fd, FBIOGET_VSCREENINFO, &Var) < 0) {
		throw TError("cannot get frame buffer information");
	}
	BPP = Var.bits_per_pixel;
	if (BPP != 16) {
		throw TError("support 16BPP frame buffer only");
	}
	Width  = Var.xres;
	Height = Var.yres;
	LineLen = Fix.line_length;
	Size = LineLen * Height;
	int PageSize = getpagesize();
	Size = (Size + PageSize - 1) / PageSize * PageSize ;
	Addr = (unsigned char *)mmap(NULL, Size, PROT_READ|PROT_WRITE,MAP_SHARED, fd, 0);
	if (Addr == (unsigned char *)MAP_FAILED) {
		throw TError("map frame buffer failed");
		return;
	}
	::close(fd);
	fd = -1;
	//Clear();
}

void TFrameBuffer::Bufferclear(){
	::munmap(Addr, Size);
	Addr = (unsigned char *)MAP_FAILED;
	::close(fd);
	fd = -1;
}

TFrameBuffer:: ~TFrameBuffer() {

}

