//--------------------------------------------------------------------------------------
// ファイル名: CollisionNode
// 作成者:
// 作成日:
// 説明: 衝突判定用ノード
//       任意のワールド行列にぶらさげ、親子関係を結ぶことができます。
//       デバッグ用に、当たり判定を表示する機能があります。
//--------------------------------------------------------------------------------------
#pragma once

#include "Collision.h"
#include "..\3D\Obj3D.h"

namespace MyLibrary
{
	// 当たり判定ノード管理クラス
	class CollisionNode
	{
	public:
		enum TYPE
		{
			TYPE_SPHERE,	// 球
			TYPE_CAPSULE,	// カプセル

			TYPE_NUM
		};
	private:
		// デバッグ表示のON/OFFフラグ
		static bool s_DebugVisible;
	public:
		// デバッグ表示のON/OFFを設定
		static void SetDebugVisible(bool flag) { s_DebugVisible = flag; }
		// デバッグ表示のON/OFFを取得
		static bool GetDebugVisible(void) { return s_DebugVisible; }

	protected:
		// 表示用オブジェクト
		Obj3D	m_Obj;
	public:
		virtual void Initialize() = 0;
		// 描画
		virtual void Draw() = 0;
		// 更新
		virtual void Update() = 0;

		// 親を設定
		void SetParent(Obj3D* pParent);
	};

	// 当たり判定球ノード
	class SphereNode : public CollisionNode, public Sphere
	{
	public:
		// コンストラクタ
		SphereNode();
		// 初期化
		void Initialize();
		// 描画
		void Draw();
		// 更新
		void Update();
		// ローカル半径を設定
		void SetLocalRadius(float radius) { m_LocalRadius = radius; }
		// 親行列からのオフセットを設定
		void SetTrans(const DirectX::SimpleMath::Vector3& trans) { m_Trans = trans; }
	protected:
		// 親行列の影響を受ける前のローカル半径
		float m_LocalRadius;
		// 親行列からのオフセット座標
		DirectX::SimpleMath::Vector3 m_Trans;
	};

	// デバッグ表示付きカプセルノード
	class CapsuleNode : public CollisionNode, public Capsule
	{
	public:
		// コンストラクタ
		CapsuleNode();
		// 初期化
		void Initialize();
		// 描画
		void Draw();
		// 更新
		void Update();
		// 親行列からの回転を設定
		void SetRot(const DirectX::SimpleMath::Vector3& rot);
		// ローカル半径を設定
		void SetLocalRadius(float radius) { m_LocalRadius = radius; }
		// ローカル軸長さを設定
		void SetLength(float length) { m_LocalLength = length; }
		// 親行列からのオフセットを設定
		void SetTrans(const DirectX::SimpleMath::Vector3& trans) { m_Trans = trans; }

	protected:
		// 親行列の影響を受ける前のローカル軸長さ
		float m_LocalLength;
		// 親行列の影響を受ける前のローカル半径
		float m_LocalRadius;
		// 親行列からのオフセット座標
		DirectX::SimpleMath::Vector3 m_Trans;
	};
}