#include <regex>
#include "Collision.h"
#include "SPrediction.h"
#include "Geometry\ClipperWrapper.hpp"
#include "Geometry\Helpers.h"

uint32_t Collision::s_YasuoWallCastedTick = 0;
uint8_t Collision::s_YasuoWallLvl;
Vector2 Collision::s_YasuoWallCastedPos;
Vector2 Collision::s_YasuoWallPos;

Collision::Collision()
{

}

bool Collision::CheckCollision(const Vector2& from, const Vector2& to, float width, float delay, float missile_speed, bool check_minion, bool check_enemy, bool check_yasuowall, bool check_ally, bool check_wall, bool is_arc)
{
	return (check_minion && CheckMinionCollision(from, to, width, delay, missile_speed, is_arc))
		|| (check_enemy && CheckEnemyHeroCollision(from, to, width, delay, missile_speed, is_arc))
		|| (check_yasuowall && CheckYasuoWallCollision(from, to, width, is_arc))
		|| (check_ally && CheckAllyHeroCollision(from, to, width, delay, missile_speed, is_arc))
		|| (check_wall && CheckWallCollision(from, to));
}

bool Collision::CheckMinionCollision(const Vector2& from, const Vector2& to, float width, float delay, float missile_speed, bool is_arc)
{
	auto spell_hitbox = Geometry::ClipperWrapper::MakePaths(MAKE_RECTANGLE(from, to, width));
	if (is_arc)
	{
		spell_hitbox = Geometry::ClipperWrapper::MakePaths(Geometry::Polygon({
			MAKE_ARC(from - Vector2(900 / 2.0f, 20.0f), to, M_PI * (Geometry::Helpers::Distance(from, to) / 900.0f), 410, 200 * Geometry::Helpers::Distance(from, to) / 900.0f).GetPoints(),
			MAKE_ARC(from - Vector2(900 / 2.0f, 20.0f), to, M_PI * (Geometry::Helpers::Distance(from, to) / 900.0f), 410, 320 * Geometry::Helpers::Distance(from, to) / 900.0f).GetPoints()
			}));
	}

	auto minions = pSDK->EntityManager->GetEnemyMinions();

	for (auto& minion : minions)
	{
		auto hitbox = Geometry::ClipperWrapper::MakePaths(MAKE_CIRCLE(sPred->GetFastUnitPosition(minion.second, delay, missile_speed), minion.second->GetBoundingRadius() * 2.0f + 10.0f));
		return Geometry::ClipperWrapper::IsIntersects(hitbox, spell_hitbox);
	}
}

bool Collision::CheckEnemyHeroCollision(const Vector2& from, const Vector2& to, float width, float delay, float missile_speed, bool is_arc)
{
	auto spell_hitbox = Geometry::ClipperWrapper::MakePaths(MAKE_RECTANGLE(from, to, width));
	if (is_arc)
	{
		spell_hitbox = Geometry::ClipperWrapper::MakePaths(Geometry::Polygon({
			MAKE_ARC(from - Vector2(900 / 2.0f, 20.0f), to, M_PI * (Geometry::Helpers::Distance(from, to) / 900.0f), 410, 200 * Geometry::Helpers::Distance(from, to) / 900.0f).GetPoints(),
			MAKE_ARC(from - Vector2(900 / 2.0f, 20.0f), to, M_PI * (Geometry::Helpers::Distance(from, to) / 900.0f), 410, 320 * Geometry::Helpers::Distance(from, to) / 900.0f).GetPoints()
			}));
	}

	auto heroes = pSDK->EntityManager->GetEnemyHeroes();

	for (auto& hero : heroes)
	{
		auto hitbox = Geometry::ClipperWrapper::MakePaths(MAKE_CIRCLE(sPred->GetFastUnitPosition(hero.second, delay, missile_speed), hero.second->GetBoundingRadius()));
		return Geometry::ClipperWrapper::IsIntersects(hitbox, spell_hitbox);
	}
}

bool Collision::CheckAllyHeroCollision(const Vector2& from, const Vector2& to, float width, float delay, float missile_speed, bool is_arc)
{
	auto spell_hitbox = Geometry::ClipperWrapper::MakePaths(MAKE_RECTANGLE(from, to, width));
	if (is_arc)
	{
		spell_hitbox = Geometry::ClipperWrapper::MakePaths(Geometry::Polygon({
			MAKE_ARC(from - Vector2(900 / 2.0f, 20.0f), to, M_PI * (Geometry::Helpers::Distance(from, to) / 900.0f), 410, 200 * Geometry::Helpers::Distance(from, to) / 900.0f).GetPoints(),
			MAKE_ARC(from - Vector2(900 / 2.0f, 20.0f), to, M_PI * (Geometry::Helpers::Distance(from, to) / 900.0f), 410, 320 * Geometry::Helpers::Distance(from, to) / 900.0f).GetPoints()
			}));
	}

	auto heroes = pSDK->EntityManager->GetAllyHeroes();

	for (auto& hero : heroes)
	{
		auto hitbox = Geometry::ClipperWrapper::MakePaths(MAKE_CIRCLE(sPred->GetFastUnitPosition(hero.second, delay, missile_speed), hero.second->GetBoundingRadius()));
		return Geometry::ClipperWrapper::IsIntersects(hitbox, spell_hitbox);
	}
}

bool Collision::CheckWallCollision(const Vector2& from, const Vector2& to)
{
	//float step = Geometry::Helpers::Distance(from, to) / 20;
	//for (int i = 0; i < 20; i++)
	//{
	//	auto p = 
	//	if (GNavMesh->IsPointWall(GUtility->To3D(p)))
	//	{
	//		return true;
	//	}
	//}
	//return false;
}

