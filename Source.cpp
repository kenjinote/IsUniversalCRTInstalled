#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <vector>
#include <string>

TCHAR szClassName[] = TEXT("Window");

BOOL IsUniversalCRTInstalled()
{
	LPCWSTR lpszSearchPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SideBySide\\Winners";
	std::vector<std::wstring>keylist;
	WCHAR szTemp[1024];
	HKEY hKey;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, lpszSearchPath, 0, KEY_READ | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS)
	{
		DWORD cSubKeys = 0;
		if (RegQueryInfoKeyW(hKey, 0, 0, 0, &cSubKeys, 0, 0, 0, 0, 0, 0, 0) == ERROR_SUCCESS && cSubKeys)
		{
			for (DWORD i = 0; i < cSubKeys; ++i)
			{
				DWORD cbName = _countof(szTemp);
				if (RegEnumKeyExW(hKey, i, szTemp, &cbName, 0, 0, 0, 0) == ERROR_SUCCESS)
				{
					for (auto architecture : { L"x86", L"amd64", L"wow64" })
					{
						if (szTemp[0] != architecture[0]) continue;
						WCHAR szKey[1024];
						lstrcpyW(szKey, architecture);
						lstrcatW(szKey, L"_microsoft-windows-ucrt_31bf3856ad364e35");
						const int nKeyLength = lstrlenW(szKey);
						if (CompareStringW(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, szKey, nKeyLength, szTemp, nKeyLength) == CSTR_EQUAL)
						{
							keylist.push_back(szTemp);
						}
					}
				}
			}
		}
		RegCloseKey(hKey);
	}
	if (keylist.size() == 0)
	{
		return FALSE;
	}
	for (auto key : keylist)
	{
		lstrcpyW(szTemp, lpszSearchPath);
		lstrcatW(szTemp, L"\\");
		lstrcatW(szTemp, key.c_str());
		if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, szTemp, 0, KEY_READ | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS)
		{
			DWORD dwType = REG_SZ;
			DWORD cbName = _countof(szTemp);
			if (RegQueryValueEx(hKey, 0, 0, &dwType, (LPBYTE)szTemp, &cbName) == ERROR_SUCCESS)
			{
				if (lstrlenW(szTemp) > 0)
				{
					RegCloseKey(hKey);
					return TRUE;
				}
			}
			RegCloseKey(hKey);
		}
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton;
	switch (msg)
	{
	case WM_CREATE:
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("Universal CRT がインストールされているかどうか判定"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SIZE:
		MoveWindow(hButton, 10, 10, 512, 32, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			MessageBox(hWnd, IsUniversalCRTInstalled() ? TEXT("Universal CRT はインストールされています。") : TEXT("Universl CRT はインストールされていません。"), TEXT("確認"), 0);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("Universal CRT がインストールされているかどうか判定"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
