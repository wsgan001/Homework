#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :QWidget(parent),ui(new Ui::Widget),Video(),Encoder(),play()//mythread(),//,FrameBuffer(),play()
{
	ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);   //去掉标题栏；
	play.FBOpen();
	capturing = false;
	has_capture = false;
	sender = new QUdpSocket(this);
	port = 8080;
	Video.opencamera() ;
    sender->connectToHost(QHostAddress("192.168.1.23"),port);
}

Widget::~Widget()
{
	Video.closecamera();
	delete sender;
	delete ui;
}

void Widget::on_capture_clicked()
{
	if(capturing == false){
		Encoder.initEncode();
		capturing = true;
		repaint();
	}
}

void Widget::on_stopcapture_clicked()
{
	//Video.stop_capturing();
	//mythread.stopcapture();
	if(capturing == true){
		capturing = false;

	}
}

void Widget::on_replay_clicked()
{
	if(capturing == false)
		play.playback();
	//close();
}

void Widget::on_quit_clicked()
{	
	close();
}

void Widget::paintEvent(QPaintEvent *){
	if(capturing == true){
		has_capture = true;
		Video.FetchPicture();
		buff = Encoder.Encode(Video,buf_size);
		QImage image(Video.getAddr(),320,240,QImage::Format_RGB16);
		QPixmap pixmap;
		pixmap.convertFromImage(image);
		ui->label->setPixmap(pixmap);
		sender->write((char *)buff,buf_size);
	}
	else if(capturing == false && has_capture == true){
		char * end = "e"; // finish mark
		Encoder.finishEncode();
		has_capture = false;
		sender->write(end,2);
	}
}
