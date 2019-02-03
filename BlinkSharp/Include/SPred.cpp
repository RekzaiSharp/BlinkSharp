#include "SPred.h"
#include <cmath>

auto SPred::GetDirectionDifferencePerc(Vector3 dir1, Vector3 dir2) -> float
{
	return Vector3(dir1.Normalized() * 100 - dir2.Normalized() * 100).Length() / 2.f;
}

auto SPred::GetBaitLevel(AIHeroClient* unit)->float
{
	auto baitlevel = 0.f;
	//TODO this
	return baitlevel;
}

auto SPred::VectorMovementCollision(_In_ Vector3 startPoint1, _In_ Vector3 endPoint1, _In_ float v1, _In_ Vector3 startPoint2, _In_ float v2, _In_ float delay,
	_Out_ float t1, _Out_ Vector2 p1, _Out_ float t2, _Out_ Vector2 p2)
{
	float sP1x = startPoint1.x, sP1y = startPoint1.z or startPoint1.y, eP1x = endPoint1.x, eP1y = endPoint1.z or endPoint1.y, sP2x = startPoint2.x, sP2y = startPoint2.z or startPoint2.y;
	float d = eP1x - sP1x, e = eP1y - sP1y;
	float dist = std::sqrt(d * d + e * e);
	t1 = INFINITY, t2 = INFINITY;
	float S = 0.f, K = 0.f;
	if (dist != 0.f)
		S = v1 * d / dist, K = v1 * e / dist;
	if (delay != 0.f)
		sP1x = sP1x + S * delay, sP1y = sP1y + K * delay;
	float r = sP2x = sP1x, j = sP2y - sP1y;
	auto c = r * r + j * j;
	if (dist > 0.f) {
		if (v1 == HUGE_VALF) {
			auto t = dist / v1;
			if (v2*t >= 0.f) {
				t1 = t;
			}
		}
		else if (v2 == HUGE_VALF) {
			t1 = 0;
		}
		else {
			auto a = S * S + K * K - v2 * v2;
			auto b = -r * S - j * K;
			if (a == 0.f) {
				if (b == 0.f) {
					if (c == 0.f) {
						t1 = 0.f;
					}
				}
				else {
					auto t = -c / (2.f * b);
					if (v2*t >= 0.f) {
						t1 = t;
					}
				}
			}
			else {
				auto sqr = b * b - a * c;
				if (sqr >= 0.f) {
					auto nom = std::sqrt(sqr);
					auto t = (-nom - b) / a;
					if (v2*t >= 0.f) {
						t1 = t;
					}
					t = (nom - b) / a;
					if (v2*t >= 0.f) {
						t2 = t;
					}
				}
			}
		}
	}
	else if(dist==0){
		t1 = 0;
	}
	p1 = GetCollisionPoint(t1, sP1x, sP1y, S, K);
	p2 = GetCollisionPoint(t2, sP1x, sP1y, S, K);
}

auto SPred::GetCollisionPoint(float t, float sP1x, float sP1y, float S, float K) -> Vector2
{
	return Vector2(sP1x + S * t, sP1y + K * t);
}

auto SPred::PredictPos(AIHeroClient* target, float delay, AIHeroClient* source, float speed, float hitbox)->Vector3 
{
	auto dir = target->GetPosition().DirectionVector;
	Vector3 pos;
	hitbox = target->GetBoundingRadius();
	if (dir.IsValid() && target->IsMoving()) {
		if (speed != 0.f) {
			float t1, t2;
			Vector2 p1, p2;
			VectorMovementCollision(target->GetPosition(), target->GetPosition() + dir * ((source->Distance(target) / speed + delay)*target->GetMovementSpeed()), target->GetMovementSpeed(), Player.GetPosition(), speed, delay, t1, p1, t2, p2);
			if (t1 == INFINITY && t1 <= 0.f && t2 >= 0.f && t2 != INFINITY) {
				pos = Vector3(p2.x, target->GetPosition().y, p2.y);
			}
			else if (t2 <= 0.f && t2 == INFINITY && t1 >= 0.f && t1 != INFINITY) {
				pos = Vector3(p1.x, target->GetPosition().y, p1.y);
			}
			else if (t2 >= 0.f && t2 != INFINITY && t1 >= 0.f && t1 != INFINITY) {
				auto t = min(t1, t2);
				if (t == t1) {
					pos = Vector3(p1.x, target->GetPosition().y, p1.y);
				}
				else {
					pos = Vector3(p2.x, target->GetPosition().y, p2.y);
				}
			}
			else {
				pos = target->GetPosition() + dir.Normalized()*(source->Distance(target)/speed+delay)*target->GetMovementSpeed();
			}
		}
		else {
			pos = target->GetPosition() + dir.Normalized()*delay*target->GetMovementSpeed();
		}
	}
	else if (!target->IsMoving()) {
		pos = target->GetPosition();
	}
	if (pos.IsValid()) {
		auto baitlevel = GetBaitLevel(target)/100;
		if (target->IsMoving() && baitlevel > 0 && baitlevel < 200) {
			pos = pos + (target->GetPosition() - pos) * baitlevel;
		}
		return pos;
	}
}

std::unique_ptr<SPred> pSPred = std::make_unique<SPred>();