#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :QWidget(parent),ui(new Ui::Widget),decoder(),play()
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    receiver = new QUdpSocket(this);
    port = 8080;
    receiver->bind(port,QUdpSocket::ShareAddress);
    stop = true;

    connect(receiver,SIGNAL(readyRead()),this,SLOT(receive()));
    has_init = false;
}


Widget::~Widget()
{
	delete receiver;
	delete ui;
}

void Widget::receive()
 {

	if(stop == false){
		QByteArray data;
		while(receiver->hasPendingDatagrams()){
			data.resize(receiver->pendingDatagramSize());
			receiver->readDatagram(data.data(),data.size());
		}

		buff_size = data.size();
		buff = (unsigned char*)data.data();

		if(buff_size < 5){
			 stop = true;
			 if(has_init) decoder.finishdecode();
			 closeh264();
			 decoder.closeDevice();
		}
		else{
			fwrite(buff, 1, buff_size, encoded_fp);
			 //fwrite(data.data(), 1, data.size(), encoded_fp);

			if(!has_init){
				decoder.predecode(buff,buff_size);
				has_init = true;
			}
			else
				decoder.nextframe(buff,buff_size);

			decoder.decode(buff_size);
			debuff = decoder.getBuf();

			QImage image(debuff,320,240,QImage::Format_RGB16);
			QPixmap pixmap;
			pixmap.convertFromImage(image);
			ui->label->setPixmap(pixmap);

			//repaint();



		}
    }
 }

int Widget::openh264(){
	// in file open
	encoded_fp = fopen(MEDIA_FILE_NAME, "wb+");
	if (encoded_fp == 0) {
		return -1;
	}
	return 0;
}

int Widget::closeh264(){
	fclose(encoded_fp);
	return 0;
}

void Widget::on_Stop_clicked()
{
	stop  = true;
}

void Widget::on_Receive_clicked()
{
	if(stop == true){
		 stop = false;
		 decoder.openDevice();
		 openh264();
		 has_init = false;
	}
}

void Widget::on_Quit_clicked()
{
	if(stop)
		close();
}

void Widget::on_Playback_clicked()
{
	if(stop)
		play.playback();
}

/*
void Widget::paintEvent(QPaintEvent *){
	if(!stop){
		QImage image(debuff,320,240,QImage::Format_RGB16);
		QPixmap pixmap;
		pixmap.convertFromImage(image);
		ui->label->setPixmap(pixmap);
	}
}
*/
