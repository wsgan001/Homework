#ifndef TFRAMEBUFFER_H
#define TFRAMEBUFFER_H

#include "TRect.h"
#include "TError.h"
#include <linux/fb.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DeviceName "/dev/fb0"

class TFrameBuffer: public TRect {
public:
	TFrameBuffer();
	virtual ~TFrameBuffer();
	void Bufferinit();
	void Bufferclear();
protected:
	TFrameBuffer(const TFrameBuffer&);
	TFrameBuffer &operator=( const TFrameBuffer&);
private:
	int fd;
};

#endif // TFRAMEBUFFER_H
