#pragma once

/*
Size found using Ghidra
IMAGE_NT_HEADERS32 -> IMAGE_OPTIONAL_HEADER32: SizeOfImage - SizeOfHeaders
*/
constexpr auto BINARY_PAYLOAD_SIZE = 0x15C1000;

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <atlbase.h>
#include <GL/gl.h>

// min and max is required by gdi, therefore NOMINMAX won't work
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

constexpr auto MOD_NAME = "iw1x";

#define MSG_BOX_INFO(message) MessageBoxA(nullptr, message, MOD_NAME, MB_ICONINFORMATION | MB_SETFOREGROUND);
#define MSG_BOX_WARN(message) MessageBoxA(nullptr, message, MOD_NAME, MB_ICONWARNING | MB_SETFOREGROUND);
#define MSG_BOX_ERROR(message) MessageBoxA(nullptr, message, MOD_NAME, MB_ICONERROR | MB_SETFOREGROUND);

#include <filesystem>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>

#include <gsl/gsl>

#include <MinHook.h>
#include <imgui.h>
#include <backends/imgui_impl_opengl2.h>
#include <backends/imgui_impl_win32.h>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "opengl32.lib")

using namespace std::literals;