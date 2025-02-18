#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "loader/loader.hpp"
#include <utils/io.hpp>
#include <utils/string.hpp>

#include <dbghelp.h>
#include <fstream>

[[noreturn]] static void WINAPI exit_hook(const int code)
{
    component_loader::pre_destroy();
    std::exit(code);
}

static BOOL WINAPI system_parameters_info_a(const UINT uiAction, const UINT uiParam, const PVOID pvParam, const UINT fWinIni)
{
    component_loader::post_unpack();
    return SystemParametersInfoA(uiAction, uiParam, pvParam, fWinIni);
}

static FARPROC WINAPI get_proc_address(const HMODULE hModule, const LPCSTR lpProcName)
{
    if (lpProcName == "GlobalMemoryStatusEx"s)
        component_loader::post_unpack();
    return GetProcAddress(hModule, lpProcName);
}

static FARPROC load_binary()
{
    loader loader;
    utils::nt::library self;

    loader.set_import_resolver([self](const std::string& library, const std::string& function) -> void*
        {
            if (function == "ExitProcess")
                return exit_hook;
            if (function == "SystemParametersInfoA")
                return system_parameters_info_a;
            if (function == "GetProcAddress")
                return get_proc_address;
            
            return component_loader::load_import(library, function);
        });
    
    // Check if the CoD file is named mohaa
    std::filesystem::path currentPath_mohaa_test = std::filesystem::current_path() / "mohaa.exe";
    if (utils::io::file_exists(currentPath_mohaa_test.string()))
        game::environment::mohaa = true;

    auto client_filename = game::environment::get_client_filename();

    std::string data;
    if (!utils::io::read_file(client_filename, &data))
        throw std::runtime_error(utils::string::va("Failed to read %s\niw1x is in your CoD folder?", client_filename.data()));

    return loader.load(self, data);
}

static void enable_dpi_awareness()
{
    const utils::nt::library user32{ "user32.dll" };
    const auto set_dpi = user32 ? user32.get_proc<BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT)>("SetProcessDpiAwarenessContext") : nullptr;
    if (set_dpi)
        set_dpi(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

static LONG WINAPI CrashLogger(EXCEPTION_POINTERS* exceptionPointers)
{
    std::string crashFilename = "iw1x_crash.log";
    std::ofstream logFile(crashFilename);
    if (logFile.is_open())
    {
        HANDLE hProcess = GetCurrentProcess();
        SymInitialize(hProcess, nullptr, TRUE);
        
        auto exceptionAddress = exceptionPointers->ExceptionRecord->ExceptionAddress;
        auto exceptionCode = exceptionPointers->ExceptionRecord->ExceptionCode;
        
        IMAGEHLP_MODULE moduleInfo = { sizeof(moduleInfo) };
        SymGetModuleInfo(hProcess, reinterpret_cast<DWORD>(exceptionAddress), &moduleInfo);
        std::filesystem::path loadedImageName = moduleInfo.LoadedImageName;
        auto file = loadedImageName.filename().string();
        
        logFile << "File: " << file << std::endl;
        logFile << "Exception Address: 0x" << std::hex << exceptionAddress << std::endl;
        logFile << "Exception Code: 0x" << std::hex << exceptionCode << std::endl;

        std::string errorMessage = std::string("A crash occured, please send your ") + crashFilename + " file in the Discord server.";
        MSG_BOX_ERROR(errorMessage.c_str());
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
    SetUnhandledExceptionFilter(CrashLogger);
#if 0
    // Crash test
    * (int*)nullptr = 1;
#endif

    enable_dpi_awareness();

#ifdef DEBUG
    // Delete stock crash file
    DeleteFileA("__codmp");
    DeleteFileA("__mohaa");
#endif

    auto premature_shutdown = true;
    const auto _ = gsl::finally([&premature_shutdown]()
        {
            if (premature_shutdown)
                component_loader::pre_destroy();
        });

    FARPROC entry_point;
    try
    {
        if (!component_loader::post_start())
            return 1;

        entry_point = load_binary();
        if (!entry_point)
            throw std::runtime_error("Unable to load binary into memory");
        
        if (!component_loader::post_load())
            return 1;
    }
    catch (std::exception& ex)
    {
        MSG_BOX_ERROR(ex.what());
        return 1;
    }

    return static_cast<int>(entry_point());
}