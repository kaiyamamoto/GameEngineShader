#include "PerformanceCounter.h"

using namespace MyLibrary;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
std::unique_ptr<PerformanceCounter> PerformanceCounter::m_Instance;

PerformanceCounter * PerformanceCounter::GetInstance()
{
	if (!m_Instance)
	{
		m_Instance.reset(new PerformanceCounter);
	}

	return m_Instance.get();
}

PerformanceCounter::PerformanceCounter()
{
	m_isCount = false;
}

void PerformanceCounter::Begin()
{
	// 周波数を取得
	if (!QueryPerformanceFrequency(&m_Frequency))
	{
		// 計測不能
		return;
	}

	//現在の時間を取得
	if (!QueryPerformanceCounter(&m_BeginTime))
	{
		// 計測不能
		return;
	}

	// 計測中になった
	m_isCount = true;
}

void PerformanceCounter::End()
{
	//現在の時間を取得
	if (!QueryPerformanceCounter(&m_EndTime))
	{
		// 計測不能
		return;
	}

	// 計測中ではなくなった
	m_isCount = false;
}

double PerformanceCounter::GetElapsedTime()
{
	// まだ計測中
	if (m_isCount)	return 0.0;

	// 分解能単位での経過時間
	LONGLONG time = m_EndTime.QuadPart - m_BeginTime.QuadPart;

	// 単位を秒に直す
	double time_d = double(time) / double(m_Frequency.QuadPart);

	return time_d;
}
