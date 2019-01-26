#include <vector>
#include <iterator>
#include "SPrediction.h"
#include "LinePrediction.h"
#include "CirclePrediction.h"
#include "ConePrediction.h"
#include "ArcPrediction.h"
#include "RingPrediction.h"
#include "VectorPrediction.h"
#include "Utility.h"
#include "Geometry\Helpers.h"
#include "ISPrediction.h"
#include "ISPredictionBase.h"

//SPrediction::SPrediction()
//	: SPlugin::SPlugin(GPluginSDK->AddMenu("SPrediction"))
//{
//	m_Menu.AddCheckBox("Check for target AA Windup", "sprediction.windup", false);
//	m_Menu.AddFloat("Max Range Dodge Ignore (%)", "sprediction.ignoredodge", 0.0f, 100.0f, 50.0f);
//	m_Menu.AddInteger("Ignore Reaction Delay", "sprediction.ignorereaction", 0, 200, 0);
//	m_Menu.AddInteger("Spell Delay", "sprediction.spelldelay", 0, 0, 200);
//
//	auto& _drawings = m_Menu.AddSubMenu("Drawings", "sprediction.drawings");
//	_drawings.AddInteger("Drawing Pos X", "sprediction.drawings.posx", 0, GRender->ScreenSize().x, GRender->ScreenSize().x - 200);
//	_drawings.AddInteger("Drawing Pos Y", "sprediction.drawings.posy", 0, GRender->ScreenSize().y, 0);
//}

bool SPrediction::RunPrediction(AdvPredictionInput* Input, AdvPredictionOutput* Output, uint32_t PredictionVersion)
{
	PredictionInput inp(Input->Target, Input->Delay, Input->Speed, Input->Radius, Input->Range, (Input->CollisionFlags & static_cast<int>(kCollidesWithMinions)) == kCollidesWithMinions, static_cast<eSpellType>(Input->Type), Input->FromPosition, Input->RangeCheckFromPosition);
	PredictionResult result = GetPrediction(inp);
	Output->CastPosition = result.cast_position_.To3D();
	if ((Output->CastPosition - Input->FromPosition).Length() > Input->Range)
	{
		Output->HitChance = kHitChanceOutOfRange;
	}
	else
	{
		Output->HitChance = result.hit_chance_ == kHitChanceHigh ? kHitChanceVeryHigh : result.hit_chance_;
	}
	Output->TargetPosition = result.unit_position_.To3D();
	return true;
}

//	static void __cdecl OnProcessSpell(void* AI, PSDK_SPELL_CAST SpellCast, void* UserData);
bool SPrediction::CastSpell(SDK_SPELL* spell, AIBaseClient* target, ePredictionChance hit_chance, bool aoe, uint8_t min_hit, eSpellType SpellType)
{
	PredictionInput input(target, spell);
	switch (SpellType)
	{
	case kLineCast:
	{
		if (!aoe)
		{
			PredictionResult result = LinePrediction(*this).GetPrediction(input);
			if (result.hit_chance_ >= hit_chance)
			{
				pSDK->Control->CastSpell(spell->Slot, result.cast_position_);
				return true;
			}
			return false;
		}
		PredictionAoeResult result = LinePrediction(*this).GetAoePrediction(input.spell_.width_, input.spell_.delay_, input.spell_.missile_speed_, input.spell_.range_, input.from_, input.range_check_from_);
		if (result.hit_count_ >= min_hit)
		{
			pSDK->Control->CastSpell(spell->Slot, result.cast_position_);
			return true;
		}
		return false;
	}
	break;
	case kCircleCast:
	{
		if (!aoe)
		{
			Game::PrintChat("in sPrediction");
			PredictionResult result = CirclePrediction(*this).GetPrediction(input);
			if (result.hit_chance_ >= hit_chance)
			{
				pSDK->Control->CastSpell(spell->Slot, result.cast_position_);
				return true;
			}
			return false;
		}
		PredictionAoeResult result = CirclePrediction(*this).GetAoePrediction(input.spell_.width_, input.spell_.delay_, input.spell_.missile_speed_, input.spell_.range_, input.from_, input.range_check_from_);
		if (result.hit_count_ >= min_hit)
		{
			pSDK->Control->CastSpell(spell->Slot, result.cast_position_);
			return true;
		}
		return false;
	}
	break;
	case kConeCast:
	{
		if (!aoe)
		{
			PredictionResult result = ConePrediction(*this).GetPrediction(input);
			if (result.hit_chance_ >= hit_chance)
			{
				pSDK->Control->CastSpell(spell->Slot, result.cast_position_);
				return true;
			}
			return false;
		}
		PredictionAoeResult result = ConePrediction(*this).GetAoePrediction(input.spell_.width_, input.spell_.delay_, input.spell_.missile_speed_, input.spell_.range_, input.from_, input.range_check_from_);
		if (result.hit_count_ >= min_hit)
		{
			pSDK->Control->CastSpell(spell->Slot, result.cast_position_);
			return true;
		}
		return false;
	}
	break;
	default:
		throw std::exception("unknown spell type");
		break;
	}
	return false;
}

