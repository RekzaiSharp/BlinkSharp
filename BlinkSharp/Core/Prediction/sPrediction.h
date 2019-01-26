#pragma once
#include "SDK/PluginData.h"
#include "PathTracker.h"
#include "Collision.h"
#include "Geometry/Helpers.h"
#include "ISPrediction.h"

struct PredictionInput;
struct PredictionResult;

class SPrediction // hier hat der vorher geerbt
{
	friend class LinePrediction;
public:
	SPrediction();
	~SPrediction();
	bool CastSpell(SDK_SPELL *spell, AIBaseClient *target, ePredictionChance hit_chance = kHitChanceHigh, bool aoe = false, uint8_t min_hit = 1, eSpellType SpellType = eSpellType::kTargetCast);
	bool CastArcSpell(SDK_SPELL *spell, AIBaseClient *target, ePredictionChance hit_chance = kHitChanceHigh, bool aoe = false, uint8_t min_hit = 1);
	bool CastRingSpell(SDK_SPELL *spell, AIBaseClient *target, float ring_radius, ePredictionChance hit_chance = kHitChanceHigh);
	bool CastVectorSpell(SDK_SPELL *spell, AIBaseClient *target, ePredictionChance hit_chance = kHitChanceHigh, bool aoe = false, uint8_t min_hit = 1);
	PredictionResult GetPrediction(PredictionInput input);
	PredictionResult GetPrediction(SDK_SPELL* spell, AIBaseClient *target);
	Vector2 GetFastUnitPosition(AIBaseClient* target, float delay, float missile_speed = 0, float distance_set = 0, float move_speed = 0);
	Vector2 GetFastUnitPosition(AIBaseClient* target, const Vector3& from, float delay, float missile_speed = 0, float distance_set = 0, float move_speed = 0);
	Collision& GetCollisionEngine(); // hier war vorher virtual
	PathTracker& GetPathTrackerEngine();
	bool RunPrediction(AdvPredictionInput* Input, AdvPredictionOutput* Output, uint32_t PredictionVersion = 1);
	float GetArrivalTime(float distance, float delay, float missile_speed = 0);
	ePredictionChance GetHitChance(float t, float avgt, float movt, float avgp, float anglediff);
protected:
	PredictionResult GetImmobilePrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from);
	PredictionResult GetImmobilePrediction(PredictionInput input);
	PredictionResult GetDashingPrediction(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from);
	PredictionResult GetDashingPrediction(PredictionInput input);
	PredictionResult WaypointAnalysis(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, float avgt, float movt, float avgp, float anglediff, const Vector3& from, float move_speed = 0.0f);
	PredictionResult WaypointAnalysis(PredictionInput input, float move_speed = 0.0f);
private:
	PathTracker m_path_tracker;
	Collision m_collision;
};

extern std::unique_ptr<SPrediction> sPred;

#pragma region PredictionInput struct
struct PredictionInput
{
	AIBaseClient *target_;
	struct
	{
		float delay_;
		float missile_speed_;
		float width_;
		float ring_radius_;		//only set for ring spells
		float range_;
		bool collisionable_;
		eSpellType type_;
	} spell_;
	std::vector<Vector3> path_;
	float avg_reaction_time_;
	float last_mov_change_time_;
	float avg_path_lenght_;
	float last_angle_diff_;
	Vector3 from_;
	Vector3 range_check_from_;

	PredictionInput(AIBaseClient *target, SDK_SPELL *spell)
	{
		/*target_ = target;
		spell_.delay_ = spell->CastDelay();
		spell_.missile_speed_ = spell->MissileSpeed();
		spell_.width_ = spell->PrimaryCastRadius();
		spell_.range_ = spell->CastRange();
		spell_.collisionable_ = (spell->AffectsFlags() & static_cast<int>(kCollidesWithMinions)) == kCollidesWithMinions;
		spell_.type_ = spell->CastType();
		path_ = target->NavInfo().Waypoints;
		if (target)
		{
			avg_reaction_time_ = dynamic_cast<SPrediction *>(g_Plugin)->GetPathTrackerEngine().GetAvgMovChangeTime(target);
			last_mov_change_time_ = dynamic_cast<SPrediction *>(g_Plugin)->GetPathTrackerEngine().GetLastMovChangeTime(target);
			avg_path_lenght_ = dynamic_cast<SPrediction *>(g_Plugin)->GetPathTrackerEngine().GetAvgPathLenght(target);
			last_angle_diff_ = dynamic_cast<SPrediction *>(g_Plugin)->GetPathTrackerEngine().GetLastAngleDiff(target);
		}
		from_ = Player.GetServerPosition();
		spell_.ring_radius_ = 0;*/
	}

