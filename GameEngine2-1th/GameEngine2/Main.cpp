//
// Main.cpp
//

#include <windows.h>
#include <wrl/client.h>
#include <d3d11_1.h>
#include <memory>

#include "Game.h"

// Indicates to hybrid graphics systems to prefer the discrete part by default
extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	std::unique_ptr<MyLibrary::Framework> framework;

    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr))
        return 1;

	try {
		framework = std::make_unique<Game>(hInstance, nCmdShow);
	}
	catch (...)
	{
		return 1;
	}
	framework->Initialize();

    // Main message loop
    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
			framework->Tick();
        }
    }

	framework->Finalize();
	framework.reset();

    CoUninitialize();

    return (int) msg.wParam;
}

// Exit helper
void ExitGame()
{
    PostQuitMessage(0);
}