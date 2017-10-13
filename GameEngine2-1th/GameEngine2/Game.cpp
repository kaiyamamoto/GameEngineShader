//
// Game.cpp
//

#include "Game.h"

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;
using namespace MyLibrary;

namespace {
	static const int VERMAX = 10000;
}

// 入力レイアウトの定義
const std::vector<D3D11_INPUT_ELEMENT_DESC> Game::INPUT_LAYOUT = 
{
	{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,sizeof(Vector3),D3D11_INPUT_PER_VERTEX_DATA,0},
	{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,sizeof(Vector3)+sizeof(Vector4),D3D11_INPUT_PER_VERTEX_DATA,0}
};

Game::Game(HINSTANCE hInstance, int nCmdShow)
	: Framework(hInstance, nCmdShow)
{
}

void Game::Initialize()
{
	DeviceResources* deviceResources = DeviceResources::GetInstance();

	// マウスライブラリにウィンドウハンドルを渡す
	MouseUtil::GetInstance()->SetWindow(DeviceResources::GetInstance()->GetWindow());
	
	// ウィンドウ矩形取得
	RECT windowRect = deviceResources->GetOutputSize();
	// デバッグカメラ作成
	m_Camera = std::make_unique<DebugCamera>(windowRect.right- windowRect.left, windowRect.bottom - windowRect.top);

	{ // Obj3D初期化
		// 設定
		Obj3D::CommonDef def;
		def.pCamera = m_Camera.get();
		def.pDevice = deviceResources->GetD3DDevice();
		def.pDeviceContext = deviceResources->GetD3DDeviceContext();
		// 設定を元に初期化
		Obj3D::InitializeCommon(def);
	}

	// 天球読み込み
	m_ObjSkydome = std::make_unique<Obj3D>();
	m_ObjSkydome->LoadModel(L"skydome");
	m_ObjSkydome->DisableLighting();

	// デバイス関連の取得
	ID3D11Device* device = deviceResources->GetD3DDevice();
	ID3D11DeviceContext* context = deviceResources->GetD3DDeviceContext();

	// 頂点シェーダ
	BinaryFile VSData = BinaryFile::LoadFile(L"Resources/Shaders/particleVS.cso");
	// ピクセルシェーダ
	BinaryFile PSData = BinaryFile::LoadFile(L"Resources/Shaders/particlePS.cso");

	// 頂点シェーダ作成
	if (FAILED(device->CreateVertexShader(VSData.GetData(), VSData.GetSize(), NULL, m_VertexShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreateVertexShader Failed.", NULL, MB_OK);
		return;
	}

	// ピクセルシェーダ作成
	if (FAILED(device->CreatePixelShader(PSData.GetData(), PSData.GetSize(), NULL, m_PixelShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreatePixelShader Failed.", NULL, MB_OK);
		return;
	}

	// プリミティブバッチ作成
	m_PrimitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColorTexture>>(context, 1, VERMAX + 1);

	// 頂点フォーマットを指定して入力レイアウト作成
	device->CreateInputLayout(&INPUT_LAYOUT[0],
		INPUT_LAYOUT.size(),
		VSData.GetData(), VSData.GetSize(),
		m_InputLayout.GetAddressOf());

	for (int i = 0; i < VERMAX; i++)
	{
		VertexPositionColorTexture vertexData;

		// スクリーンサイズ取得
		int scx, scy;
		Framework::GetDefaultSize(scx, scy);

		auto x = (0.5f*(float)scy / scx) * cos(2.0 * 3.14 * i);
		auto y = 0.5f * sin(2.0 * 3.14 * i);
		
		/* 四角
		auto x = 0.5f * cos(i*(scx*scy));
		auto y = 0.5f * sin(i);
		*/

		vertexData.position = Vector3(x, y, 0.0f);
		vertexData.textureCoordinate = Vector2(0.0f);
		m_vertices.push_back(vertexData);
	}
}

void Game::Finalize()
{

}

#pragma region Frame Update
/// <summary>
/// 毎フレーム更新
/// </summary>
/// <param name="timer">時間情報</param>
void Game::Update(StepTimer const& timer)
{
	MouseUtil::GetInstance()->Update();

	m_Camera->Update();
}
#pragma endregion

#pragma region Frame Render
/// <summary>
/// 毎フレーム描画
/// </summary>
void Game::Render()
{
	// デバイス関連の取得
	DeviceResources* deviceResources = DeviceResources::GetInstance();
	ID3D11Device* device = deviceResources->GetD3DDevice();
	ID3D11DeviceContext* context = deviceResources->GetD3DDeviceContext();

	//m_ObjSkydome->Draw();

	// デバイスコンテキストに描画しようシェーダを指定
	context->VSSetShader(m_VertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_PixelShader.Get(), nullptr, 0);

	// レイアウト指定
	context->IASetInputLayout(m_InputLayout.Get());

	m_PrimitiveBatch->Begin();

	m_PrimitiveBatch->Draw(
		D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
		&m_vertices[0],
		m_vertices.size()
	);

	m_PrimitiveBatch->End();

	// デバイスコンテキストに描画しようシェーダを指定
	context->VSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);


}
#pragma endregion
