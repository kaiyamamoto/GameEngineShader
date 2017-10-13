/// <summary>
/// パフォーマンス計測クラス
/// 
/// 高分解能パフォーマンスカウンタを用いて、
/// 1ミリ秒より細かい精度で処理時間を計測します
/// </summary>
#pragma once

#include <windows.h>
#include <memory>

namespace MyLibrary
{
	class PerformanceCounter
	{
	/// <summary>
	/// 静的メンバ
	/// </summary>
	public:
		// シングルトンインスタンスの取得
		static PerformanceCounter* GetInstance();
	private:
		// シングルトンインスタンス
		static std::unique_ptr<PerformanceCounter> m_Instance;

	/// <summary>
	/// インスタンスメンバ
	/// </summary>
	private:
		// 高分解能パフォーマンスカウンタの周波数
		LARGE_INTEGER	m_Frequency;
		// 計測開始時間
		LARGE_INTEGER	m_BeginTime;
		// 計測終了時間
		LARGE_INTEGER	m_EndTime;
		// 計測中フラグ
		bool	m_isCount;
	public:
		// コンストラクタ
		PerformanceCounter();
		// 計測開始
		void Begin();
		// 計測終了
		void End();
		// 経過時間を秒単位で取得
		double GetElapsedTime();
	};
}
