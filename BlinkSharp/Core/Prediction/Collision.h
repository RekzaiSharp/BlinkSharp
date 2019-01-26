#pragma once
#include "Geometry\ClipperWrapper.hpp"
#include "../../Include/SDK Extensions.h"

enum class sCollisionFlags
{
	None = 0,
	Minions = 1,
	AllyChampions = 2,
	EnemyChampions = 4,
	Wall = 8,
	YasuoWall = 16,
};

struct CollisionResult
{
	std::vector<AIBaseClient*> units_;
	sCollisionFlags objects_;

	CollisionResult()
	{
		objects_ = sCollisionFlags::None;
	}

	CollisionResult(std::vector<AIBaseClient*>&& units, sCollisionFlags flags)
		: units_(std::move(units)), objects_(flags)
	{

	}
};

inline sCollisionFlags operator|(const sCollisionFlags& left, const sCollisionFlags& right)
{
	return static_cast<sCollisionFlags>(static_cast<uint32_t>(left) | static_cast<uint32_t>(right));
}

inline sCollisionFlags operator&(const sCollisionFlags& left, const sCollisionFlags& right)
{
	return static_cast<sCollisionFlags>(static_cast<uint32_t>(left) & static_cast<uint32_t>(right));
}

inline sCollisionFlags operator|=(sCollisionFlags& left, const sCollisionFlags& right)
{
	return left = left | right;
}

inline sCollisionFlags operator&=(sCollisionFlags& left, const sCollisionFlags& right)
{
	return left = left & right;
}

class Collision
{
public:
	Collision();
	~Collision();
	bool CheckCollision(const Vector2& from, const Vector2& to, float width, float delay, float missile_speed, bool check_minion = true, bool check_enemy = false, bool check_yasuowall = true, bool check_ally = false, bool check_wall = false, bool is_arc = false);
	bool CheckMinionCollision(const Vector2& from, const Vector2& to, float width, float delay, float missile_speed = 0, bool is_arc = false);
	bool CheckEnemyHeroCollision(const Vector2& from, const Vector2& to, float width, float delay, float missile_speed = 0, bool is_arc = false);
	bool CheckAllyHeroCollision(const Vector2& from, const Vector2& to, float width, float delay, float missile_speed = 0, bool is_arc = false);
	bool CheckWallCollision(const Vector2& from, const Vector2& to);
	bool CheckYasuoWallCollision(const Vector2& from, const Vector2& to, float width, bool is_arc = false);
	CollisionResult GetCollisions(const Vector2& from, const Vector2& to, float range, float width, float delay, float missile_speed = 0, bool is_arc = false);
	static void __cdecl OnProcessSpell(void* AI, PSDK_SPELL_CAST SpellCast, void* UserData);
	static void __cdecl OnCreateObject(void* Object, unsigned int NetworkID, void* UserData);
	static uint32_t s_YasuoWallCastedTick;
	static uint8_t s_YasuoWallLvl;
	static Vector2 s_YasuoWallCastedPos;
	static Vector2 s_YasuoWallPos;
};

