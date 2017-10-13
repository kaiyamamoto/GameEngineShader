/// <summary>
/// テクスチャの格納庫
/// </summary>
#pragma once

#include <windows.h>
#include <wrl/client.h>
#include <memory>
#include <map>
#include <string>
#include <d3d11_1.h>

namespace MyLibrary
{
	// テクスチャ
	struct Texture
	{
		// テクスチャ情報
		CD3D11_TEXTURE2D_DESC desc;
		// DirectX11シェーダーリソースビュー
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;
	};

	// テクスチャの格納庫
	class TextureCache
	{
	public:
		static const std::wstring RESOURCE_DIRECTORY;
		static const std::wstring RESOURCE_EXT;

		static TextureCache* GetInstance();
	private:
		static std::unique_ptr<TextureCache> m_Instance;
	public:
		TextureCache();
		// 初期化
		void Initialize(ID3D11Device* d3dDevice);
		// テクスチャ読み込み
		Texture* LoadTexture(const wchar_t* filename);
	private:
		// DirectX11デバイス
		ID3D11Device* m_d3dDevice;

		// テクスチャ配列
		std::map<std::wstring, std::unique_ptr<Texture>> m_Textures;
	};
}