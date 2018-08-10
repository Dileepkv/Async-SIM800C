#pragma once

#ifdef WINDOWS
#include <stdio.h>
#endif

#ifdef ARDUINO
#include "ardprintf.h"
#include <stdio.h>

//#include "ardprintf.cpp"
#define printf ardprintf
#endif
