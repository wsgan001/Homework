#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtNetwork>

#include "Decoder.h"
#include "Play.h"
#define MAXDATA 320*240*2
#define MEDIA_FILE_NAME "/tmp/cam_encoding.264"

namespace Ui {
    class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    int openh264();
    int closeh264();
private:
    Ui::Widget *ui;

    QUdpSocket *receiver;
    quint16 port;
    unsigned char *buff;
    int buff_size;
    bool stop;

    bool has_init;
    Decoder decoder;
    unsigned char* debuff;

    FILE* encoded_fp;

    Play play;
private slots:
    void on_Playback_clicked();
    void on_Quit_clicked();
    void on_Receive_clicked();
    void on_Stop_clicked();
    void receive();
   // void paintEvent(QPaintEvent *);
};

#endif // WIDGET_H
