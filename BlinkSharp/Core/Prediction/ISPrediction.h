#pragma once
#include "SDK/PluginData.h"

struct CollisionResult;
struct PredictionInput;
struct PredictionResult;

class ICollision2
{
public:
	virtual bool CheckCollision(const Vector2& from, const Vector2& to, float width, float delay, float missile_speed, bool check_minion = true, bool check_enemy = false, bool check_yasuowall = true, bool check_ally = false, bool check_wall = false, bool is_arc = false) = 0;
	virtual bool CheckMinionCollision(const Vector2& from, const Vector2& to, float width, float delay, float missile_speed = 0, bool is_arc = false) = 0;
	virtual bool CheckEnemyHeroCollision(const Vector2& from, const Vector2& to, float width, float delay, float missile_speed = 0, bool is_arc = false) = 0;
	virtual bool CheckAllyHeroCollision(const Vector2& from, const Vector2& to, float width, float delay, float missile_speed = 0, bool is_arc = false) = 0;
	virtual bool CheckWallCollision(const Vector2& from, const Vector2& to) = 0;
	virtual bool CheckYasuoWallCollision(const Vector2& from, const Vector2& to, float width, bool is_arc = false) = 0;
	virtual CollisionResult GetCollisions(const Vector2& from, const Vector2& to, float range, float width, float delay, float missile_speed = 0, bool is_arc = false) = 0;
};

class IPathTracker
{
public:
	virtual uint32_t GetMovImmobileTime(AIBaseClient *hero) = 0;
	virtual uint32_t GetLastMovChangeTime(AIBaseClient *hero) = 0;
	virtual float GetAvgMovChangeTime(AIBaseClient *hero) = 0;
	virtual float GetAvgPathLenght(AIBaseClient *hero) = 0;
	virtual float GetLastAngleDiff(AIBaseClient *hero) = 0;
	virtual float GetAvgOrbwalkTime(AIBaseClient *hero) = 0;
};

class ISPrediction
{
public:
	virtual bool CastSpell(SDK_SPELL *spell, AIBaseClient *target, ePredictionChance hit_chance = kHitChanceHigh, bool aoe = false, uint8_t min_hit = 1) = 0;
	virtual bool CastArcSpell(SDK_SPELL *spell, AIBaseClient *target, ePredictionChance hit_chance = kHitChanceHigh, bool aoe = false, uint8_t min_hit = 1) = 0;
	virtual bool CastRingSpell(SDK_SPELL *spell, AIBaseClient *target, float ring_radius, ePredictionChance hit_chance = kHitChanceHigh) = 0;
	virtual bool CastVectorSpell(SDK_SPELL *spell, AIBaseClient *target, ePredictionChance hit_chance = kHitChanceHigh, bool aoe = false, uint8_t min_hit = 1) = 0;
	virtual PredictionResult GetPrediction(PredictionInput input) = 0;
	virtual PredictionResult GetPrediction(SDK_SPELL *spell, AIBaseClient *target) = 0;
	virtual Vector2 GetFastUnitPosition(AIBaseClient *target, float delay, float missile_speed = 0, float distance_set = 0, float move_speed = 0) = 0;
	virtual Vector2 GetFastUnitPosition(AIBaseClient *target, const Vector3& from, float delay, float missile_speed = 0, float distance_set = 0, float move_speed = 0) = 0;
	virtual ICollision& GetCollisionEngine() = 0;
	virtual IPathTracker& GetPathTrackerEngine() = 0;
};