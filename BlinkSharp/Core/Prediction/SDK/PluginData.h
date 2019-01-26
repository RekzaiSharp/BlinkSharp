#ifndef PluginData_h__
#define PluginData_h__

#include "../../../Include/SDK Extensions.h"
#include <Windows.h>
#include <vector>
#include <cstdint>
#include <functional>

enum ePredictionChance
{
	kHitChanceCollision,
	kHitChanceOutOfRange,
	kHitChanceImpossible,
	kHitChanceLow,
	kHitChanceMedium,
	kHitChanceHigh,
	kHitChanceVeryHigh,
	kHitChanceDashing,
	kHitChanceImmobile
};

enum eCollisionFlags
{
	kCollidesWithNothing = 0,
	kCollidesWithMinions = (1 << 1),	// Spell collides with enemy minions
	kCollidesWithHeroes = (1 << 2),		// Spell collides with enemy champions
	kCollidesWithYasuoWall = (1 << 3),	// Spell collides with Yasuo wall
	kCollidesWithWalls = (1 << 4),		// Spell collides with walls
	kCollidesWithAllies = (1 << 5)		// Spell collides with ally champions
};

enum eSpellType
{
	kTargetCast,
	kLineCast,
	kCircleCast,
	kConeCast
};

enum eBuffType
{
	BUFF_Internal,
	BUFF_Aura,
	BUFF_CombatEnchancer,
	BUFF_CombatDehancer,
	BUFF_SpellShield,
	BUFF_Stun,
	BUFF_Invisibility,
	BUFF_Silence,
	BUFF_Taunt,
	BUFF_Polymorph,
	BUFF_Slow,
	BUFF_Snare,
	BUFF_Damage,
	BUFF_Heal,
	BUFF_Haste,
	BUFF_SpellImmunity,
	BUFF_PhysicalImmunity,
	BUFF_Invulnerability,
	BUFF_Sleep,
	BUFF_NearSight,
	BUFF_Frenzy,
	BUFF_Fear,
	BUFF_Charm,
	BUFF_Poison,
	BUFF_Suppression,
	BUFF_Blind,
	BUFF_Counter,
	BUFF_Shred,
	BUFF_Flee,
	BUFF_Knockup,
	BUFF_Knockback,
	BUFF_Disarm
};

enum eInterruptibleDanger
{
	kLowDanger,
	kMediumDanger,
	kHighDanger
};

struct OnTeleportArgs
{
	AIBaseClient* Source;	// Object that is teleporting
	int Type;		// The type of teleport (eTeleportType)
	int Status;		// The status of teleport (eTeleportStatus)
	int Duration;	// The duration of the teleport in ms
};

struct AdvPredictionInput
{
	Vector3 FromPosition;				// Start position for casting
	Vector3 RangeCheckFromPosition;	// Start position for prediction range checks
	bool IsAoE;						// True for area of effect spells
	bool AddBoundingRadius;			// Adds target bounding radius to prediction
	int CollisionFlags;				// (Example kCollidesWithMinions|kCollidesWithYasuoWall)
	float Delay;					// Delay in seconds for cast (e.g WindupTime)
	float Radius;					// Radius of the spell
	float Range;					// Range of the spell
	float Speed;					// Speed of the spell
	int Type;						// Type of the spell (e.g kLineCast)
	AIBaseClient* Target;					// Unit to run prediction for
};

struct AdvPredictionOutput
{
	int HitChance;						// Odds of hitting target (e.g kHitChanceHigh)
	Vector3 CastPosition;					// Predicted position of where you should cast
	Vector3 TargetPosition;				// Predicted position of unit when spell will land
	std::vector<AIBaseClient*> AoETargetsHit;	// Vector of all targets hit when using AoE prediction
};

struct PredictionOutput
{
	Vector3					CastPosition;
	std::vector<AIBaseClient*>		EnemiesHit;
	ePredictionChance		HitChance;
};

struct UnitDash
{
	AIBaseClient*				Source;
	Vector3				StartPosition;
	Vector3				EndPosition;
	int					StartTick;
	int					EndTick;
	int					Duration;
	float				Speed;
};

struct GapCloserSpell
{
	AIBaseClient* Sender;
	Vector3 StartPosition;
	Vector3 EndPosition;
	bool IsTargeted;
	int StartTick;
	int Slot;
	void* Data;
};

#endif // PluginData_h__