//
// Framework.cpp
//

#include "Framework.h"

#include <Mouse.h>
#include <Keyboard.h>
#include <string>

extern void ExitGame();

using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace MyLibrary;

// Windows procedure
LRESULT CALLBACK Framework::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	static bool s_in_sizemove = false;
	static bool s_in_suspend = false;
	static bool s_minimized = false;
	static bool s_fullscreen = false;
	// TODO: Set s_fullscreen to true if defaulting to fullscreen.

	auto framework = reinterpret_cast<Framework*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			if (!s_minimized)
			{
				s_minimized = true;
				if (!s_in_suspend && framework)
					framework->OnSuspending();
				s_in_suspend = true;
			}
		}
		else if (s_minimized)
		{
			s_minimized = false;
			if (s_in_suspend && framework)
				framework->OnResuming();
			s_in_suspend = false;
		}
		else if (!s_in_sizemove && framework)
		{
			framework->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
		}
		break;

	case WM_ENTERSIZEMOVE:
		s_in_sizemove = true;
		break;

	case WM_EXITSIZEMOVE:
		s_in_sizemove = false;
		if (framework)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);

			framework->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
		}
		break;

	case WM_GETMINMAXINFO:
	{
		auto info = reinterpret_cast<MINMAXINFO*>(lParam);
		info->ptMinTrackSize.x = 320;
		info->ptMinTrackSize.y = 200;
	}
	break;

	case WM_ACTIVATEAPP:
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);

		if (framework)
		{
			if (wParam)
			{
				framework->OnActivated();
			}
			else
			{
				framework->OnDeactivated();
			}
		}
		break;

	case WM_POWERBROADCAST:
		switch (wParam)
		{
		case PBT_APMQUERYSUSPEND:
			if (!s_in_suspend && framework)
				framework->OnSuspending();
			s_in_suspend = true;
			return TRUE;

		case PBT_APMRESUMESUSPEND:
			if (!s_minimized)
			{
				if (s_in_suspend && framework)
					framework->OnResuming();
				s_in_suspend = false;
			}
			return TRUE;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SYSKEYDOWN:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			// Implements the classic ALT+ENTER fullscreen toggle
			if (s_fullscreen)
			{
				SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
				SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

				int width = 800;
				int height = 600;
				if (framework)
					framework->GetDefaultSize(width, height);

				ShowWindow(hWnd, SW_SHOWNORMAL);

				SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			else
			{
				SetWindowLongPtr(hWnd, GWL_STYLE, 0);
				SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

				SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

				ShowWindow(hWnd, SW_SHOWMAXIMIZED);
			}

			s_fullscreen = !s_fullscreen;
		}
		break;

	case WM_MENUCHAR:
		// A menu is active and the user presses a key that does not correspond
		// to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
		return MAKELRESULT(0, MNC_CLOSE);
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

Framework::Framework(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, L"IDI_ICON");
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"GameWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, L"IDI_ICON");
	if (!RegisterClassEx(&wcex))
		throw 0;

	// Create window
	int w, h;
	GetDefaultSize(w, h);

	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.right = static_cast<LONG>(w);
	rc.bottom = static_cast<LONG>(h);

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	m_hwnd = CreateWindowEx(0, wcex.lpszClassName, L"Game", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);

	if (!m_hwnd)
		throw 0;

	ShowWindow(m_hwnd, nCmdShow);

	SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	GetClientRect(m_hwnd, &rc);

	m_deviceResources = DeviceResources::GetInstance();
	m_deviceResources->RegisterDeviceNotify(this);

	m_deviceResources->SetWindow(m_hwnd, w, h);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	// FPS表示有効
	m_IsDispFPS = true;
}

MyLibrary::Framework::~Framework()
{
}

void Framework::Initialize()
{
}

void MyLibrary::Framework::Finalize()
{
}

#pragma region Frame Update
// Executes the basic game loop.
void Framework::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

	// FPSをタイトルバーに表示
	if (m_IsDispFPS)
	{
		unsigned int fps = m_timer.GetFramesPerSecond();
		std::wstring str = L"FPS:" + std::to_wstring(fps);
		SetWindowText(m_hwnd, str.c_str());
	}

	PreRender();
    Render();
	PostRender();
}
#pragma endregion

#pragma region Frame Render
void Framework::PreRender()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	Clear();

	m_deviceResources->PIXBeginEvent(L"Render");

	m_SpriteRenderer->Begin();
}

void Framework::PostRender()
{
	m_SpriteRenderer->End();
	m_DebugText->Draw();

	m_deviceResources->PIXEndEvent();

	// Show the new frame.
	m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Framework::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    //context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearRenderTargetView(renderTarget, Colors::Black);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Framework::OnActivated()
{
    // TODO: Framework is becoming active window.
}

void Framework::OnDeactivated()
{
    // TODO: Framework is becoming background window.
}

void Framework::OnSuspending()
{
    // TODO: Framework is being power-suspended (or minimized).
}

void Framework::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Framework is being power-resumed (or returning from minimize).
}

void Framework::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Framework window is being resized.
}

// Properties
void Framework::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 1600;
    height = 900;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Framework::CreateDeviceDependentResources()
{
    ID3D11Device* device = m_deviceResources->GetD3DDevice();
	ID3D11DeviceContext* context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Initialize device dependent objects here (independent of window size).

	// スプライトマネージャ生成、初期化
	m_SpriteRenderer = SpriteRenderer::GetInstance();
	m_SpriteRenderer->Initialize(device, context);
	// テクスチャキャッシュ生成
	m_TextureCache = TextureCache::GetInstance();
	m_TextureCache->Initialize(device);
	// スプライトファクトリー生成
	m_SpriteFactory = std::make_unique<SpriteFactory>(m_SpriteRenderer, m_TextureCache);
	// デバッグテキスト作成
	m_DebugText = std::make_unique<DebugText>(device, context);

}

// Allocate all memory resources that change on a window SizeChanged event.
void Framework::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
}

void Framework::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
}

void Framework::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
