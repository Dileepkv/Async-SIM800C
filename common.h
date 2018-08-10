#include "printf.h"



#pragma once

typedef unsigned int uint;

#define MAX(X, Y)            \
	({                       \
		typeof(X) x_ = (X);  \
		typeof(Y) y_ = (Y);  \
		(x_ > y_) ? x_ : y_; \
	})

#define MIN(X, Y)            \
	({                       \
		typeof(X) x_ = (X);  \
		typeof(Y) y_ = (Y);  \
		(x_ < y_) ? x_ : y_; \
	})

#define ARR_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

#define PRINT_LINE() \
	printf("\n\nNOTICE: %s:%d FUNCTION[%s]\n\n", __FILE__, __LINE__, __FUNCTION__)
