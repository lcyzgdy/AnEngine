#pragma once
#ifndef __DTIMER_H__
#define __DTIMER_H__

#include"onwind.h"

class DTimer
{
	LARGE_INTEGER qpcFrequency;
	LARGE_INTEGER qpcLastTime;
	UINT64 qpcMaxDelta;
	// 时间数据源使用QPC单元

	UINT64 elapsedTicks;
	UINT64 totalTicks;
	UINT64 leftOverTicks;	// ???

	UINT64 qpcSecondCounter;
	UINT32 FrameCount;
	UINT32 framesPerSecond;
	UINT32 framesThisSecond;

	bool isFixedTimeStep;
	UINT64 targetElapsedTicks;
	// 用于配置固定帧率模式

public:
	DTimer();
	~DTimer();

	const UINT64 GetElapsedTicks();
	const double GetElapsedSeconds();
	// 获取自上次调用以来所经过的时间

	const UINT64 GetTotalTicks();
	const double GetTotalSeconds();
	// 获取程序启动后的时间

	UINT32 GetFrameCount();
	// 获取程序启动以来更新的帧数

	UINT32 GetFramePerSecond();
	// 获取帧率

	void SetFixedFramerate(bool _isFixedTimeStep);
	// 设置是否使用固定帧率

	void SetTargetElapsedTicks(UINT64 _targetElapsed);
	void SetTargetElapsedSeconds(double _targetElapsed);
	// 设置更新频率

	static const UINT64 TicksPerSecond = 1000000;
	// 设置每秒使用1000000个时钟周期

	static double TicksToSeconds(UINT64 _ticks);
	static UINT64 SecondsToTicks(double _seconds);

	void ResetElapsedTime();
	// 在长时间中断（如阻塞IO、线程中断）后调用以追赶更新进度

	typedef void(*LpUpdateFunc) (void);

	void Tick(LpUpdateFunc _update);
	// 更新计时器状态，调用指定次数的update函数（OnUpdate）
};

#endif // !__DTIMER_H__
