#pragma once

#include "../../Include/SDK Extensions.h"

class Irelia {
private:
	static AttackableUnit CurrentTarget;
	static AttackableUnit* OrbTarget;
public:
	static void Init();

	static void	__cdecl	Tick(_In_ void* UserData);
	static void	__cdecl	Update(_In_ void* UserData);
	static void	__cdecl	Draw(_In_ void* UserData);
	static void __cdecl DrawMenu(_In_ void* UserData);
	static void __cdecl OnCreate(void* object, unsigned int* net_id, void* UserData);
	static void __cdecl OnDelete(void* object, unsigned int* net_id, void* UserData);

	static Spell::Targeted Q;
	static Spell::Active W;
	static Spell::Skillshot E;
	static Spell::Skillshot R;

	struct stun_blade
	{
		void* object;
		Vector3 position;
	};

	stun_blade active_blade;

	bool InFastCombo;
	int ComboPart = 0;

	//Menu
	bool combo_q = true;
	int q_mode = 1;
	bool combo_w = true;
	int w_mode = 0;
	bool combo_e = true;
	bool combo_r = true;
	int r_mode = 1;

	bool clear_q = true;
	float clear_qmana = 20.f;

	bool msc_stack = true;
	bool msc_turrets = true;

	bool drw_cd = true;
	bool drw_q = true;
	bool drw_q_e = true;
	bool draw_w = true;
	bool draw_e = true;
	bool draw_r = true;

	auto QDamage(bool minion) -> float;
};

extern std::unique_ptr<Irelia> pIrelia;