#pragma once
// デバッグ用カメラクラス
#include "Camera.h"

namespace MyLibrary
{
	// デバッグ用カメラクラス
	class DebugCamera : public Camera
	{
		// カメラの距離
		static const float DEFAULT_CAMERA_DISTANCE;

		// 横回転
		float m_yAngle, m_yTmp;

		// 縦回転
		float m_xAngle, m_xTmp;

		// ドラッグされた座標
		int m_x, m_y;

		float m_sx, m_sy;

		// スクロールフォイール値
		int m_scrollWheelValue;

	private:

		void Motion(int x, int y);

	public:
		// コンストラクタ
		DebugCamera(int w, int h);

		// 更新
		void Update() override;
	};
}
