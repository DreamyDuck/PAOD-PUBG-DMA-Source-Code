#include "Include/WinHeaders.hpp"

#pragma section(".alloc")
__declspec(allocate(".alloc")) unsigned char _Scatter_buffer[0x1000];
#pragma comment( linker, "/SECTION:.alloc,EWR" )
