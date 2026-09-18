#include "PerformanceMonitor.h"
#include "Chat.h"
#include "World.h"
#include "MapManager.h"

PerformanceMonitor sPerfMonitor;

PerformanceMonitor::PerformanceMonitor()
{
	gPerfMonitorInterface = this;
}

PerformanceMonitor::~PerformanceMonitor()
{
    // Other global containers can be destroyed after this monitor. Unpublish
    // before MemBytes/its mutex are destroyed, so their allocator callbacks do
    // not enter an already-destroyed map (also affects the --version exit).
    if (gPerfMonitorInterface == this)
        gPerfMonitorInterface = nullptr;
}

void PerformanceMonitor::Initialize()
{
	// in seconds
	uint32 IntervalReportValue = sWorld.getConfig(CONFIG_UINT32_PERFORMANCE_REPORT_INTERVAL);

	// convert to milliseconds
	IntervalReport.SetInterval(IntervalReportValue * 1000);
	IntervalReport.SetCurrent(0);
}

void PerformanceMonitor::FrameStart()
{
	if (!g_bEnableStatGather)
	{
		return;
	}
	Tick.FrameStart();
	WorldSleep.FrameStart();
	WorldTick.FrameStart();
	TEST0.FrameStart();
	UpdateSession.FrameStart();
	MapManager.FrameStart();

	const MapManager::MapMapType& Maps = sMapMgr.Maps();
	for (MapManager::MapMapType::const_iterator iter = Maps.cbegin(); iter != Maps.cend(); iter++)
	{
		iter->second->MovementPerfTimer.FrameStart();
		iter->second->SpellPerfTimer.FrameStart();
		iter->second->UpdateTimer.FrameStart();
	}
}

void PerformanceMonitor::FrameEnd(uint32 delta)
{
	if (!g_bEnableStatGather)
	{
		return;
	}
	WorldSleep.FrameEnd();
	Tick.FrameEnd();
	WorldTick.FrameEnd();
	TEST0.FrameEnd();
	UpdateSession.FrameEnd();
	MapManager.FrameEnd();

	const MapManager::MapMapType& Maps = sMapMgr.Maps();
	for (MapManager::MapMapType::const_iterator iter = Maps.cbegin(); iter != Maps.cend(); iter++)
	{
		Map* CurrentMap = iter->second;
		CurrentMap->MovementPerfTimer.FrameEnd();
		CurrentMap->SpellPerfTimer.FrameEnd();
		CurrentMap->UpdateTimer.FrameEnd();
	}

	QPC_Counter = CPU::qpc_counter;
	CPU::qpc_counter = 0;

	IntervalReport.Update(delta);
	if (IntervalReport.Passed())
	{
		IntervalReport.Reset();
		ReportPerformanceToDB();
	}
}

void PerformanceMonitor::SetReportInterval(uint32 IntervalInSeconds)
{
	IntervalReport.SetInterval(IntervalInSeconds * 1000);
}

void PerformanceMonitor::ReportCPU(ChatHandler& Handler)
{
	/// #### CPU ####
	Handler.PSendSysMessage("CPU Performance report");
	Handler.PSendSysMessage("QPC counter: %u", QPC_Counter);

	auto GetPercentOfLambda = [](XStatTimer Local, XStatTimer Global) -> float
		{
			return 100.0f * float(Local.result) / float(Global.result);
		};

	auto ReportStatWithParentLambda = [&Handler, &GetPercentOfLambda](const char* Name, const XStatTimer& Timer, const XStatTimer& Parent, int32 Level = 1)
		{
			char BeginShit[256];
			if (Level == 0)
			{
				memset(BeginShit, 0, sizeof(BeginShit));
			}
			else
			{
				int32 Cursor = 0;
				for (int32 i = 0; i < Level; i++)
				{
					BeginShit[Cursor++] = '-';
					BeginShit[Cursor++] = '>';
					BeginShit[Cursor++] = ' ';
				}
				BeginShit[Cursor] = 0;
			}
			Handler.PSendSysMessage(" %s %s: [%u] %2.2fms, %2.1f%%, min: %2.2fms, max: %2.2fms", 
				BeginShit, Name, Timer.count, Timer.result, GetPercentOfLambda(Timer, Parent), Timer.MinResult, Timer.MaxResult);
		};

	Handler.PSendSysMessage("Tick: %2.2f", Tick.result);
	Handler.PSendSysMessage("WorldSleep: %2.2f", WorldSleep.result);

	ReportStatWithParentLambda("TEST0", TEST0, Tick);
	ReportStatWithParentLambda("WorldTick", WorldTick, Tick);
	ReportStatWithParentLambda("UpdateSession", UpdateSession, WorldTick, 2);
	ReportStatWithParentLambda("MapManager", MapManager, WorldTick, 2);

	/// #### Maps ####
	Handler.PSendSysMessage("Map performance report. Some maps update async");

	const MapManager::MapMapType& Maps = sMapMgr.Maps();
	for (MapManager::MapMapType::const_iterator iter = Maps.cbegin(); iter != Maps.cend(); iter++)
	{
		Map* CurrentMap = iter->second;
		Handler.PSendSysMessage("Map: %u, InstanceID: %u", CurrentMap->GetId(), CurrentMap->GetInstanceId());
		ReportStatWithParentLambda("Update", CurrentMap->UpdateTimer, MapManager);
		ReportStatWithParentLambda("Movement", CurrentMap->MovementPerfTimer, CurrentMap->UpdateTimer, 2);
		ReportStatWithParentLambda("Spell", CurrentMap->SpellPerfTimer, CurrentMap->UpdateTimer, 2);
	}
}

void PerformanceMonitor::ReportMemory(ChatHandler& Handler)
{
	Handler.PSendSysMessage("Memory Performance report");
	int64 ProcessMemory = (int64)Memory::GetProcessMemory(); // fix later
	int64 AllTrackedBytes = 0;
	std::lock_guard guard{ MemBytesGuard };

	std::unordered_map<std::string, int64> FixedMap;

	for (auto& [key, value] : MemBytes)
	{
		FixedMap[key] += value;
		AllTrackedBytes += value;
	}

	auto GetValueAsMbLambda = [](int64 InValue) -> double
		{
			double fConvertValue = InValue;
			fConvertValue /= 1024.0; // to kbytes
			fConvertValue /= 1024.0; // to mbytes
			return fConvertValue;
		};

	double fProcessMemory = GetValueAsMbLambda(ProcessMemory);
	double fTrackedMemory = GetValueAsMbLambda(AllTrackedBytes);
	Handler.PSendSysMessage("All process memory: %.2fMb (Tracked: %.2fMb)", fProcessMemory, fTrackedMemory);

	for (auto& [key, value] : FixedMap)
	{
		Handler.PSendSysMessage("-> %s - %.2fMb", key.c_str(), GetValueAsMbLambda(value));
	}
}

void PerformanceMonitor::ReportPerformanceToDB()
{

}

void PerformanceMonitor::ReportAlloc(const char* Category, size_t Bytes)
{
	std::lock_guard guard{ MemBytesGuard };
	MemBytes[Category] += Bytes;
}

void PerformanceMonitor::ReportDealloc(const char* Category, size_t Bytes)
{
	std::lock_guard guard{ MemBytesGuard };
	MemBytes[Category] -= Bytes;
}
