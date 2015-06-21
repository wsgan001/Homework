#ifndef TRECT_H
#define TRECT_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "TError.h"

class TRect {
public:
	TRect();
	virtual ~TRect();
	bool DrawRect(const TRect &SrcRect, int x, int y) const;
	bool DrawRect(const TRect &rect) const;
	bool Clear() const;
	unsigned char *getAddr();
protected:
	TRect(const TRect&);
	TRect &operator=( const TRect&);

protected:
	unsigned char *Addr;
	int Size;
	int Width, Height, LineLen;
	unsigned BPP;
};
#endif // TRECT_H
