#include "base.h"

#include "window.h"
#include <glfw/glfw3.h>

#ifdef NIT_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <ShlObj_core.h>
#define P_OPEN  _popen
#define P_CLOSE _pclose
#endif

namespace nit
{
    String BrowseFile(const Window* window, const String& filter)
    {
#ifdef NIT_PLATFORM_WINDOWS
        NIT_CHECK(window && window->handler);
        OPENFILENAMEA ofn;
        CHAR sz_file[260] = {};
        CHAR current_dir[256] = {};
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window(window->handler);
        ofn.lpstrFile = sz_file;
        ofn.nMaxFile = sizeof(sz_file);
        if (GetCurrentDirectoryA(256, current_dir))
        {
            ofn.lpstrInitialDir = current_dir;
        }
        ofn.lpstrFilter = filter.c_str();
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        // Sets the default extension by extracting it from the filter
        ofn.lpstrDefExt = strchr(filter.c_str(), '\0') + 1;

        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;
        }

#endif
        return {};
    }

    String SaveFile(const Window* window, const String& filter)
    {
#ifdef NIT_PLATFORM_WINDOWS
        NIT_CHECK(window && window->handler);
        OPENFILENAMEA ofn;
        CHAR sz_file[260] = {};
        CHAR current_dir[256] = {};
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window(window->handler);
        ofn.lpstrFile = sz_file;
        ofn.nMaxFile = sizeof(sz_file);
        if (GetCurrentDirectoryA(256, current_dir))
        {
            ofn.lpstrInitialDir = current_dir;
        }
        ofn.lpstrFilter = filter.c_str();
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        // Sets the default extension by extracting it from the filter
        ofn.lpstrDefExt = strchr(filter.c_str(), '\0') + 1;

        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;
        }
#endif
        return {};
    }
}

#ifdef NIT_PLATFORM_WINDOWS
BOOL CALLBACK BrowseCallbackProcWEnum(HWND h_wnd_child, LPARAM)
{
    wchar_t buf[255];
    GetClassNameW(h_wnd_child, buf, sizeof(buf));
    if (wcscmp(buf, L"SysTreeView32") == 0)
    {
        HTREEITEM h_node = TreeView_GetSelection(h_wnd_child);
        TreeView_EnsureVisible(h_wnd_child, h_node);
        return FALSE;
    }
    return TRUE;
}

int __stdcall BrowseCallbackProcW(HWND hwnd, UINT u_msg, LPARAM lp, LPARAM data)
{
    switch (u_msg)
    {
    case BFFM_INITIALIZED:
        SendMessage(hwnd, BFFM_SETSELECTIONW, TRUE, data);
        break;
    case BFFM_SELCHANGED:
        EnumChildWindows(hwnd, BrowseCallbackProcWEnum, 0);
    }
    return 0;
}
#endif

namespace nit
{
    String SelectFolder(const Window* window, const String& title, const String& path)
    {
#ifdef NIT_PLATFORM_WINDOWS
        NIT_CHECK(window);
        static char file_response[1024];
        static wchar_t l_buff[256];
        wchar_t* l_ret_val;

        BROWSEINFOW b_info;

        const wchar_t* a_title = std::wstring(title.begin(), title.end()).c_str();

        if (!title.empty() && !wcscmp(a_title, L"folder_query")) 
        { 
            strcpy(file_response, "windows_wchar"); return ""; 
        }

        HRESULT l_h_result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

        b_info.hwndOwner = glfwGetWin32Window(window->handler);
        b_info.pidlRoot = nullptr;
        b_info.pszDisplayName = l_buff;
        b_info.lpszTitle = a_title && wcslen(a_title) ? a_title : nullptr;
        if (l_h_result == S_OK || l_h_result == S_FALSE)
        {
            b_info.ulFlags = BIF_USENEWUI;
        }
        b_info.lpfn = BrowseCallbackProcW;
        b_info.lParam = (LPARAM) path.c_str();
        b_info.iImage = -1;

        LPITEMIDLIST lp_item = SHBrowseForFolderW(&b_info);
        if (!lp_item)
        {
            l_ret_val = nullptr;
        }
        else
        {
            SHGetPathFromIDListW(lp_item, l_buff);
            l_ret_val = l_buff;
        }

        if (l_h_result == S_OK || l_h_result == S_FALSE)
        {
            CoUninitialize();
        }

        if (!l_ret_val)
        {
            return {};
        }

        std::wstring wide(l_ret_val);
        size_t length = std::wcstombs(nullptr, wide.c_str(), 0) + 1;
        std::string str(length, '\0');
        std::wcstombs(str.data(), wide.c_str(), length - 1);
        str.resize(length - 1);

        return str;
#else
        return  {};
#endif
    }

    String ExecuteCMD(const char* cmd)
    {
#ifdef NIT_PLATFORM_WINDOWS
        FixedArray<char, 128> buffer;
        String result;
        std::unique_ptr<FILE, decltype(&P_CLOSE)> pipe(P_OPEN(cmd, "r"), P_CLOSE);

        NIT_CHECK_MSG(pipe, "Pipe open failed!");

        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr)
        {
            result += buffer.data();
        }

        return result;
    }
#else
        return "";
#endif
}
