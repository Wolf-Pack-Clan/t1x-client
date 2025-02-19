#include "std_include.hpp"

#pragma comment(linker, "/merge:.text=._text")
#pragma comment(linker, "/merge:.rdata=._rdata")
#pragma comment(linker, "/merge:.data=._data")

/*
Size found using Ghidra
IMAGE_NT_HEADERS32 -> IMAGE_OPTIONAL_HEADER32 -> SizeOfStackReserve
*/
#pragma comment(linker, "/stack:0x800000")

#pragma bss_seg(".payload")
char payload_data[BINARY_PAYLOAD_SIZE];

extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 1;
	__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;
};