/// <summary>
/// スプライト描画クラス
/// </summary>
#pragma once

#include <windows.h>
#include <wrl/client.h>
#include <memory>
#include <map>
#include <string>
#include <d3d11_1.h>

#include <SpriteBatch.h>
#include <SimpleMath.h>
#include <DirectXColors.h>
#include <CommonStates.h>

#include "TextureCache.h"

namespace MyLibrary
{
	class SpriteRenderer
	{
	public:
		static SpriteRenderer* GetInstance();
	private:
		static std::unique_ptr<SpriteRenderer> m_Instance;
	public:
		// 初期化
		void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
		
		// 描画受付開始
		void Begin();
		// 描画受付終了
		void End();
		// 描画
		void Draw(ID3D11ShaderResourceView* texture, DirectX::XMFLOAT2 const& position, RECT const* sourceRectangle, DirectX::FXMVECTOR color = DirectX::Colors::White, float rotation = 0, const DirectX::XMFLOAT2& origin = DirectX::SimpleMath::Vector2(0,0), const DirectX::XMFLOAT2& scale = DirectX::SimpleMath::Vector2(1,1), DirectX::SpriteEffects effects = DirectX::SpriteEffects_None, float layerDepth = 0);
	private:
		// スプライトバッチ
		std::unique_ptr<DirectX::SpriteBatch>	m_SpriteBatch;
		// 汎用描画設定
		std::unique_ptr<DirectX::CommonStates>	m_CommonStates;
	};
}