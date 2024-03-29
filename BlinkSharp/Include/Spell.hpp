#pragma once

#include "SDK Extensions.h"
#include <string>

namespace Spell {

#pragma region Abstract Classes
	class SpellBase {
		std::string _invalidName{ "" };
	public:
		SpellSlot Slot;
		float Range;
		float Delay;
		float Speed;
		float Width;
		int   Collision;

		DamageType DmgType;
		SkillshotType Type = SkillshotType::Line;
		AIBaseClient* From = &Player;
		AIBaseClient* RangeCheckFrom = From;
		int  CollisionFlags = (int)(CollisionFlags::Minions) | (int)(CollisionFlags::YasuoWall);

		unsigned int LastCastTick;
		float LastCast;

		bool IsValid() {
			return (int)Slot >= SPELL_SLOT_START && (int)Slot <= SPELL_SLOT_MAX;
		}
		SDK_SPELL Handle() {
			return Player.GetSpell((unsigned char)Slot);
		}
		bool IsLearned() {
			return IsValid() && Handle().Learned;
		}
		int Level() {
			return IsValid() ? Handle().Level : 0;
		}
		std::string Name() {
			return IsValid() ? std::string(Handle().ScriptName) : _invalidName;
		}
		SpellState State() {
			if (!IsValid()) { return SpellState::Unknown; }
			int CanCastFlags = 1;
			CHECKRAWFAIL(SdkCanAICastSpell(Player.PTR(), (unsigned char)Slot, NULL, &CanCastFlags));
			return (SpellState)CanCastFlags;
		}
		int ToggleState() {
			return IsValid() && Handle().ToggleState;
		}
		bool IsOnCooldown() {
			return IsValid() && (State() >= SpellState::Cooldown);
		}
		float ManaCost() {
			if (!IsValid() || !IsLearned()) { return 0; }
			return Handle().ManaCost;
		}
		int AmmoQuantity() {
			return Handle().CurrentAmmo;
		}
		float RangeSqr() {
			return Range * Range;
		}
		virtual AIHeroClient* GetTarget() {
			return pCore->TS->GetTarget(Range, DmgType);
		}
		virtual float GetHealthPrediction(AIBaseClient* target) {
			return (target && IsValid()) ? pSDK->HealthPred->GetHealthPrediction(target, (unsigned int)(Delay*1000.0f)) : 0.0f;
		}
		virtual float GetSpellDamage(AIBaseClient* target) {
			return (From && target && IsValid()) ? pSDK->DamageLib->GetSpellDamage(From, target, (unsigned char)Slot, SkillStage::Default) : 0.0f;
		}
		bool IsReady(unsigned int extraTime = 0) {
			if (!IsValid()) { return false; }

			return extraTime == 0 ? (State() == SpellState::Ready)
				: Handle().CooldownExpires + extraTime / 1000.f - Game::Time() < 0;
		}
		bool IsInRange(AIBaseClient* targetEntity) {
			return (RangeCheckFrom && RangeCheckFrom->IsValid() ? Player.Distance(targetEntity) < Range : false);
		}
		bool IsInRange(Vector3* targetPosition) {
			return (RangeCheckFrom && RangeCheckFrom->IsValid() ? Player.Distance(targetPosition) < Range : false);
		}

		virtual void DrawRange(PSDKCOLOR color, float lineWidth = 3.f) {
			auto pos{ (From ? From : &Player)->GetPosition() };
			Draw::Circle(&pos, Range, color);
		}

		virtual bool Cast() { return false; };
		virtual bool Cast(AIBaseClient* Target) { return false; };
		virtual bool Cast(Vector3* Pos) { return false; };
		virtual bool Cast(Vector3* startPos, Vector3* endPos) { return false; };

	protected:
		SpellBase(SpellSlot spellSlot, float spellRange = HUGE_VALF, DamageType dmgType = DamageType::Physical) {
			this->Slot = spellSlot;
			this->Range = spellRange;
			this->DmgType = dmgType;
		}
	};

	class Ranged : public Spell::SpellBase {
	protected:
		Ranged(SpellSlot spellSlot, float spellRange, DamageType dmgType = DamageType::Physical)
			: SpellBase(spellSlot, spellRange, dmgType) {};

	public:
		bool Cast() {
			SdkUiConsoleWrite("Can't Cast Spell %s Without a Target!", Name().c_str());
			return false;
		}
	};

#pragma endregion

#pragma region Skillshot
	class Skillshot : public Ranged {
	public:
		SkillshotType Type;

		float Speed;
		float Width;
		float Radius;

		float ConeAngleDegrees;

