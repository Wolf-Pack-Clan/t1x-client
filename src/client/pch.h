#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

// Windows
#include <Windows.h>
#include <atlbase.h>
#include <dbghelp.h>
#include <TlHelp32.h>
#include <wincrypt.h>

// C++
#include <cassert>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <map>

// Submodules
#include <gsl/gsl>
#include <MinHook.h>

/*
Size found using Ghidra
IMAGE_NT_HEADERS32 -> IMAGE_OPTIONAL_HEADER32: SizeOfImage - SizeOfHeaders
*/
constexpr auto BINARY_PAYLOAD_SIZE = 0x4A3E000;

constexpr auto MOD_NAME = "t1x";