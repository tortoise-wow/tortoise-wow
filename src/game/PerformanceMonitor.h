#pragma once

#include "Common.h"
#include "SharedDefines.h"
#include "Timer.h"
#include "AllocatorWithCategory.h"

class ChatHandler;

struct PerformanceMonitor : public IPerfMonitor
{
	PerformanceMonitor();
    ~PerformanceMonitor();

	void Initialize();
	void FrameStart();
	void FrameEnd(uint32 delta);

	void SetReportInterval(uint32 IntervalInSeconds);

	void ReportCPU(ChatHandler& Handler);
	void ReportMemory(ChatHandler& Handler);

	virtual void ReportAlloc(const char* Category, size_t Bytes) override;
	virtual void ReportDealloc(const char* Category, size_t Bytes) override;

	XStatTimer Tick;
	XStatTimer WorldSleep;
	XStatTimer WorldTick;
	XStatTimer UpdateSession;
	XStatTimer MapManager;
	XStatTimer TEST0;

protected:

	uint32 QPC_Counter = 0;

	void ReportPerformanceToDB();

	IntervalTimer IntervalReport;

	using MemBytesMap = std::unordered_map<const char*, int64>;
	MemBytesMap MemBytes;
	std::mutex MemBytesGuard;
};

extern PerformanceMonitor sPerfMonitor;
