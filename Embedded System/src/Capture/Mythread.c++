#include "Mythread.h"

Mythread::Mythread(): 	Video(),Encoder(),FrameBuffer(){
	is_stop = true;
}

Mythread::~Mythread(){
}

void Mythread::run(){



	try {
		Video.opencamera() ;
		Encoder.initEncode();
		FrameBuffer.Bufferinit();
		//Video.set_stop(false);
		is_stop = false;
		//while(!Video.Is_stop()) {
		//for (int countt = 0;countt < 500;countt++) {
		while(! is_stop){
			Video.FetchPicture();
			//Encoder.Encode(Video);
			FrameBuffer.DrawRect(Video);
		}
	} catch (TError &e) {
		e.Output();
	}
	FrameBuffer.Bufferclear();
	Encoder.finishEncode();
	Video.closecamera();
}

void Mythread::stopcapture(){
	//Video.stop_capturing();
	is_stop = true;
}