bool SPrediction::CastArcSpell(SDK_SPELL* spell, AIBaseClient *target, ePredictionChance hit_chance, bool aoe, uint8_t min_hit)
{
	PredictionInput input(target, spell);
	if (!aoe)
	{
		PredictionResult result = ArcPrediction(*this).GetPrediction(input);
		if (result.hit_chance_ >= hit_chance)
		{
			pSDK->Control->CastSpell(spell->Slot, result.cast_position_);
			return true;
		}
		return false;
	}
	PredictionAoeResult result = ArcPrediction(*this).GetAoePrediction(input.spell_.width_, input.spell_.delay_, input.spell_.missile_speed_, input.spell_.range_, input.from_, input.range_check_from_);
	if (result.hit_count_ >= min_hit)
	{
		pSDK->Control->CastSpell(spell->Slot, result.cast_position_);
		return true;
	}
	return false;
}

bool SPrediction::CastRingSpell(SDK_SPELL* spell, AIBaseClient *target, float ring_radius, ePredictionChance hit_chance)
{
	//PredictionInput input(target, spell);
	//input.spell_.ring_radius_ = ring_radius;

	//PredictionResult result = RingPrediction(*this).GetPrediction(input);
	//if (result.hit_chance_ >= hit_chance)
	//{
	//	spell->CastOnPosition(GUtility->To3D(result.cast_position_));
	//	return true;
	//}
	//return false;
}

bool SPrediction::CastVectorSpell(SDK_SPELL *spell, AIBaseClient *target, ePredictionChance hit_chance, bool aoe, uint8_t min_hit)
{
	PredictionInput input(target, spell);
	if (!aoe)
	{
		PredictionResult result = VectorPrediction(*this).GetPrediction(input);
		if (result.hit_chance_ >= hit_chance)
		{
			pSDK->Control->CastSpell(spell->Slot, result.cast_position_);
			return true;
		}
		return false;
	}
	PredictionAoeResult result = VectorPrediction(*this).GetAoePrediction(input.spell_.width_, input.spell_.delay_, input.spell_.missile_speed_, input.spell_.range_, input.from_, input.range_check_from_);
	if (result.hit_count_ >= min_hit)
	{
		pSDK->Control->CastSpell(spell->Slot, result.cast_position_);
		return true;
	}
	return false;
}

PredictionResult SPrediction::GetPrediction(SDK_SPELL* spell, AIBaseClient *target)
{
	PredictionInput input(target, spell);
	return GetPrediction(input);
}

PredictionResult SPrediction::GetPrediction(PredictionInput input)
{
	switch (input.spell_.type_)
	{
	case kLineCast:
		return LinePrediction(*this).GetPrediction(input);
	case kCircleCast:
		return CirclePrediction(*this).GetPrediction(input);
	case kConeCast:
		return ConePrediction(*this).GetPrediction(input);
	}
	return PredictionResult(input, input.target_, Vector2(0, 0), Vector2(0, 0), kHitChanceImpossible, CollisionResult());
}

