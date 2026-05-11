#include "SpaceShip.h"

USpaceShip::USpaceShip()
{
	HP             = 100;
	MaxHP          = 100;
	AttackMin      = 8;
	AttackMax      = 18;
	MissileCharges = 2;
	MissileDamageMin = 20;
	MissileDamageMax = 35;
}

void USpaceShip::TakeDamage(int32 Amount)
{
	HP = FMath::Max(0, HP - Amount);
}

int32 USpaceShip::RollAttack() const
{
	return FMath::RandRange(AttackMin, AttackMax);
}

int32 USpaceShip::RollMissile() const
{
	return FMath::RandRange(MissileDamageMin, MissileDamageMax);
}

float USpaceShip::GetHPPercent() const
{
	if (MaxHP <= 0) return 0.f;
	return static_cast<float>(HP) / static_cast<float>(MaxHP);
}

bool USpaceShip::IsAlive() const
{
	return HP > 0;
}