bool Collision::CheckYasuoWallCollision(const Vector2& from, const Vector2& to, float width, bool is_arc)
{
	if (GetTickCount() - s_YasuoWallCastedTick > 4000)
	{
		return false;
	}

	Vector2 direction = Geometry::Helpers::Perpendicular((s_YasuoWallPos - s_YasuoWallCastedPos).Normalize());
	float w = 300 + 50 * s_YasuoWallLvl;
	Vector2 start = s_YasuoWallPos + w / 2 * direction;
	Vector2 end = start - w / 2 * direction;

	auto hitbox = Geometry::ClipperWrapper::MakePaths(MAKE_RECTANGLE(start, end, 5));
	auto spell_hitbox = Geometry::ClipperWrapper::MakePaths(MAKE_RECTANGLE(from, to, width));
	if (is_arc)
	{
		spell_hitbox = Geometry::ClipperWrapper::MakePaths(Geometry::Polygon({
			MAKE_ARC(from - Vector2(900 / 2.0f, 20.0f), to, M_PI * (Geometry::Helpers::Distance(from, to) / 900.0f), 410, 200 * Geometry::Helpers::Distance(from, to) / 900.0f).GetPoints(),
			MAKE_ARC(from - Vector2(900 / 2.0f, 20.0f), to, M_PI * (Geometry::Helpers::Distance(from, to) / 900.0f), 410, 320 * Geometry::Helpers::Distance(from, to) / 900.0f).GetPoints()
			}));
	}

	return Geometry::ClipperWrapper::IsIntersects(hitbox, spell_hitbox);
}

CollisionResult Collision::GetCollisions(const Vector2& from, const Vector2& to, float range, float width, float delay, float missile_speed, bool is_arc)
{
	std::vector<AIBaseClient*> collided_units;
	auto spell_hitbox = Geometry::ClipperWrapper::MakePaths(MAKE_RECTANGLE(from, to, width));
	if (is_arc)
	{
		spell_hitbox = Geometry::ClipperWrapper::MakePaths(Geometry::Polygon({
			MAKE_ARC(from - Vector2(900 / 2.0f, 20.0f), to, M_PI * (Geometry::Helpers::Distance(from, to) / 900.0f), 410, 200 * Geometry::Helpers::Distance(from, to) / 900.0f).GetPoints(),
			MAKE_ARC(from - Vector2(900 / 2.0f, 20.0f), to, M_PI * (Geometry::Helpers::Distance(from, to) / 900.0f), 410, 320 * Geometry::Helpers::Distance(from, to) / 900.0f).GetPoints()
			}));
	}

	sCollisionFlags _flags = sCollisionFlags::None;
	auto minions = pSDK->EntityManager->GetEnemyMinions();
	for (auto& minion : minions)
	{
		auto hitbox = Geometry::ClipperWrapper::MakePaths(MAKE_CIRCLE(sPred->GetFastUnitPosition(minion.second, delay, missile_speed), minion.second->GetBoundingRadius() + 15.0f));
		if (Geometry::ClipperWrapper::IsIntersects(hitbox, spell_hitbox))
		{
			collided_units.push_back(minion.second);
			_flags |= sCollisionFlags::Minions;
		}
	}

	auto enemies = pSDK->EntityManager->GetEnemyHeroes();
	for (auto& hero : enemies)
	{
		auto hitbox = Geometry::ClipperWrapper::MakePaths(MAKE_CIRCLE(sPred->GetFastUnitPosition(hero.second, delay, missile_speed), hero.second->GetBoundingRadius()));
		if (Geometry::ClipperWrapper::IsIntersects(hitbox, spell_hitbox))
		{
			collided_units.push_back(hero.second);
			_flags |= sCollisionFlags::EnemyChampions;
		}
	}

	auto allies = pSDK->EntityManager->GetAllyHeroes();
	for (auto& hero : allies)
	{
		auto hitbox = Geometry::ClipperWrapper::MakePaths(MAKE_CIRCLE(sPred->GetFastUnitPosition(hero.second, delay, missile_speed), hero.second->GetBoundingRadius()));
		if (Geometry::ClipperWrapper::IsIntersects(hitbox, spell_hitbox))
		{
			collided_units.push_back(hero.second);
			_flags |= sCollisionFlags::AllyChampions;
		}
	}

	if (CheckWallCollision(from, to))
	{
		_flags |= sCollisionFlags::Wall;
	}

	if (CheckYasuoWallCollision(from, to, width, is_arc))
	{
		_flags |= sCollisionFlags::YasuoWall;
	}

	return CollisionResult(std::move(collided_units), _flags);
}

void Collision::OnProcessSpell(void* AI, PSDK_SPELL_CAST SpellCast, void* UserData)
{
	auto sender = pSDK->EntityManager->GetObjectFromPTR(AI); //YasuoWMovingWall
	if (sender->IsValid() && sender->GetTeamID() != Player.GetTeamID() && Common::CompareLower(SpellCast->Spell.Name, "YasuoWMovingWall"))
	{
		s_YasuoWallCastedTick = GetTickCount();
		s_YasuoWallLvl = SpellCast->Spell.Level;
		auto& pos = SpellCast->EndPosition;
		s_YasuoWallCastedPos = Vector2(pos.x, pos.z);
	}

}

void Collision::OnCreateObject(void* Object, unsigned NetworkID, void* UserData)
{
	auto sender = pSDK->EntityManager->GetObjectFromPTR(Object);

	auto name = sender->GetName();
	std::regex re("_w_windwall_enemy_0.\\.troy", std::regex_constants::ECMAScript | std::regex_constants::icase);
	std::cmatch m;
	if (std::regex_match(name, m, re))
	{
		s_YasuoWallPos = Geometry::Helpers::To2D(sender->GetPosition());
	}
}

Collision::~Collision()
{
}