		HitChance MinimumHitChance;
		int AllowedCollisionCount;
		bool HasCollision() { return AllowedCollisionCount > 0; }

		Skillshot(SpellSlot spellSlot, float spellRange, SkillshotType skillShotType, float castDelay = 0.250f,
			float spellSpeed = 0.0f, float spellWidth = 0.0f, DamageType dmgType = DamageType::Physical)
			: Ranged(spellSlot, spellRange, dmgType)
		{
			Type = skillShotType;

			Delay = castDelay;
			Speed = spellSpeed;
			Width = spellWidth;
			ConeAngleDegrees = 90.0f;

			MinimumHitChance = HitChance::Medium;
		}

		Skillshot(unsigned char spellSlot, float spellRange, SkillshotType skillShotType, float castDelay = 0.250f,
			float spellSpeed = 0.0f, float spellWidth = 0.0f, DamageType dmgType = DamageType::Physical)
			: Ranged((SpellSlot)spellSlot, spellRange, dmgType)
		{
			Type = skillShotType;

			Delay = castDelay;
			Speed = spellSpeed;
			Width = spellWidth;
			ConeAngleDegrees = 90.0f;

			MinimumHitChance = HitChance::Medium;
		}

		//virtual std::shared_ptr<IPrediction::Output> GetPrediction(AIHeroClient* target) {
		//	return pCore->Prediction->GetPrediction(target, Type == SkillshotType::Circle ? Radius : Width, Delay, Speed, Range, AllowedCollisionCount > 0, Type);
		//}
		float GetHealthPrediction(AIBaseClient* target) override {
			auto time{ (unsigned int)(Delay * 1000.0f) };

			if (std::abs(Speed - HUGE_VALF) > EPSILON) {
				time += (int)(1000.0f * max(target->Distance(From ? From : &Player) - Player.GetBoundingRadius(), 0) / Speed > 1.0f ? Speed : 1.f);
			}

			return pSDK->HealthPred->GetHealthPrediction(target, time);
		}
		bool  Cast(Vector3* targetPosition)				override {
			bool result{ pSDK->Control->CastSpell((unsigned char)Slot, targetPosition) };
			if (result) {
				LastCast = Game::Time();
				LastCastTick = GetTickCount();
			}
			return result;
		}
		//bool  Cast(AIBaseClient* targetEntity)			override {
		//	if (!Game::IsAvailable() || !targetEntity || targetEntity->IsValid()) { return false; }
		//
		//	if (!targetEntity->IsHero()) {
		//		auto from{ (From ? From : &Player)->GetPosition().To2D() };
		//		auto pos{ pCore->Prediction->GetFastPrediction(targetEntity, Delay, Speed, &from) };
		//
		//		if (pos.IsValid() && pos.DistanceSqr(from) < RangeSqr()) {
		//			pSDK->Control->CastSpell((unsigned char)Slot, &(pos.To3D(from.y)));
		//			return true;
		//		}
		//		return false;
		//	}
		//
		//	auto Target{ (AIHeroClient*)targetEntity };
		//
		//	auto prediction{ GetPrediction(Target) };
		//
		//	if (prediction->Hitchance < MinimumHitChance) {
		//		return false;
		//	}
		//
		//	if (Player.Distance(&(prediction->UnitPosition)) < Range) {
		//		bool result{ pSDK->Control->CastSpell((unsigned char)Slot, &(prediction->CastPosition)) };
		//		if (result) {
		//			LastCast = Game::Time();
		//			LastCastTick = GetTickCount();
		//		}
		//		return result;
		//	}
		//	return false;
		//}
		bool  Cast(Vector3* start, Vector3* end)		override {
			bool result{ pSDK->Control->CastSpell((unsigned char)Slot, start, end) };
			if (result) {
				LastCast = Game::Time();
				LastCastTick = GetTickCount();
			}
			return result;
		}

		struct BestPosition {
			int HitNumber;
			Vector3 CastPosition;
		};

