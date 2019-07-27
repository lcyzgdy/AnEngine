#include "DTimer.h"
#include "ThreadPool.hpp"

namespace AnEngine
{
	//DTimer* DTimer::m_uniqueObj = nullptr;

	DTimer::DTimer() :
		m_elapsedTicks(0), m_frameCount(0), m_framesPerSecond(0), m_framesThisSecond(0),
		m_isFixedTimeStep(false), m_leftOverTicks(false), m_qpcSecondCounter(0),
		m_targetElapsedTicks(TicksPerSecond / 60), m_totalTicks(0)
	{
		QueryPerformanceFrequency(&m_qpcFrequency);
		QueryPerformanceCounter(&m_qpcLastTime);

		m_qpcMaxDelta = m_qpcFrequency.QuadPart / 10;
		// 将最大增量设置为0.1s
		/*Utility::u_s_threadPool.Commit([this]()
		{
			while (this->m_running)
			{
				this_thread::sleep_for(std::chrono::milliseconds(1));
				this->Tick(nullptr);
			}
		})*/;
	}

	DTimer::~DTimer()
	{
		m_running = false;
	}

	/*DTimer* DTimer::Instance()
	{
		if (m_uniqueObj == nullptr)
		{
			m_uniqueObj = new DTimer();
		}
		return m_uniqueObj;
	}*/

	const uint64_t DTimer::GetElapsedTicks()
	{
		//Tick(nullptr);
		return m_elapsedTicks;
	}

	const double DTimer::GetElapsedSeconds()
	{
		//Tick(nullptr);
		return TicksToSeconds(m_elapsedTicks);
	}

	const uint64_t DTimer::GetTotalTicks()
	{
		//Tick(nullptr);
		return m_totalTicks;
	}

	const double DTimer::GetTotalSeconds()
	{
		//Tick(nullptr);
		return TicksToSeconds(m_totalTicks);
	}

	uint32_t DTimer::GetFrameCount()
	{
		return m_frameCount;
	}

	uint32_t DTimer::GetFramePerSecond()
	{
		return m_framesPerSecond;
	}

	void DTimer::SetFixedFramerate(bool _isFixedTimeStep)
	{
		m_isFixedTimeStep = _isFixedTimeStep;
	}

	void DTimer::SetTargetElapsedTicks(uint64_t _targetElapsed)
	{
		m_targetElapsedTicks = _targetElapsed;
	}

	void DTimer::SetTargetElapsedSeconds(double _targetElapsed)
	{
		m_targetElapsedTicks = SecondsToTicks(_targetElapsed);
	}

	double DTimer::TicksToSeconds(uint64_t _ticks)
	{
		return static_cast<double>(_ticks) / TicksPerSecond;
	}

	uint64_t DTimer::SecondsToTicks(double _seconds)
	{
		return static_cast<uint64_t>(_seconds * TicksPerSecond);
	}

	void DTimer::ResetElapsedTime()
	{
		QueryPerformanceCounter(&m_qpcLastTime);

		m_leftOverTicks = 0;
		m_framesPerSecond = 0;
		m_framesThisSecond = 0;
		m_qpcSecondCounter = 0;
	}

	void DTimer::Tick(LpUpdateFunc _update)
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);

		uint64_t delta = currentTime.QuadPart - m_qpcLastTime.QuadPart;
		m_qpcLastTime = currentTime;
		m_qpcSecondCounter += delta;

		if (delta > m_qpcMaxDelta)
		{
			delta = m_qpcMaxDelta;
		}	// 控制过大的delta，例如在debug中中断

		delta *= TicksPerSecond;
		delta /= m_qpcFrequency.QuadPart;
		// QPC单位转国际单位

		uint32_t lastFrameCount = m_frameCount;

		if (m_isFixedTimeStep)
		{
			if (abs(static_cast<int>(delta - m_targetElapsedTicks)) < TicksPerSecond / 4000)	// ??
			{
				delta = m_targetElapsedTicks;
			}
			m_leftOverTicks += delta;
			// 防止失之毫厘谬以千里。固定帧率时，打开了垂直同步的程序会由于显示器的问题累积下微小误差

			while (m_leftOverTicks >= m_targetElapsedTicks)
			{
				m_elapsedTicks = m_targetElapsedTicks;
				m_totalTicks += m_targetElapsedTicks;
				m_leftOverTicks -= m_targetElapsedTicks;
				m_frameCount++;

				if (_update)
				{
					_update();
				}
			}
		}
		else
		{
			m_elapsedTicks = delta;
			m_totalTicks += delta;
			m_leftOverTicks = 0;
			m_frameCount++;
			if (_update)
			{
				_update();
			}
		}

		if (m_frameCount != lastFrameCount)
		{
			m_framesThisSecond++;
		}
		if (m_qpcSecondCounter >= static_cast<uint64_t>(m_qpcFrequency.QuadPart))
		{
			m_framesPerSecond = m_framesThisSecond;
			m_framesThisSecond = 0;
			m_qpcSecondCounter %= m_qpcFrequency.QuadPart;
		}
	}
}