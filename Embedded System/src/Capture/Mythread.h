#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include "TRect.h"
#include "TVideo.h"
#include "TFrameBuffer.h"
#include "TH264Encoder.h"
#include "TError.h"
class Mythread :  public QThread{
	Q_OBJECT
protected:
	void run();
private:
	bool is_stop;
	TVideo Video;
	TH264Encoder Encoder;
	TFrameBuffer FrameBuffer;
public:
	Mythread();
	~Mythread();
	void stopcapture();
};


#endif // MYTHREAD_H
