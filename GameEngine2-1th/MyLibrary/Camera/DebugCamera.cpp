// デバッグ用カメラクラス
#include "DebugCamera.h"
#include "..\Input\MouseUtil.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace MyLibrary;

const float DebugCamera::DEFAULT_CAMERA_DISTANCE = 5.0f;


// コンストラクタ
DebugCamera::DebugCamera(int w, int h)
	: Camera(w,h), m_yAngle(0.0f), m_yTmp(0.0f), m_xAngle(0.0f), m_xTmp(0.0f), m_x(0), m_y(0), m_scrollWheelValue(0)
{
	// 画面サイズに対する相対的なスケールに調整
	m_sx = 1.0f / (float)w;
	m_sy = 1.0f / (float)h;
	m_View = DirectX::SimpleMath::Matrix::Identity;
}

//--------------------------------------------------------------------------------------
// 更新
//--------------------------------------------------------------------------------------
void DebugCamera::Update()
{
	// マウス情報を取得
	MouseUtil* mouse = MouseUtil::GetInstance();

	// マウスの左ボタンが押された
	if (mouse->IsTriggered(MouseUtil::Button::Left))
	{
		XMINT2 pos = mouse->GetPos();
		// マウスの座標を取得
		m_x = pos.x;
		m_y = pos.y;
	}
	else if (mouse->IsReleased(MouseUtil::Button::Left))
	{
		// 現在の回転を保存
		m_xAngle = m_xTmp;
		m_yAngle = m_yTmp;
	}
	// マウスのボタンが押されていたらカメラを移動させる
	if (mouse->IsPressed(MouseUtil::Button::Left))
	{
		XMINT2 pos = mouse->GetPos();
		Motion(pos.x, pos.y);
	}

	// マウスのフォイール値を取得
	m_scrollWheelValue = mouse->GetWheelValue();
	if (m_scrollWheelValue > 0)
	{
		m_scrollWheelValue = 0;
		mouse->ResetWheelValue();
	}

	// ビュー行列を算出する
	Matrix rotY = Matrix::CreateRotationY(m_yTmp);
	Matrix rotX = Matrix::CreateRotationX(m_xTmp);

	Matrix rt = rotY * rotX;

	Vector3 eye(0.0f, 0.0f, 1.0f);
	Vector3 target(0.0f, 0.0f, 0.0f);
	Vector3 up(0.0f, 1.0f, 0.0f);

	eye = Vector3::Transform(eye, rt.Invert());
	eye *= (DEFAULT_CAMERA_DISTANCE - m_scrollWheelValue / 100);
	up = Vector3::Transform(up, rt.Invert());

	Camera::SetEyepos(eye);
	Camera::SetRefpos(target);
	Camera::SetUpvec(up);
	Camera::Update();
}

//--------------------------------------------------------------------------------------
// 行列の生成
//--------------------------------------------------------------------------------------
void DebugCamera::Motion(int x, int y)
{
	// マウスポインタの位置のドラッグ開始位置からの変位 (相対値)
	float dx = (x - m_x) * m_sx;
	float dy = (y - m_y) * m_sy;

	if (dx != 0.0f || dy != 0.0f)
	{
		// Ｙ軸の回転
		float yAngle = dx * XM_PI;
		// Ｘ軸の回転
		float xAngle = dy * XM_PI;

		m_xTmp = m_xAngle + xAngle;
		m_yTmp = m_yAngle + yAngle;
	}
}