Vector2 SPrediction::GetFastUnitPosition(AIBaseClient* target, float delay, float missile_speed, float distance_set, float move_speed)
{
	return GetFastUnitPosition(target, Player.GetServerPosition(), delay, missile_speed, distance_set, std::fpclassify(move_speed) == FP_ZERO ? target->GetMovementSpeed() : move_speed);
}

Vector2 SPrediction::GetFastUnitPosition(AIBaseClient* target, const Vector3& from, float delay, float missile_speed, float distance_set, float move_speed)
{
	if (std::fpclassify(move_speed) == FP_ZERO)
	{
		move_speed = target->GetMovementSpeed();
	}
	auto hero = target->AsAIHeroClient();
	auto path = target->GetWaypoints();

	if (path.size() <= 1 || Utility::IsImmobileTarget(target))
	{
		return Geometry::Helpers::To2D(target->GetServerPosition());
	}

	if (target->IsDashing())
	{
		return Geometry::Helpers::To2D(g_EnemyInfo[target->GetNetworkID()].last_dash_info_.EndPosition);
	}

	float _channel_t;
	if (target->IsCasting())
	{
		return Geometry::Helpers::To2D(target->GetServerPosition());
	}

	float distance = distance_set;
	if (std::fpclassify(distance) == FP_ZERO)
	{
		float target_distance = Geometry::Helpers::Distance(Geometry::Helpers::To2D(from), Geometry::Helpers::To2D(target->GetServerPosition()));

		float fly_time = 0;
		if (std::fpclassify(missile_speed) != FP_ZERO)
		{
			if (path.size() == 2)
			{
				Vector3 Vt = (path[1] - path[0]).Normalize() * move_speed;
				Vector3 Vs = (target->GetServerPosition() - from).Normalize() * missile_speed;
				Vector2 Vr = Geometry::Helpers::To2D(Vt - Vs);
				fly_time = target_distance / Vr.Length();
			}
			else
			{
				fly_time = target_distance / missile_speed;
			}
		}

		float t = fly_time + delay + Game::Ping() / 2000.0f;
		distance = t * move_speed;
	}

	for (auto it = path.begin(); it != std::prev(path.end()); ++it)
	{
		float d = Geometry::Helpers::Distance(Geometry::Helpers::To2D(*it), Geometry::Helpers::To2D(*std::next(it)));
		if (distance == d)
		{
			return Geometry::Helpers::To2D(*std::next(it));
		}
		else if (distance < d)
		{
			Geometry::Helpers::To2D(*it + distance * (*std::next(it) - *it).Normalize());
		}
		else
		{
			distance -= d;
		}
	}

	return Geometry::Helpers::To2D(path.back());
}

PredictionResult SPrediction::GetImmobilePrediction(AIBaseClient* target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from)
{
	return GetImmobilePrediction(PredictionInput(target, delay, missile_speed, width, range, collisionable, type, from, range_check_from));
}

PredictionResult SPrediction::GetImmobilePrediction(PredictionInput input)
{
	PredictionResult result(input, input.target_, Geometry::Helpers::To2D(input.target_->GetServerPosition()), Geometry::Helpers::To2D(input.target_->GetServerPosition()), kHitChanceImpossible, CollisionResult());

	float t = input.spell_.delay_ + Game::Ping() / 2000.0f;
	if (std::fpclassify(input.spell_.missile_speed_) != FP_ZERO)
	{
		t += Geometry::Helpers::Distance(Geometry::Helpers::To2D(input.from_), Geometry::Helpers::To2D(input.target_->GetServerPosition())) / input.spell_.missile_speed_;
	}

	if (input.spell_.type_ == kCircleCast)
	{
		t += input.spell_.width_ / input.target_->GetMovementSpeed() / 2.0f;
	}

	if (t >= Utility::LeftImmobileTime(input.target_))
	{
		result.hit_chance_ = kHitChanceImmobile;
		result.Lock();

		return result;
	}

	if (input.target_->IsHero())
	{
		result.hit_chance_ = GetHitChance(t - Utility::LeftImmobileTime(input.target_), m_path_tracker.GetAvgMovChangeTime(input.target_), 0, 0, 0);
	}
	else
	{
		result.hit_chance_ = kHitChanceHigh;
	}

	result.Lock();
	return result;
}

