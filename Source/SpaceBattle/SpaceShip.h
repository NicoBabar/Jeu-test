#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SpaceShip.generated.h"

UCLASS()
class SPACEBATTLE_API USpaceShip : public UObject
{
	GENERATED_BODY()

public:
	USpaceShip();

	UPROPERTY()
	int32 HP;

	UPROPERTY()
	int32 MaxHP;

	UPROPERTY()
	int32 AttackMin;

	UPROPERTY()
	int32 AttackMax;

	UPROPERTY()
	int32 MissileCharges;

	UPROPERTY()
	int32 MissileDamageMin;

	UPROPERTY()
	int32 MissileDamageMax;

	void TakeDamage(int32 Amount);
	int32 RollAttack() const;
	int32 RollMissile() const;
	float GetHPPercent() const;
	bool IsAlive() const;
};
