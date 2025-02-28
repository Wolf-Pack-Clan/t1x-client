#include "pch.h"

#include "hook.h"
#include "string.h"
#include "io.h"

#include "loader/loader.h"
#include "loader/component_loader.h"

#include "components/window.h"

bool clientNamedMohaa = false;
DWORD address_cgame_mp;
DWORD address_ui_mp;
utils::hook::detour hook_GetModuleFileNameW;
utils::hook::detour hook_GetModuleFileNameA;

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

        std::string errorMessage = "A crash occured, please send your " + crashFilename + " file in the Discord server.";

        MessageBoxA(NULL, errorMessage.c_str(), MOD_NAME, MB_ICONERROR | MB_SETFOREGROUND);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

static std::string get_client_filename()
{
    return clientNamedMohaa ? "mohaa.exe" : "CoDMP.exe";
}

static void enable_dpi_awareness()
{
    const utils::nt::library user32{ "user32.dll" };
    const auto set_dpi = user32 ? user32.get_proc<BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT)>("SetProcessDpiAwarenessContext") : nullptr;
    if (set_dpi)
        set_dpi(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

static bool registerURLProtocol()
{
    HKEY hKey;
    HKEY hCommandKey;
    
    std::string parentKeyPath = std::string("Software\\Classes\\") + MOD_NAME;
    std::string commandKeyPath = parentKeyPath + "\\shell\\open\\command";
    std::filesystem::path modPath = std::filesystem::current_path() / "iw1x.exe";
    std::string commandValue = "\"" + modPath.string() + "\" \"%1\"";
    
    // Check if the key already exists
    if (RegOpenKeyExA(HKEY_CURRENT_USER, parentKeyPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return true;
    }
    
    // Create the parent key
    if (RegCreateKeyExA(HKEY_CURRENT_USER, parentKeyPath.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hCommandKey, nullptr) != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return false;
    }
    
    // Set "URL Protocol" or it will not work
    RegSetValueExA(hCommandKey, "URL Protocol", 0, REG_SZ, NULL, 0);
    
    // Create the command key
    if (RegCreateKeyExA(HKEY_CURRENT_USER, commandKeyPath.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hCommandKey, nullptr) != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return false;
    }
    
    // Set the command key value
    if (RegSetValueExA(hCommandKey, nullptr, 0, REG_SZ, (const BYTE*)commandValue.c_str(), (DWORD)commandValue.length() + 1) != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        RegCloseKey(hCommandKey);
        return false;
    }
    
    RegCloseKey(hCommandKey);
    RegCloseKey(hKey);

    return true;
}

[[noreturn]] static void WINAPI stub_ExitProcess(const int code)
{
    component_loader::pre_destroy();
    std::exit(code);
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
    auto* orig = static_cast<decltype(GetModuleFileNameA)*>(hook_GetModuleFileNameA.get_original());
    auto ret = orig(hModule, lpFilename, nSize);
    
    if (!strcmp(PathFindFileNameA(lpFilename), "iw1x.exe"))
    {
        std::filesystem::path path = lpFilename;
        auto binary = get_client_filename();
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

        auto client_filename = get_client_filename();
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
            if (function == "GetProcAddress")
                return stub_GetProcAddress;
            if (function == "LoadLibraryA")
                return stub_LoadLibraryA;

            return component_loader::load_import(library, function);
        });

    const utils::nt::library kernel32("kernel32.dll");
    hook_GetModuleFileNameW.create(kernel32.get_proc<DWORD(WINAPI*)(HMODULE, LPWSTR, DWORD)>("GetModuleFileNameW"), stub_GetModuleFileNameW);
    hook_GetModuleFileNameA.create(kernel32.get_proc<DWORD(WINAPI*)(HMODULE, LPSTR, DWORD)>("GetModuleFileNameA"), stub_GetModuleFileNameA);
    
    // Check if the CoD file is named mohaa
    std::filesystem::path currentPath_mohaa_test = std::filesystem::current_path() / "mohaa.exe";
    if (utils::io::file_exists(currentPath_mohaa_test.string()))
        clientNamedMohaa = true;

    auto client_filename = get_client_filename();

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









DWORD getAlreadyRunningInstance()
{
    HANDLE hProcList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (hProcList == INVALID_HANDLE_VALUE)
        return NULL;

    PROCESSENTRY32 procEntry;
    procEntry.dwSize = sizeof(procEntry);

    if (Process32First(hProcList, &procEntry))
    {
        while (Process32Next(hProcList, &procEntry))
        {
            if (!strcmp(procEntry.szExeFile, "iw1x.exe"))
            {
                DWORD dwPID = procEntry.th32ProcessID;
                CloseHandle(hProcList);
                return dwPID;
            }
        }
    }
    return NULL;
}

// TODO: Don't create a function for this, if possible
HWND foundHwnd = nullptr;
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    DWORD currentPid;
    GetWindowThreadProcessId(hwnd, &currentPid);
    
    if (currentPid == static_cast<DWORD>(lParam))
    {
        foundHwnd = hwnd;
        return FALSE;
    }
    return TRUE;
}

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR lpCmdLine, _In_ int)
{
#if 0
    MessageBox(NULL, lpCmdLine, "", NULL);
#endif
    
    SetUnhandledExceptionFilter(CrashLogger);
#if 0
    // Crash test
    * (int*)nullptr = 1;
#endif

#ifdef DEBUG
    // Delete stock crash file
    DeleteFileA("__codmp");
    DeleteFileA("__mohaa");
#endif
    
    if (!registerURLProtocol())
        MessageBoxA(NULL, "registerURLProtocol failed", MOD_NAME, MB_ICONERROR | MB_SETFOREGROUND);

    enable_dpi_awareness();
    
    // Check if started using URL
    std::string cmdLine = lpCmdLine;
    if (!cmdLine.empty())
    {
        /*
        Extract IP from URL
        TODO: Shorten
        */
        const std::string prefix = "iw1x://";
        if (cmdLine.front() == '"') cmdLine.erase(0, 1);
        if (cmdLine.back() == '"') cmdLine.pop_back();
        if (cmdLine.back() == '/') cmdLine.pop_back();
        if (cmdLine.find(prefix) == 0) cmdLine.erase(0, prefix.length());
        //MessageBoxA(NULL, cmdLine.c_str(), "", NULL);
        
        if (utils::string::isValidIPPort(cmdLine))
        {
            // Check if an instance is already running
            //auto runningInstance = getAlreadyRunningInstance();
            //if (runningInstance != NULL)
            //{
                /*MessageBox(NULL, "runningInstance", "", NULL);
                
                EnumWindows(EnumWindowsProc, static_cast<LPARAM>(runningInstance));
                if (foundHwnd == nullptr)
                {
                    MessageBox(NULL, "Couldn't find window of running instance", MOD_NAME, MB_ICONERROR | MB_SETFOREGROUND);
                    return 1;
                }


                //SetForegroundWindow(foundHwnd);


                COPYDATASTRUCT cds;
                cds.dwData = window::ID_MSG_CONNECT;
                cds.cbData = strlen(cmdLine.c_str()) + 1;
                cds.lpData = (void*)cmdLine.c_str();
                SendMessage(foundHwnd, WM_COPYDATA, NULL, (LPARAM)&cds);*/

                
                //return 0;
            /*}
            else
            {*/
                // Save the arg in the window component for stub_Com_Init
                std::string arg = "+connect " + cmdLine;
                strncpy_s(window::sys_cmdline, arg.c_str(), sizeof(window::sys_cmdline));
            
                // Set the working directory
                char moduleFilename[MAX_PATH];
                GetModuleFileName(NULL, moduleFilename, sizeof(moduleFilename));
                std::filesystem::path path(moduleFilename);
                SetCurrentDirectory(path.parent_path().string().c_str());
                //MessageBoxA(NULL, path.parent_path().string().c_str(), "", NULL);
            //}
        }
    }
    
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
        MessageBoxA(NULL, ex.what(), MOD_NAME, MB_ICONERROR | MB_SETFOREGROUND);
        return 1;
    }

    return static_cast<int>(entry_point());
}