	PredictionInput(AIBaseClient *target, SDK_SPELL *spell, const Vector3& from)
		: PredictionInput::PredictionInput(target, spell)
	{
		from_ = from;
	}

	PredictionInput(AIBaseClient *target, float delay, float speed, float radius, float range, bool collision, eSpellType type, const Vector3& from, const Vector3& range_check_from)
	{
		target_ = target;
		spell_.delay_ = delay;
		spell_.missile_speed_ = speed;
		spell_.width_ = radius;
		spell_.range_ = range;
		spell_.collisionable_ = collision;
		spell_.type_ = type;
	//	path_ = target->GetWaypointList();
		if (target->IsHero())
		{
			/*avg_reaction_time_ = dynamic_cast<SPrediction *>(g_Plugin)->GetPathTrackerEngine().GetAvgMovChangeTime(target);
			last_mov_change_time_ = dynamic_cast<SPrediction *>(g_Plugin)->GetPathTrackerEngine().GetLastMovChangeTime(target);
			avg_path_lenght_ = dynamic_cast<SPrediction *>(g_Plugin)->GetPathTrackerEngine().GetAvgPathLenght(target);
			last_angle_diff_ = dynamic_cast<SPrediction *>(g_Plugin)->GetPathTrackerEngine().GetLastAngleDiff(target);*/
		}
		from_ = from;
		range_check_from_ = range_check_from;
		spell_.ring_radius_ = 0;
	}

	PredictionInput(const PredictionInput& other)
	{
		target_ = other.target_;
		spell_ = other.spell_;
		path_ = other.path_;
		avg_reaction_time_ = other.avg_reaction_time_;
		last_mov_change_time_ = other.last_mov_change_time_;
		avg_path_lenght_ = other.avg_path_lenght_;
		last_angle_diff_ = other.last_angle_diff_;
		from_ = other.from_;
		range_check_from_ = other.range_check_from_;
	}
};
#pragma endregion

#pragma region PredictionResult struct
struct PredictionResult
{
	PredictionInput input_;
	AIBaseClient *unit_;
	Vector2 cast_position_;
	Vector2 cast_position_source_;		//only set for vector spells
	Vector2 unit_position_;
	ePredictionChance hit_chance_;
	CollisionResult collision_;

	PredictionResult(PredictionInput input, AIBaseClient *unit, Vector2 castpos, Vector2 unitpos, ePredictionChance hc, CollisionResult col)
		: input_(input)
	{
		unit_ = unit;
		cast_position_ = castpos;
		unit_position_ = unitpos;
		hit_chance_ = hc;
		collision_ = col;
	}

	void Lock(bool check_dodge = true)
	{
		//collision_ = static_cast<SPrediction *>(g_Plugin)->GetCollisionEngine().GetCollisions(Geometry::Helpers::To2D(input_.from_),
			//cast_position_, input_.spell_.range_, input_.spell_.width_, input_.spell_.delay_, input_.spell_.missile_speed_);

		CheckCollisions();
		//CheckOutofRange(check_dodge);
	}

	void CheckCollisions()
	{
		if (input_.spell_.collisionable_ && ((collision_.objects_ & sCollisionFlags::Minions) == sCollisionFlags::Minions || (collision_.objects_ & sCollisionFlags::YasuoWall) == sCollisionFlags::YasuoWall))
		{
			hit_chance_ = kHitChanceCollision;
		}
	}

	void CheckOutofRange(bool check_dodge)
	{
		if (Geometry::Helpers::Distance(Geometry::Helpers::To2D(input_.range_check_from_), cast_position_) > input_.spell_.range_
			- (check_dodge ?
				sPred->GetArrivalTime(Geometry::Helpers::Distance(Geometry::Helpers::To2D(input_.from_), cast_position_), input_.spell_.delay_, input_.spell_.missile_speed_)
				* unit_->GetMovementSpeed() * (100 - 1 / 100.0f) : 0))
		{
			hit_chance_ = kHitChanceOutOfRange;
		}
	}
};
#pragma endregion

#pragma region PredictionAoeResult
struct PredictionAoeResult
{
	Vector2 cast_position_;
	Vector2 cast_position_source_;		//only set for vector spells
	CollisionResult collision_;
	uint8_t hit_count_;

	PredictionAoeResult(const Vector2& castpos, CollisionResult col, int cnt)
	{
		cast_position_ = castpos;
		collision_ = col;
		hit_count_ = cnt;
	}
};
#pragma endregion
