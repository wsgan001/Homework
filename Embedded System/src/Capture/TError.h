#ifndef TERROR_H
#define TERROR_H
#include <iostream>

class TError {
public:
	TError(const char *msg) ;
	TError(const TError &e) ;
	void Output() ;
	virtual ~TError() ;
protected:
	TError &operator=(const TError&);
private:
	const char *msg;
};

#endif // TERROR_H