PredictionResult SPrediction::GetDashingPrediction(AIBaseClient* target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, const Vector3& from, const Vector3& range_check_from)
{
	return GetDashingPrediction(PredictionInput(target, delay, missile_speed, width, range, collisionable, type, from, range_check_from));
}

PredictionResult SPrediction::GetDashingPrediction(PredictionInput input)
{
	PredictionResult result(input, input.target_, Vector2(0, 0), Vector2(0, 0), kHitChanceImpossible, CollisionResult());

	if (input.target_->IsDashing())
	{
		auto dash_info = g_EnemyInfo[input.target_->GetNetworkID()].last_dash_info_;
		result.cast_position_ = GetFastUnitPosition(input.target_, input.spell_.delay_, input.spell_.missile_speed_, 0.0f, dash_info.Speed);
		result.unit_position_ = result.cast_position_;
		result.hit_chance_ = kHitChanceDashing;

		result.Lock(false);
	}

	return result;
}

PredictionResult SPrediction::WaypointAnalysis(AIBaseClient *target, float width, float delay, float missile_speed, float range, bool collisionable, eSpellType type, float avgt, float movt, float avgp, float anglediff, const Vector3& from, float move_speed)
{
	PredictionInput input(target, delay, missile_speed, width, range, collisionable, type, from, Vector3(0, 0, 0));
	input.avg_reaction_time_ = avgt;
	input.last_mov_change_time_ = movt;
	input.avg_path_lenght_ = avgp;
	input.last_angle_diff_ = anglediff;
	return WaypointAnalysis(input, move_speed);
}

