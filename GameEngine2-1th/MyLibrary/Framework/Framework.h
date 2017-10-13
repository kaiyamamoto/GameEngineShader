/// <summary>
/// フレームワーク基底クラス
/// </summary>
#pragma once

#include <DirectXMath.h>
#include <DirectXColors.h>

#include "../Common/DeviceResources.h"
#include "../Common/StepTimer.h"

#include "../2D/SpriteRenderer.h"
#include "../2D/Sprite.h"
#include "../2D/DebugText.h"

namespace MyLibrary
{
	class Framework : public IDeviceNotify
	{
	public:

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		Framework(HINSTANCE hInstance, int nCmdShow);

		virtual ~Framework();

		virtual void Initialize();

		virtual void Finalize();

		// Basic game loop
		virtual void Tick();

		// IDeviceNotify
		virtual void OnDeviceLost() override;
		virtual void OnDeviceRestored() override;

		// Messages
		virtual void OnActivated();
		virtual void OnDeactivated();
		virtual void OnSuspending();
		virtual void OnResuming();
		virtual void OnWindowSizeChanged(int width, int height);

		// Properties
		void GetDefaultSize(int& width, int& height) const;

		void SetIsDispFPS(bool flag) { m_IsDispFPS = flag; }

	protected:

		virtual void Update(StepTimer const& timer) = 0;
		virtual void PreRender();
		virtual void PostRender();
		virtual void Render() = 0;
		virtual void Clear();

		virtual void CreateDeviceDependentResources();
		virtual void CreateWindowSizeDependentResources();

		HWND m_hwnd;

		// Device resources.
		DeviceResources*    m_deviceResources;

		// Rendering loop timer.
		StepTimer                           m_timer;

		// デバッグテキスト
		std::unique_ptr<DebugText>	m_DebugText;
		// スプライトレンダラー
		SpriteRenderer* m_SpriteRenderer;
		// テクスチャキャッシュ
		TextureCache* m_TextureCache;
		// スプライトファクトリー
		std::unique_ptr<SpriteFactory> m_SpriteFactory;

		// FPS表示有効
		bool m_IsDispFPS;
	};
}