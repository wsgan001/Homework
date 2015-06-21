#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <math.h>

#include <QtNetwork>

#include "TRect.h"
#include "TVideo.h"
//#include "TFrameBuffer.h"
#include "TH264Encoder.h"
#include "TError.h"
#include "Play.h"
//#include "Mythread.h"

namespace Ui {
	class Widget;
}

class Widget : public QWidget
{
	Q_OBJECT

public:
	explicit Widget(QWidget *parent = 0);
	~Widget();

private:
	Ui::Widget *ui;
	TVideo Video;
	TH264Encoder Encoder;
	//TFrameBuffer FrameBuffer;
	//Mythread mythread;
	Play play;
	bool capturing;
	bool has_capture;
	QUdpSocket *sender;

	quint16 port;
	unsigned char *buff;
	long buf_size;
private slots:
	void on_quit_clicked();
	void on_replay_clicked();
	void on_stopcapture_clicked();
	void on_capture_clicked();
	void paintEvent(QPaintEvent *);
};

#endif // WIDGET_H