PredictionResult SPrediction::WaypointAnalysis(PredictionInput input, float move_speed)
{
	if (std::fpclassify(move_speed) == FP_ZERO)
	{
		move_speed = input.target_->GetMovementSpeed();
	}

	PredictionResult result(input, input.target_, Vector2(0, 0), Vector2(0, 0), kHitChanceImpossible, CollisionResult());

	float max_fly_time = 0.0f;

	if (std::fpclassify(input.spell_.missile_speed_) != FP_ZERO)
	{
		max_fly_time = input.spell_.range_ / input.spell_.missile_speed_;
	}

	float t_min = input.spell_.delay_ + Game::Ping() / 2000.0f + 0 / 1000.0f;
	float t_max = max_fly_time + t_min + Game::Ping() / 2000.0f;
	float path_time = 0.0f;
	int path_bounds[2] = { -1, -1 };

	//find bounds
	for (auto it = input.path_.begin(); it != std::prev(input.path_.end()); ++it)
	{
		float t = Geometry::Helpers::Distance(it->To2D(), std::next(it)->To2D()) / move_speed;
		if (path_time <= t_min && path_time + t >= t_min)
		{
			path_bounds[0] = std::distance(input.path_.begin(), it);
		}
		if (path_time <= t_max && path_time + t >= t_max)
		{
			path_bounds[1] = std::distance(input.path_.begin(), it);
		}

		if (path_bounds[0] != -1 && path_bounds[1] != -1)
		{
			break;
		}

		path_time += t;
	}

	if (path_bounds[0] == -1 || path_bounds[1] == -1)
	{
		if (input.path_.size() == 2)
		{
			float move_time = Geometry::Helpers::Distance(input.path_[0].To2D(), input.path_[1].To2D()) / move_speed;
			float t_min_left = t_min - move_time;
			float t_max_left = t_max - move_time;

			float t_calc = t_max_left;
			if (t_min_left > 0)
			{
				t_calc = t_min_left;
			}

			Vector2 direction = Geometry::Helpers::To2D(input.path_[1] - input.path_[0]).Normalize();
			result.hit_chance_ = kHitChanceMedium;

			result.cast_position_ = input.path_[1].To2D() + direction * (t_calc * move_speed - input.spell_.width_ * 2.f);
			result.unit_position_ = input.path_[1].To2D();
			if (result.cast_position_.To3D().IsWall())
			{
				result.hit_chance_ = kHitChanceImpossible;
			}
			else
			{
				float ratio = move_time / (t_calc + move_time);
				if (ratio < 0.3f)
				{
					result.hit_chance_ = kHitChanceVeryHigh;
				}
				else if (ratio < 0.5f)
				{
					result.hit_chance_ = kHitChanceHigh;
				}
			}

			return result;
		}
	}

	if (path_bounds[0] != -1 && path_bounds[1] != -1)
	{
		for (int k = path_bounds[0]; k <= path_bounds[1]; k++)
		{
			Vector2 direction =(input.path_[k + 1] - input.path_[k]).To2D().Normalize();
			float distance = input.spell_.width_;
			float extender = input.target_->GetBoundingRadius();

			if (input.spell_.type_ == kLineCast)
			{
				extender = input.spell_.width_;
			}

			int steps = floor(Geometry::Helpers::Distance(input.path_[k], input.path_[k + 1]) / distance);
			for (int i = 1; i < steps - 1; i++)
			{
				Vector2 pt_center = input.path_[k].To2D() + (direction * distance * i);
				Vector2 pt_a = pt_center - (direction * extender);
				Vector2 pt_b = pt_center + (direction * extender);

				float fly_time = 0.0f;
				if (std::fpclassify(input.spell_.missile_speed_) != FP_ZERO)
				{
					fly_time = Geometry::Helpers::Distance(Geometry::Helpers::To2D(input.from_), pt_center) / input.spell_.missile_speed_;
				}
				float t = fly_time + input.spell_.delay_ + Game::Ping() / 2000.0f + 0 / 1000.0f;

				Vector2 current_position = Geometry::Helpers::To2D(input.target_->GetServerPosition());

				float t_a = Geometry::Helpers::Distance(current_position, pt_a) / move_speed;
				float t_b = Geometry::Helpers::Distance(current_position, pt_b) / move_speed;

				if (min(t_a, t_b) <= t && max(t_a, t_b) >= t)
				{
					result.hit_chance_ = GetHitChance(t * 1000.f, input.avg_reaction_time_, input.last_mov_change_time_, input.avg_path_lenght_, input.last_angle_diff_);
					result.cast_position_ = pt_center;
					result.unit_position_ = pt_center + (direction * (t - min(t_a, t_b)) * move_speed);
					return result;
				}
			}
		}
	}
	return result;
}

float SPrediction::GetArrivalTime(float distance, float delay, float missile_speed)
{

	if (std::fpclassify(missile_speed) != FP_ZERO)
	{
		return distance / missile_speed + delay;
	}
	return delay;
}

ePredictionChance SPrediction::GetHitChance(float t, float avgt, float movt, float avgp, float anglediff)
{
	if (avgp > 400)
	{
		if (movt > 50)
		{
			if (avgt >= t + Game::Ping())
			{
				if (anglediff < 30)
				{
					return kHitChanceVeryHigh;
				}
				else
				{
					return kHitChanceHigh;
				}
			}
			else if (avgt - movt >= t)
			{
				return kHitChanceMedium;
			}
			else
			{
				return kHitChanceLow;
			}
		}
		else
		{
			return kHitChanceVeryHigh;
		}
	}
	else
	{
		return kHitChanceHigh;
	}
}

Collision& SPrediction::GetCollisionEngine()
{
	return m_collision;
}

PathTracker& SPrediction::GetPathTrackerEngine()
{
	return m_path_tracker;
}

SPrediction::SPrediction()
{
}

SPrediction::~SPrediction()
{

}

std::unique_ptr<SPrediction> sPred = std::make_unique<SPrediction>();