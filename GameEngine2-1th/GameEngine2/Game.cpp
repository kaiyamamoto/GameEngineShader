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
	static const int VERMAX = 10;
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

	// コモンステート作成
	m_commonStates = std::make_unique<CommonStates>(device);

	{
		// 三角形の頂点設定
		VertexPositionColorTexture vertexData;
		auto x = 0.0f;
		auto y = 0.0f;
		vertexData.position = Vector3(x, y, 0.0f);
		// 色変更　赤
		//vertexData.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

		// 色変更　黒
		vertexData.color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
		vertexData.textureCoordinate = Vector2(0.0f);
		m_vertices.push_back(vertexData);

		y -= 0.5f;
		vertexData.position = Vector3(x, y, 0.0f);
		m_vertices.push_back(vertexData);

		x += 0.5f;
		vertexData.position = Vector3(x, y, 0.0f);
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

	/* 頂点座標移動
	for (auto& vertex : m_vertices){
	vertex.position.x += 0.01f;
	}
	*/

	/* 色変更
	for (auto& vertex : m_vertices) {
	if (vertex.color.x >= 0)vertex.color.x -= 0.01f;
	if (vertex.color.z <= 1)vertex.color.z += 0.01f;
	}
	*/
}
#pragma endregion

#pragma region Frame Render
/// <summary>
/// 毎フレーム描画
/// </summary>
void Game::Render()
{
	m_ObjSkydome->Draw();

	// デバイス関連の取得
	DeviceResources* deviceResources = DeviceResources::GetInstance();
	ID3D11Device* device = deviceResources->GetD3DDevice();
	ID3D11DeviceContext* context = deviceResources->GetD3DDeviceContext();

	// デバイスコンテキストに描画ようシェーダを指定
	context->VSSetShader(m_VertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_PixelShader.Get(), nullptr, 0);

	// レイアウト指定
	context->IASetInputLayout(m_InputLayout.Get());

	// 陰面消去
	context->IASetInputLayout(m_InputLayout.Get());
	context->RSSetState(m_commonStates->CullNone());

	// αブレンディング
	context->OMSetBlendState(m_commonStates->NonPremultiplied(), nullptr, 0xffffffff);

	// 深度テスト設定
	context->OMSetDepthStencilState(m_commonStates->DepthDefault(), 0);

	m_PrimitiveBatch->Begin();

	m_PrimitiveBatch->Draw(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		&m_vertices[0],
		m_vertices.size()
	);

	m_PrimitiveBatch->End();

	// デバイスコンテキストに描画しようシェーダを指定
	context->VSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);

}
#pragma endregion