		/*
		virtual BestPosition GetBestLinearCastPosition(IEnumerable<Obj_AI_Base> entities, int moreDelay = 0, Vector2 ? sourcePosition = null)
		{
			var targets = entities.ToArray();

			switch (targets.Length)
			{
			case 0:
				return new BestPosition();
			case 1:
				return new BestPosition
				{
					CastPosition = GetPrediction(targets[0]).CastPosition,
					HitNumber = 1
				};
			}

			var extradelay = CastDelay + moreDelay;
			var possiblePositions =
				new List<Vector2>(
					targets.OrderBy(o = > o.Health)
					.Select(o = > Prediction.Position.PredictUnitPosition(o, extradelay)));
			foreach(var target in targets)
			{
				var predictedPos = Prediction.Position.PredictUnitPosition(target, extradelay);
				possiblePositions.AddRange(from t in targets
					orderby t.Health
				where t.NetworkId != target.NetworkId
					select(predictedPos + predictedPos) / 2);
			}

			var startPos = sourcePosition ? ? Player.Instance.ServerPosition.To2D();
			var minionCount = 0;
			var result = Vector2.Zero;

			foreach(var pos in possiblePositions.Where(o = > o.IsInRange(startPos, Range)))
			{
				var endPos = startPos + Range * (pos - startPos).Normalized();
				var count =
					targets.Where(t = > t.IsValidTarget())
					.OrderBy(o = > o.Health)
					.Count(
						o = >
						Prediction.Position.PredictUnitPosition(o, extradelay)
						.Distance(startPos, endPos, true, true) <= Width * Width);

				if (count >= minionCount)
				{
					result = endPos;
					minionCount = count;
				}
			}

			return new BestPosition
			{
				CastPosition = result.To3DWorld(),
				HitNumber = minionCount
			};
		}

		public virtual BestPosition GetBestCircularCastPosition(IEnumerable<Obj_AI_Base> entities, int hitChance = 60, int moreDelay = 0)
		{
			var bestCircularCastPos =
				Prediction.Position.PredictCircularMissileAoe(entities.ToArray(), Range, Width, CastDelay + moreDelay, Speed)
				.OrderByDescending(r = > r.GetCollisionObjects<AIHeroClient>().Length)
				.ThenByDescending(r = > r.GetCollisionObjects<Obj_AI_Base>().Length)
				.FirstOrDefault();

			if (bestCircularCastPos != null && bestCircularCastPos.HitChancePercent >= hitChance)
			{
				return new BestPosition
				{
					CastPosition = bestCircularCastPos.CastPosition,
					HitNumber = bestCircularCastPos.CollisionObjects.Length
				};
			}

			return new BestPosition
			{
				CastPosition = Vector3.Zero,
				HitNumber = 0
			};
		}

		/// <summary>
		/// Get the best postion to cast a cone spell
		/// </summary>
		/// <param name="entities"></param>
		/// <param name="hitChance"></param>
		/// <param name="moreDelay"></param>
		/// <returns></returns>
		public virtual BestPosition GetBestConeCastPosition(IEnumerable<Obj_AI_Base> entities, int hitChance = 60, int moreDelay = 0)
		{
			var bestCastConePos =
				Prediction.Position.PredictConeSpellAoe(entities.ToArray(), Range, Width, CastDelay + moreDelay, Speed)
				.OrderByDescending(r = > r.GetCollisionObjects<AIHeroClient>().Length)
				.ThenByDescending(r = > r.GetCollisionObjects<Obj_AI_Base>().Length)
				.FirstOrDefault();

			if (bestCastConePos != null && bestCastConePos.HitChancePercent >= hitChance)
			{
				return new BestPosition
				{
					CastPosition = bestCastConePos.CastPosition,
					HitNumber = bestCastConePos.CollisionObjects.Length
				};
			}

			return new BestPosition{ CastPosition = Vector3.Zero, HitNumber = 0 };
		}

		/// <summary>
		/// Cast os the best position to hit as many heroes as possible
		/// </summary>
		/// <param name="minTargets">The minimun enemy hero count to cast spell</param>
		/// <param name="minHitchancePercent">Hitchance</param>
		/// <returns></returns>
		public virtual bool CastIfItWillHit(int minTargets = 2, int minHitchancePercent = 75)
		{
			switch (Type)
			{
			case SkillShotType.Linear:
				var targetsLinear = EntityManager.Heroes.Enemies.Where(CanCast).ToArray();
				var pred = GetBestLinearCastPosition(targetsLinear);

				if (pred.CastPosition != Vector3.Zero && pred.HitNumber > 0)
				{
					if (pred.HitNumber >= minTargets)
					{
						return Cast(pred.CastPosition);
					}
				}
				break;
			case SkillShotType.Circular:
				var targetsCircular = EntityManager.Heroes.Enemies.Where(CanCast).ToArray();
				var predCircular = GetBestLinearCastPosition(targetsCircular);

				if (predCircular.CastPosition != Vector3.Zero && predCircular.HitNumber > 0)
				{
					if (predCircular.HitNumber >= minTargets)
					{
						return Cast(predCircular.CastPosition);
					}
				}
				break;
			case SkillShotType.Cone:
				var targetsCone = EntityManager.Heroes.Enemies.Where(CanCast).ToArray();
				var predCone = GetBestLinearCastPosition(targetsCone);

				if (predCone.CastPosition != Vector3.Zero && predCone.HitNumber > 0)
				{
					if (predCone.HitNumber >= minTargets)
					{
						return Cast(predCone.CastPosition);
					}
				}
				break;
			}
			return false;
		}

		/// <summary>
		/// Cast spells on the best farm position
		/// </summary>
		/// <param name="minMinion"></param>
		/// <param name="hitChance"></param>
		/// <returns></returns>
		public virtual bool CastOnBestFarmPosition(int minMinion = 3, int hitChance = 50)
		{
			switch (Type)
			{
			case SkillShotType.Linear:
				var minionsLinear = EntityManager.MinionsAndMonsters.EnemyMinions.Where(CanCast).OrderBy(m = > m.Health);
				var farmLocationLinear = GetBestLinearCastPosition(minionsLinear);
				if (farmLocationLinear.HitNumber >= minMinion)
				{
					Cast(farmLocationLinear.CastPosition);
				}
				break;
			case SkillShotType.Circular:
				var minionsCircular = EntityManager.MinionsAndMonsters.EnemyMinions.Where(CanCast).OrderBy(m = > m.Health).ToArray();

				var farmLocationCircular = GetBestCircularCastPosition(minionsCircular, hitChance);
				if (farmLocationCircular.HitNumber >= minMinion)
				{
					Cast(farmLocationCircular.CastPosition);
				}
				break;
			case SkillShotType.Cone:
				var minionsCone = EntityManager.MinionsAndMonsters.EnemyMinions.Where(CanCast).OrderBy(m = > m.Health).ToArray();
				var farmLocationCone = GetBestConeCastPosition(minionsCone, hitChance);
				if (farmLocationCone.HitNumber >= minMinion)
				{
					Cast(farmLocationCone.CastPosition);
				}
				break;
			}
			return false;
		}

		#endregion BestPosition

			public virtual bool CastMinimumHitchance(Obj_AI_Base target, HitChance hitChance = HitChance.Medium)
		{
			if (target == null) return false;
			var pred = GetPrediction(target);
			return pred.HitChance >= hitChance && Cast(pred.CastPosition);
		}
		*/

