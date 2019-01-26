#include "PathTracker.h"
#include "Geometry/Helpers.h"

std::map<uint32_t, EnemyData> g_EnemyInfo;

PathTracker::PathTracker()
{
}


PathTracker::~PathTracker()
{
}

uint32_t PathTracker::GetMovImmobileTime(AIBaseClient *hero)
{
	return g_EnemyInfo[hero->GetNetworkID()].is_stopped_ ? GetTickCount() - g_EnemyInfo[hero->GetNetworkID()].stop_tick_ : 0;
}

uint32_t PathTracker::GetLastMovChangeTime(AIBaseClient *hero)
{
	return GetTickCount() - g_EnemyInfo[hero->GetNetworkID()].last_waypoint_tick_;
}

float PathTracker::GetAvgMovChangeTime(AIBaseClient *hero)
{
	return g_EnemyInfo[hero->GetNetworkID()].avg_tick_ + 1;
}

float PathTracker::GetAvgPathLenght(AIBaseClient *hero)
{
	return g_EnemyInfo[hero->GetNetworkID()].avg_path_lenght_;
}

float PathTracker::GetLastAngleDiff(AIBaseClient *hero)
{
	return g_EnemyInfo[hero->GetNetworkID()].last_angle_diff_;
}

float PathTracker::GetAvgOrbwalkTime(AIBaseClient *hero)
{
	return g_EnemyInfo[hero->GetNetworkID()].avg_orbwalk_time_;
}

void PathTracker::OnNewPath(void* AI, bool Move, bool Stop, void* Userdata)
{
	auto object = pSDK->EntityManager->GetObjectFromPTR(AI);
	
	if (!SdkIsObjectAI(AI))
		return;

	if (object->GetTeamID() == Player.GetTeamID() || !object->IsHero())
		return;

	auto Hero = object->AsAIHeroClient();

	if (g_EnemyInfo.find(Hero->NetworkID) == g_EnemyInfo.end())
		return;

	auto& enemy = g_EnemyInfo[Hero->NetworkID];

	if (Hero->NavInfo().NumberOfWaypoints < 2)
	{
		if (!enemy.is_stopped_)
		{
			enemy.stop_tick_ = GetTickCount();
			enemy.last_waypoint_tick_ = GetTickCount();
			enemy.is_stopped_ = true;
			enemy.count_ = 0;
			enemy.avg_tick_ = 0;
			enemy.avg_path_lenght_ = 0;
			enemy.last_angle_diff_ = 360;
		}
	}
	else
	{
		std::vector<Vector2> wp;
		auto pos = Player.GetServerPosition();
		wp.emplace_back(pos.x, pos.y);
		for (auto i = 0; i <= Hero->NavInfo().NumberOfWaypoints; ++i)
		{
			wp.emplace_back(Hero->NavInfo().Waypoints[i].x, Hero->NavInfo().Waypoints[i].z);
		}

		if (!enemy.is_stopped_)
		{
			enemy.avg_tick_ = (enemy.count_ * enemy.avg_tick_ + (GetTickCount() - enemy.last_waypoint_tick_)) / ++enemy.count_;
			enemy.avg_path_lenght_ = ((enemy.count_ - 1) * enemy.avg_path_lenght_ + Geometry::Helpers::PathLenght(wp)) / enemy.count_;
		}
		enemy.last_waypoint_tick_ = GetTickCount();
		enemy.is_stopped_ = false;
		enemy.last_waypoint_ = std::move(wp);

		if (enemy.is_windup_checked_)
		{
			if (GetTickCount() - enemy.last_aa_tick_ < 300)
			{
				enemy.avg_orbwalk_time_ = (enemy.avg_orbwalk_time_ * enemy.orbwalk_count_ + (enemy.last_waypoint_tick_ - enemy.last_windup_tick_)) / ++enemy.orbwalk_count_;
				enemy.is_windup_checked_ = true;
			}
		}
	}
}

void PathTracker::OnAICastAttack(void* AI, void* TargetObject, bool StartAttack, bool StopAttack, void* UserData)
{
	auto sender = pSDK->EntityManager->GetObjectFromPTR(AI);

	if (sender->GetTeamID() != Player.GetTeamID() && sender->IsHero())
	{
		auto& enemy = g_EnemyInfo[sender->GetNetworkID()];
		enemy.last_windup_tick_ = GetTickCount();
		enemy.is_windup_checked_ = false;
	}
}

void PathTracker::OnProcessSpell(void* AI, PSDK_SPELL_CAST SpellCast, void* UserData)
{
	auto sender = pSDK->EntityManager->GetObjectFromPTR(AI);

	if (sender->GetTeamID() != Player.GetTeamID() && sender->IsHero() && SpellCast->IsAutoAttack)
	{
		auto& enemy = g_EnemyInfo[sender->GetNetworkID()];
		enemy.last_aa_tick_ = GetTickCount();
	}
}
