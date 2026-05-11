#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleGameMode.generated.h"

class USpaceShip;
class UBattleWidget;

UCLASS()
class SPACEBATTLE_API ABattleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABattleGameMode();

	UPROPERTY()
	USpaceShip* PlayerShip;

	UPROPERTY()
	USpaceShip* EnemyShip;

	UPROPERTY()
	UBattleWidget* BattleWidget;

	void PlayerAttack();
	void PlayerMissile();
	void AddLog(const FString& Message);

protected:
	virtual void BeginPlay() override;

private:
	void ExecuteEnemyTurn();
	void EndBattle(bool bPlayerWon);

	bool bBattleOver;
	FTimerHandle EnemyTurnTimer;
	TArray<FString> LogLines;
};
