#pragma once

#include "GameObjects.hpp"
#include "Game.hpp"

#pragma region Enums

enum class CollisionFlags {
	Nothing		= (0 << 0),
	Minions		= (1 << 0),
	Heroes		= (1 << 1),
	Wall		= (1 << 2),
	BraumWall	= (1 << 3),
	YasuoWall	= (1 << 4),
	Default		= ((1 << 0) | (1 << 4))
};

inline CollisionFlags operator |(CollisionFlags lhs, CollisionFlags rhs) {
	return static_cast<CollisionFlags> (
		static_cast<std::underlying_type<CollisionFlags>::type>(lhs) |
		static_cast<std::underlying_type<CollisionFlags>::type>(rhs)
		);
}

inline CollisionFlags operator &(CollisionFlags lhs, CollisionFlags rhs)
{
	return static_cast<CollisionFlags> (
		static_cast<std::underlying_type<CollisionFlags>::type>(lhs) &
		static_cast<std::underlying_type<CollisionFlags>::type>(rhs)
		);
}

enum class SkillType {
	Line,
	Circle,
	Cone,
	Arc,
	None
};
enum class SkillshotType {
	Line,
	Circle,
	Cone
};
enum class HitChance {
	Impossible = -1,
	Collision,
	OutOfRange,
	Low,
	Medium,
	High,
	VeryHigh,
	Dashing,
	Immobile
};
#pragma endregion

class ICollision {
public:
	struct Output {
		std::vector<AIBaseClient*>	 Units;
		CollisionFlags				 Objects;
	};

	virtual bool CheckCollisions(Vector2& from, Vector2& to, float width, float delay, float missileSpeed = HUGE_VALF, bool isArc = false, CollisionFlags CollisionFlags = CollisionFlags::Default) = 0;
	
	virtual bool CheckWallCollision     (Vector2& from, Vector2& to) = 0;
	virtual bool CheckHeroCollision     (Vector2& from, Vector2& to, float width, float delay, float missileSpeed = HUGE_VALF, bool isArc = false) = 0;
	virtual bool CheckMinionCollision   (Vector2& from, Vector2& to, float width, float delay, float missileSpeed = HUGE_VALF, bool isArc = false) = 0;
	virtual bool CheckYasuoWallCollision(Vector2& from, Vector2& to, float width, bool isArc = false) = 0;

	virtual std::shared_ptr<ICollision::Output> GetCollisions(Vector2& from, Vector2& to, float range, float width, float delay, float missileSpeed = HUGE_VALF, bool isArc = false, CollisionFlags CollisionFlags = CollisionFlags::Default) = 0;
};

class IPrediction {
public:
	struct Input {		
		float Delay;
		float Width;
		float Range;
		float MissileSpeed;		
		bool  Collision;
		CollisionFlags Flags;
		Vector3 From;
		Vector3 RangeCheckFrom;
		AIHeroClient* Target;
		SkillshotType SkillShotType;	

		virtual void NewInput(
			SkillshotType _Type, 
			float _Range, 
			float _Delay = 0.25f, 
			float _MissileSpeed = HUGE_VALF,
			float _Width = 0.0f, 
			bool _Collision = false, 
			CollisionFlags _Flags = CollisionFlags::Nothing, 
			Vector3 _From = Vector3()
		) = 0;
	};

	struct Output {
		AIBaseClient*	  Unit;
		Vector2			  CastPosition;
		Vector2			  UnitPosition;
		HitChance         Hitchance;
		std::shared_ptr<IPrediction::Input> Input;
		std::shared_ptr<ICollision::Output> CollisionResult;
	};

	virtual Vector2 GetFastPrediction(AIBaseClient * target, float delay, float missileSpeed = HUGE_VALF, Vector2 * from = NULL, float distanceSet = 0) = 0;

	virtual std::shared_ptr<IPrediction::Output> GetPrediction(std::shared_ptr <IPrediction::Input> Data) = 0;
	virtual std::shared_ptr<IPrediction::Output> GetPrediction(AIHeroClient* target, float width, float delay, float missileSpeed, float range, bool collisionable, SkillshotType type) = 0;
};

