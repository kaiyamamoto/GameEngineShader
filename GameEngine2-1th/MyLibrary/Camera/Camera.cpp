#include "Camera.h"

#include "..\Common\DeviceResources.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace MyLibrary;

/**
*	@brief コンストラクタ
*/
Camera::Camera(int width, int height)
{
	m_View = Matrix::Identity;
	m_Eyepos = Vector3(0.0f, 6.0f, 10.0f);
	m_Refpos = Vector3(0.0f, 2.0f, 0.0f);
	m_Upvec = Vector3(0.0f, 1.0f, 0.0f);

	m_Proj = Matrix::Identity;
	m_FovY = XMConvertToRadians(60.0f);
	m_Aspect = (float)width / height;
	m_NearClip = 0.1f;
	m_FarClip = 1000.0f;
}

/**
*	@brief デストラクタ
*/
Camera::~Camera()
{
}

/**
*	@brief 更新
*/
void Camera::Update()
{
	// ビュー行列を計算
	m_View = Matrix::CreateLookAt(m_Eyepos, m_Refpos, m_Upvec);
	// プロジェクション行列を計算
	m_Proj = Matrix::CreatePerspectiveFieldOfView(m_FovY, m_Aspect, m_NearClip, m_FarClip);
	// ビルボード行列を計算
	CalcBillboard();
}

/// <summary>
/// ３Ｄ→２Ｄ座標変換
/// ワールド座標からスクリーン座標を計算する
/// </summary>
/// <param name="pos2d"></param>
/// <returns>成否</returns>
bool Camera::Project(const Vector3& worldPos, Vector2* screenPos)
{
	Vector4 clipPos;
	Vector4 worldPosV4(worldPos.x, worldPos.y, worldPos.z, 1.0f);

	// ビュー変換
	clipPos = Vector4::Transform(worldPosV4, m_View);

	// 射影変換
	clipPos = Vector4::Transform(clipPos, m_Proj);

	// ビューポートの取得
	D3D11_VIEWPORT viewport = DeviceResources::GetInstance()->GetScreenViewport();

	// ニアクリップより前方にいなければ計算できない
	if (clipPos.w <= 1.0e-5) return false;

	// ビューポート変換
	float ndcX = clipPos.x / clipPos.w;
	float ndcY = -clipPos.y / clipPos.w;

	screenPos->x = (ndcX + 1.0f) * 0.5f * viewport.Width + viewport.TopLeftX;
	screenPos->y = (ndcY + 1.0f) * 0.5f * viewport.Height + viewport.TopLeftY;

	return true;
}

/// <summary>
/// ２Ｄ→３Ｄ座標変換
/// スクリーン座標を、ニアクリップ、ファークリップ間の線分に変換する
/// </summary>
/// <param name="screenPos"></param>
/// <param name="worldSegment"></param>
void Camera::UnProject(const Vector2& screenPos, Segment* worldSegment)
{
	// ニアクリップ平面でのワールド座標を得る
	UnProject(screenPos, m_NearClip, &worldSegment->start);
	// ファークリップ平面でのワールド座標を得る
	UnProject(screenPos, m_FarClip, &worldSegment->end);
}

/// <summary>
/// ２Ｄ→３Ｄ座標変換
/// スクリーン座標を、ワールド座標に変換する
/// </summary>
/// <param name="screenPos">スクリーン座標</param>
/// <param name="distance">カメラからの距離</param>
/// <param name="worldPos">ワールド座標（出力）</param>
void Camera::UnProject(const Vector2& screenPos, float distance, Vector3* worldPos)
{
	Vector2 clipPos;
	Vector4 clipPosV4;

	// ビューポートの取得
	D3D11_VIEWPORT viewport = DeviceResources::GetInstance()->GetScreenViewport();

	// スクリーン座標→射影座標
	clipPos.x = (screenPos.x - viewport.TopLeftX) / (viewport.Width / 2.0f) - 1.0f;
	clipPos.y = (screenPos.y - viewport.TopLeftY) / (viewport.Height / 2.0f) - 1.0f;
	clipPos.y = -clipPos.y;

	clipPosV4.x = distance * clipPos.x;
	clipPosV4.y = distance * clipPos.y;
	clipPosV4.z = (-distance * m_FarClip + m_NearClip * m_FarClip) / (m_NearClip - m_FarClip);
	clipPosV4.w = distance;

	// プロジェクション、ビュー逆変換
	Matrix invMat = m_View * m_Proj;
	invMat.Invert(invMat);

	Matrix invView;
	m_View.Invert(invView);

	Matrix invProj;
	m_Proj.Invert(invProj);

	// 射影座標→ビュー座標
	Vector4 viewPos = Vector4::Transform(clipPosV4, invProj);
	// ビュー座標→ワールド座標
	Vector4 world = Vector4::Transform(viewPos, invView);

	worldPos->x = world.x;
	worldPos->y = world.y;
	worldPos->z = world.z;
}

/// <summary>
/// ビルボード行列の計算
/// </summary>
void Camera::CalcBillboard()
{
	// 視線方向
	Vector3 eyeDir = m_Eyepos - m_Refpos;
	// Y軸
	Vector3 Y = Vector3::UnitY;
	// X軸
	Vector3 X = Y.Cross(eyeDir);
	X.Normalize();
	// Z軸
	Vector3 Z = X.Cross(Y);
	Z.Normalize();
	// Y軸周りビルボード行列（右手系の為Z方向反転）
	m_BillboardConstrainY = Matrix::Identity;
	m_BillboardConstrainY.m[0][0] = X.x;
	m_BillboardConstrainY.m[0][1] = X.y;
	m_BillboardConstrainY.m[0][2] = X.z;
	m_BillboardConstrainY.m[1][0] = Y.x;
	m_BillboardConstrainY.m[1][1] = Y.y;
	m_BillboardConstrainY.m[1][2] = Y.z;
	m_BillboardConstrainY.m[2][0] = -Z.x;
	m_BillboardConstrainY.m[2][1] = -Z.y;
	m_BillboardConstrainY.m[2][2] = -Z.z;

	Y = eyeDir.Cross(X);
	Y.Normalize();
	eyeDir.Normalize();
	// ビルボード行列（右手系の為Z方向反転）
	m_Billboard = Matrix::Identity;
	m_Billboard.m[0][0] = X.x;
	m_Billboard.m[0][1] = X.y;
	m_Billboard.m[0][2] = X.z;
	m_Billboard.m[1][0] = Y.x;
	m_Billboard.m[1][1] = Y.y;
	m_Billboard.m[1][2] = Y.z;
	m_Billboard.m[2][0] = -eyeDir.x;
	m_Billboard.m[2][1] = -eyeDir.y;
	m_Billboard.m[2][2] = -eyeDir.z;
}
