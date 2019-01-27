#pragma once
#include "../../../Include/SDK Extensions.h"

class Irelia_Combo
{
public:

	enum ActiveModes
	{
		None,
		GapCloser,
		Trading,
		Teamfight
	};


	AttackableUnit* ComboTarget;
	auto Combo_Tick() -> void;
	auto BasicTrade() -> void;
	auto Combo_Fast() -> void;
	auto Combo_Melee() -> void;
	auto Combo_Killable() -> void;
	auto Gather_Stacks() -> void;
	auto Combo_Gapcloser()-> void;
	float CastTick;
	ActiveModes ActiveMode;

	bool gap_closed;
	bool first_cast;
	AttackableUnit* gap_minion;
	AIHeroClient* gap_target;
	bool clear_gap = false;
};

extern std::unique_ptr<Irelia_Combo> pIreliaCombo;