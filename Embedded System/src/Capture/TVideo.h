#ifndef CAPTURE_H
#define CAPTURE_H

#include "TRect.h"
#include "TError.h"
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#include "header.h"


#define DeviceNameCam "/dev/camera"


class TVideo : public TRect
{
public:
    TVideo();
    ~TVideo();
    bool FetchPicture();
    //int stop_capturing();
    int opencamera();
    int closecamera();
    bool Is_close();
    //bool Is_stop();
    //void set_stop(bool st);
private:
    int fd;
    void TryAnotherCamera();
    bool is_close;
    //bool is_stop;
};


#endif // CAPTURE_H
