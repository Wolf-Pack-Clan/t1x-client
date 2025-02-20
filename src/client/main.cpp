#include <std_include.hpp>

#include <io.hpp>
#include <string.hpp>
#include <hook.hpp>

#include "loader/loader.hpp"
#include "loader/component_loader.hpp"

#include "stock/game.hpp"

#include <dbghelp.h>
#include <fstream>

DWORD address_cgame_mp;
DWORD address_ui_mp;
utils::hook::detour hook_GetModuleFileNameW;

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

static void enable_dpi_awareness()
{
    const utils::nt::library user32{ "user32.dll" };
    const auto set_dpi = user32 ? user32.get_proc<BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT)>("SetProcessDpiAwarenessContext") : nullptr;
    if (set_dpi)
        set_dpi(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

[[noreturn]] static void WINAPI stub_ExitProcess(const int code)
{
    component_loader::pre_destroy();
    std::exit(code);
}

static BOOL WINAPI stub_SystemParametersInfoA(const UINT uiAction, const UINT uiParam, const PVOID pvParam, const UINT fWinIni)
{
    component_loader::post_unpack();
    return SystemParametersInfoA(uiAction, uiParam, pvParam, fWinIni);
}

static FARPROC WINAPI stub_GetProcAddress(const HMODULE hModule, const LPCSTR lpProcName)
{
    if (lpProcName == "GlobalMemoryStatusEx"s)
        component_loader::post_unpack();
    return GetProcAddress(hModule, lpProcName);
}

static HMODULE WINAPI stub_LoadLibraryA(LPCSTR lpLibFileName)
{
    auto ret = LoadLibraryA(lpLibFileName);
    auto hModule_address = (DWORD)GetModuleHandleA(lpLibFileName);

    if (lpLibFileName != NULL)
    {
        auto fileName = PathFindFileNameA(lpLibFileName);
        if (!strcmp(fileName, "cgame_mp_x86.dll"))
        {
            address_cgame_mp = hModule_address;
            component_loader::post_cgame();
        }
        else if (!strcmp(fileName, "ui_mp_x86.dll"))
        {
            address_ui_mp = hModule_address;
            component_loader::post_ui_mp();
        }
    }

    return ret;
}

/*
Return original client filename, so GPU driver knows what game it is,
so if it has a profile for it, it will get enabled
(this prevents buffer overrun when glGetString(GL_EXTENSIONS) gets called)
*/
// For AMD and Intel HD Graphics
static DWORD WINAPI stub_GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
    auto ret = GetModuleFileNameA(hModule, lpFilename, nSize);
    if (!strcmp(PathFindFileNameA(lpFilename), "iw1x.exe"))
    {
        std::filesystem::path path = lpFilename;
        auto binary = game::environment::get_client_filename();
        path.replace_filename(binary);
        std::string pathStr = path.string();
        std::copy(pathStr.begin(), pathStr.end(), lpFilename);
        lpFilename[pathStr.size()] = '\0';
    }
    return ret;
}
// For Nvidia
static DWORD WINAPI stub_GetModuleFileNameW(HMODULE hModule, LPWSTR lpFilename, DWORD nSize)
{
    auto* orig = static_cast<decltype(GetModuleFileNameW)*>(hook_GetModuleFileNameW.get_original());
    auto ret = orig(hModule, lpFilename, nSize);

    int required_size = WideCharToMultiByte(CP_UTF8, 0, lpFilename, -1, nullptr, 0, nullptr, nullptr);
    std::string pathStr(required_size - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, lpFilename, -1, pathStr.data(), required_size, nullptr, nullptr);

    if (!strcmp(PathFindFileNameA(pathStr.c_str()), "iw1x.exe"))
    {
        std::filesystem::path pathFs = pathStr;

        auto client_filename = game::environment::get_client_filename();
        pathFs.replace_filename(client_filename);
        pathStr = pathFs.string();

        required_size = MultiByteToWideChar(CP_UTF8, 0, pathStr.c_str(), -1, nullptr, 0);
        MultiByteToWideChar(CP_UTF8, 0, pathStr.c_str(), -1, lpFilename, required_size);
    }

    return ret;
}

static FARPROC load_binary()
{
    loader loader;
    utils::nt::library self;

    loader.set_import_resolver([self](const std::string& library, const std::string& function) -> void*
        {
            if (function == "ExitProcess")
                return stub_ExitProcess;
            if (function == "SystemParametersInfoA")
                return stub_SystemParametersInfoA;
            if (function == "GetProcAddress")
                return stub_GetProcAddress;
            if (function == "LoadLibraryA")
                return stub_LoadLibraryA;
            if (function == "GetModuleFileNameA")
                return stub_GetModuleFileNameA;

            return component_loader::load_import(library, function);
        });

    const utils::nt::library kernel32("kernel32.dll");
    hook_GetModuleFileNameW.create(kernel32.get_proc<DWORD(WINAPI*)(HMODULE, LPWSTR, DWORD)>("GetModuleFileNameW"), stub_GetModuleFileNameW);
    
    // Check if the CoD file is named mohaa
    std::filesystem::path currentPath_mohaa_test = std::filesystem::current_path() / "mohaa.exe";
    if (utils::io::file_exists(currentPath_mohaa_test.string()))
        game::environment::mohaa = true;

    auto client_filename = game::environment::get_client_filename();

    std::string data;
    if (!utils::io::read_file(client_filename, &data))
    {
        std::stringstream ss;
        ss << "Failed to read " << client_filename;
        ss << std::endl << std::endl << "Is " << MOD_NAME << " in your CoD folder?";
        throw std::runtime_error(ss.str());
    }
    return loader.load(self, data);
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