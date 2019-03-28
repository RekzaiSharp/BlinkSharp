#pragma once
#include "../Include/SDK Extensions.h"

class SharpOrbwalker : IOrbwalker
{
	static bool lowPing;
	static void Init()
	{
		if (Game::Ping () < 30)
			lowPing = true;


	}
	static bool SpellChanneling()
	{
		if (Common::CompareLower (Player.GetCharName(), "Fiddlesticks")) {
			if (Player.HasBuff ("Drain", false)) {
				return true;
			}
			if (Player.HasBuff ("Crowstorm", false)) {
				return true;
			}

		}
		if (Common::CompareLower (Player.GetCharName(), "Galio")) {
			if (Player.HasBuff ("GalioIdolOfDurand", false)) {
				return true;
			}
		}
		if (Common::CompareLower (Player.GetCharName(), "Karthus")) {
			if (Player.HasBuff ("karthusfallenonecastsound", true)) {
				return true;
			}
		}
		if (Common::CompareLower (Player.GetCharName(), "Katarina")) {
			if (Player.HasBuff ("katarinarsound", true)) {
				return true;
			}
		}
		if (Common::CompareLower (Player.GetCharName(), "Malzahar")) {
			if (Player.HasBuff ("alzaharnethergraspsound", false)) {
				return true;
			}
		}
		if ((Common::CompareLower (Player.GetCharName(), "MissFortune"))) {
			if (Player.HasBuff ("missfortunebulletsound", false)) {
				return true;
			}
		}
		if (Common::CompareLower (Player.GetCharName(), "Nunu")) {
			if (Player.HasBuff ("AbsoluteZero", false)) {
				return true;
			}
		}
		if (Common::CompareLower (Player.GetCharName(), "Pantheon")) {
			if (Player.HasBuff ("pantheonesound", false)) {
				return true;
			}
			if (Player.HasBuff ("PantheonRJump", false)) {
				return true;
			}
		}
		if (Common::CompareLower (Player.GetCharName(), "Shen")) {
			if (Player.HasBuff ("shenstandunitedlock", false)) {
				return true;
			}
		}
		if (Common::CompareLower (Player.GetCharName(), "TwistedFate")) {
			if (Player.HasBuff ("Destiny", false)) {
				return true;
			}
		}
		if (Common::CompareLower (Player.GetCharName(), "VelkozR")) {
			if (Player.HasBuff ("velkozr", false)) {
				return true;
			}
		}
		if (Common::CompareLower (Player.GetCharName(), "Xerath")) {
			if (Player.HasBuff ("XerathLocusOfPower2", false)) {
				return true;
			}
		}

		return false;
	}
	static bool IsWindingUp (float extrad) {

	}
	static void Tick()
	{

	}
	bool CanMove() override
	{
		if (Player.HasBuff ("KalistaSpearHoldVFXBuff") && Common::CompareLower (Player.GetCharName (), "kalista"))
			return true;

		if (SpellChanneling ())
			return false;

		float baseAttackSpeed = 1 / Player.GetAttackDelay ();
		float extrarad = 0;

		return true;

	}
	bool CanAttack() override
	{
	}
};
