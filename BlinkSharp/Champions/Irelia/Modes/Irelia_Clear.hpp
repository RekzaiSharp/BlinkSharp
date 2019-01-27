#pragma once
#include <memory>
#include <vector>
#include "../BlinkSharp/Include/SDK Extensions.h"

class Irelia_Clear
{
private:
	std::vector<AIBaseClient*> Clear_Targets;
public:
	auto Clear_Tick() -> void;
	auto Clear_Basic() -> void;
	auto Draw_Clear() -> void;
};

extern std::unique_ptr<Irelia_Clear> pIreliaClear;