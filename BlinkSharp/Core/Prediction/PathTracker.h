#pragma once
#include <vector>
#include <map>
#include "SDK/PluginData.h"


struct EnemyData
{
	bool is_stopped_;
	std::vector<Vector2> last_waypoint_;
	uint32_t last_waypoint_tick_;
	uint32_t stop_tick_;
	float avg_tick_;
	float avg_path_lenght_;
	uint32_t count_;
	uint32_t last_aa_tick_;
	uint32_t last_windup_tick_;
	bool is_windup_checked_;
	uint32_t orbwalk_count_;
	float avg_orbwalk_time_;
	float last_angle_diff_;
	UnitDash last_dash_info_;

	EnemyData()
		: is_stopped_(false)
		, last_waypoint_tick_(0)
		, stop_tick_(0)
		, avg_tick_(0.0f)
		, avg_path_lenght_(0.0f)
		, count_(0)
		, last_aa_tick_(0)
		, last_windup_tick_(0)
		, is_windup_checked_(false)
		, orbwalk_count_(0)
		, avg_orbwalk_time_(0.0f)
		, last_angle_diff_(0.0f)
	{

	}

	EnemyData(const std::vector<Vector2>& wp)
		: last_waypoint_(wp)
		, is_stopped_(false)
		, last_waypoint_tick_(0)
		, stop_tick_(0)
		, avg_tick_(0.0f)
		, avg_path_lenght_(0.0f)
		, count_(0)
		, last_aa_tick_(0)
		, last_windup_tick_(0)
		, is_windup_checked_(false)
		, orbwalk_count_(0)
		, avg_orbwalk_time_(0.0f)
		, last_angle_diff_(0.0f)
	{

	}
};

extern std::map<uint32_t, EnemyData> g_EnemyInfo;

class PathTracker
{
public:
	PathTracker();
	~PathTracker();
	uint32_t GetMovImmobileTime(AIBaseClient *hero);
	uint32_t GetLastMovChangeTime(AIBaseClient *hero);
	float GetAvgMovChangeTime(AIBaseClient *hero);
	float GetAvgPathLenght(AIBaseClient *hero);
	float GetLastAngleDiff(AIBaseClient *hero);
	float GetAvgOrbwalkTime(AIBaseClient *hero);
	static void __cdecl OnNewPath(void* AI, bool Move, bool Stop, void* Userdata);
	static void __cdecl OnAICastAttack(void* AI, void* TargetObject, bool StartAttack, bool StopAttack, void* UserData);
	static void __cdecl OnProcessSpell(void* AI, PSDK_SPELL_CAST SpellCast, void* UserData);
	//void OnDash(UnitDash *Args);
};