		//virtual bool CastMinimumHitchance(AIHeroClient* target, HitChance hitChancePercent = HitChance::High) {
		//	if (target == NULL) return false;
		//	auto   pred{ GetPrediction(target) };
		//	return pred->Hitchance >= hitChancePercent && Cast(&(pred->CastPosition.To3D()));
		//}
	};
#pragma endregion

#pragma region Targeted

	class Targeted : public Ranged {
	public:
		Targeted(SpellSlot spellSlot, float spellRange, DamageType dmgType = DamageType::Physical)
			: Ranged(spellSlot, spellRange, dmgType) {};
		Targeted(unsigned char spellSlot, float spellRange, DamageType dmgType = DamageType::Physical)
			: Ranged((SpellSlot)spellSlot, spellRange, dmgType) {};

		bool Cast(AIBaseClient* targetEntity)	override {
			bool result{ pSDK->Control->CastSpell((unsigned char)Slot, targetEntity) };
			if (result) {
				LastCast = Game::Time();
				LastCastTick = GetTickCount();
			}
			return result;
		}
		bool Cast(Vector3* targetPosition)		override {
			bool result{ pSDK->Control->CastSpell((unsigned char)Slot, targetPosition) };
			if (result) {
				LastCast = Game::Time();
				LastCastTick = GetTickCount();
			}
			return result;
		}
	};

#pragma endregion

#pragma region Active

	class Active : public SpellBase {
	public:
		Active(SpellSlot spellSlot, float spellRange = HUGE_VALF, DamageType dmgType = DamageType::Physical)
			: SpellBase(spellSlot, spellRange, dmgType) {};
		
		Active(unsigned char spellSlot, float spellRange = HUGE_VALF, DamageType dmgType = DamageType::Physical)
			: SpellBase((SpellSlot)spellSlot, spellRange, dmgType) {};

		bool Cast()								override {
			bool result{ pSDK->Control->CastSpell((unsigned char)Slot) };
			if (result) {
				LastCast = Game::Time();
				LastCastTick = GetTickCount();
			}
			return result;
		}
		bool Cast(AIBaseClient* targetEntity)	override {
			bool result{ pSDK->Control->CastSpell((unsigned char)Slot, targetEntity) };
			if (result) {
				LastCast = Game::Time();
				LastCastTick = GetTickCount();
			}
			return result;
		}
		bool Cast(Vector3* targetPosition)		override {
			return false;
		}
	};

#pragma endregion 

}