#pragma once
#ifndef __DTIMER_H__
#define __DTIMER_H__

#include"onwind.h"

namespace AnEngine
{
#ifdef WIN32
	class DTimer : public NonCopyable//::Singleton<DTimer>
	{
		static DTimer* m_uniqueObj;
		//friend class ::Singleton<DTimer>;

		LARGE_INTEGER m_qpcFrequency;
		LARGE_INTEGER m_qpcLastTime;
		uint64_t m_qpcMaxDelta;
		// 时间数据源使用QPC单元

		uint64_t m_elapsedTicks;
		uint64_t m_totalTicks;
		uint64_t m_leftOverTicks;	// ???

		uint64_t m_qpcSecondCounter;
		uint32_t m_frameCount;
		uint32_t m_framesPerSecond;
		uint32_t m_framesThisSecond;

		bool m_running;

		bool m_isFixedTimeStep;
		uint64_t m_targetElapsedTicks;
		// 用于配置固定帧率模式

		DTimer();
		~DTimer();

	public:
		static DTimer* GetInstance();

		const uint64_t GetElapsedTicks();
		const double GetElapsedSeconds();
		// 获取自上次调用以来所经过的时间

		const uint64_t GetTotalTicks();
		const double GetTotalSeconds();
		// 获取程序启动后的时间

		uint32_t GetFrameCount();
		// 获取程序启动以来更新的帧数

		uint32_t GetFramePerSecond();
		// 获取帧率

		void SetFixedFramerate(bool _isFixedTimeStep);
		// 设置是否使用固定帧率

		void SetTargetElapsedTicks(uint64_t _targetElapsed);
		void SetTargetElapsedSeconds(double _targetElapsed);
		// 设置更新频率

		static const uint64_t TicksPerSecond = 1000000;
		// 设置每秒使用1000000个时钟周期

		static double TicksToSeconds(uint64_t _ticks);
		static uint64_t SecondsToTicks(double _seconds);

		void ResetElapsedTime();
		// 在长时间中断（如阻塞IO、线程中断）后调用以追赶更新进度

		typedef void(*LpUpdateFunc) (void);

		void Tick(LpUpdateFunc _update);
		// 更新计时器状态，调用指定次数的update函数（OnUpdate）
	};

	using StepTimer = DTimer;
#endif

	class Timer
	{
	public:
		static inline uint64_t _vectorcall GetTotalTicks() { return DTimer::GetInstance()->GetTotalTicks(); }
		static inline double Timer::GetTotalSeconds() { return DTimer::GetInstance()->GetTotalSeconds(); }
		static inline uint32_t GetFPS() { return DTimer::GetInstance()->GetFramePerSecond(); }
	};
}
#endif // !__DTIMER_H__
