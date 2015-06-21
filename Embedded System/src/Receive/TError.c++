#include "TError.h"

TError::TError(const char *msg) {
	this->msg = msg;
}

TError::TError(const TError &e) {
	msg = e.msg;
}

void TError::Output() {
	std::cerr << msg << std::endl;
}
TError::~TError() {}



