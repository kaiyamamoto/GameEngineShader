#include "TextureCache.h"

#include <WICTextureLoader.h>

using namespace DirectX;
using namespace MyLibrary;

// 定数
// リソースディレクトリパス
const std::wstring TextureCache::RESOURCE_DIRECTORY = L"Resources/Textures/";
// ファイル拡張子
const std::wstring TextureCache::RESOURCE_EXT = L".png";

// 静的メンバ変数の実体
std::unique_ptr<TextureCache> TextureCache::m_Instance;

TextureCache * MyLibrary::TextureCache::GetInstance()
{
	if (!m_Instance)
	{
		m_Instance.reset(new TextureCache);
	}

	return m_Instance.get();
}

TextureCache::TextureCache()
{
	m_d3dDevice = nullptr;
}

void TextureCache::Initialize(ID3D11Device * d3dDevice)
{
	m_d3dDevice = d3dDevice;
}

/// <summary>
/// ファイル名を指定してテクスチャ読み込み
/// </summary>
/// <param name="filename">テクスチャファイル名</param>
/// <returns>読み込んだテクスチャ</returns>
Texture* TextureCache::LoadTexture(const wchar_t * filename)
{
	HRESULT res;

	// 指定ファイルを読み込み済みでないか？
	if (m_Textures.count(filename) == 0)
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> resourceTexture;

		// フルパスに補完
		std::wstring fullpath_bin = RESOURCE_DIRECTORY + filename + RESOURCE_EXT;
		// テクスチャを読み込み、コンテナに登録（キーはファイル名）
		res = CreateWICTextureFromFile(m_d3dDevice, fullpath_bin.c_str(), resource.GetAddressOf(),
			shaderResourceView.ReleaseAndGetAddressOf());

		// 読み込み失敗
		if (FAILED(res)) 	goto FAILED;

		// リソースをテクスチャとして解釈
		res = resource.As(&resourceTexture);

		// テクスチャではない
		if (FAILED(res)) 	goto FAILED;

		// テクスチャ情報取得
		CD3D11_TEXTURE2D_DESC texDesc;
		resourceTexture->GetDesc(&texDesc);

		// 読み込んだ情報を格納
		std::unique_ptr<Texture> texture = std::make_unique<Texture>();
		texture->desc = texDesc;
		texture->shaderResourceView = shaderResourceView;

		m_Textures[filename] = std::move(texture);
	}

	return m_Textures[filename].get();

FAILED:
	return nullptr;
}