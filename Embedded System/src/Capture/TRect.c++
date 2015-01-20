#include"TRect.h"


TRect::TRect():  Addr(0), Size(0), Width(0), Height(0), LineLen(0), BPP(16) {

}

TRect::~TRect() {
 }

bool TRect::DrawRect(const TRect &SrcRect, int x, int y) const {
	if (BPP != 16 || SrcRect.BPP != 16) {
		throw TError("does not support other than 16 BPP yet");
	}

	int x0, y0, x1, y1;
	x0 = x; //80
	y0 = y; //40
	x1 = x0 + SrcRect.Width - 1; // x1 = 399
	y1 = y0 + SrcRect.Height - 1; //y1 = 279
	if (x0 < 0) {
		x0 = 0;
	}
	if (x0 > Width - 1) {  //80 > 480 ?
		return true;
	}
	if( x1 < 0) {
		return true;
	}
	if (x1 > Width - 1) { // 399 > 479 ?
		x1 = Width - 1;
	}
	if (y0 < 0) {
		y0 = 0;
	}
	if (y0 > Height - 1) {   //40 > 320 ?
		return true;
	}
	if (y1 < 0) {
		return true;
	}
	if (y1 > Height - 1) {  //279 > 319 ?
		y1 = Height -1;
	}
	// x1 = 399  y1 = 279   x0 = 80   y0 = 40
	int copyLineLen = (x1 + 1 - x0) * BPP / 8;  // 320 * 2
	//unsigned char *DstPtr = Addr + LineLen * y0 + x0 * BPP / 8;
	unsigned char *DstPtr = Addr + LineLen * y0 * 2 + (x0 - 65)* BPP / 8;
	const unsigned char *SrcPtr = SrcRect.Addr + SrcRect.LineLen *(y0 - y) + (x0 - x) * SrcRect.BPP / 8;

	for (int i = y0; i <= y1; i++) {
		memcpy(DstPtr, SrcPtr, copyLineLen);
		DstPtr += LineLen;
		SrcPtr += SrcRect.LineLen;
	}
	return true;
}

bool TRect::DrawRect(const TRect &rect) const { // default is Center
	return DrawRect(rect, (Width - rect.Width) / 2, (Height - rect.Height) / 2); //rect,80,40
}

bool TRect::Clear() const {
	int i;
	unsigned char *ptr;
	for (i = 0, ptr = Addr; i < Height; i++, ptr += LineLen) {
		memset(ptr, 0, Width * BPP / 8);
	}
	return true;
 }

unsigned char* TRect::getAddr()
{
	return Addr;
}
