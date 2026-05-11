#include "BattleGameMode.h"
#include "SpaceShip.h"
#include "BattleWidget.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

ABattleGameMode::ABattleGameMode()
	: bBattleOver(false)
{
}

void ABattleGameMode::BeginPlay()
{
	Super::BeginPlay();

	// --- Create ships ---
	PlayerShip = NewObject<USpaceShip>(this);
	PlayerShip->HP             = 100;
	PlayerShip->MaxHP          = 100;
	PlayerShip->AttackMin      = 8;
	PlayerShip->AttackMax      = 18;
	PlayerShip->MissileCharges = 2;
	PlayerShip->MissileDamageMin = 20;
	PlayerShip->MissileDamageMax = 35;

	EnemyShip = NewObject<USpaceShip>(this);
	EnemyShip->HP             = 80;
	EnemyShip->MaxHP          = 80;
	EnemyShip->AttackMin      = 6;
	EnemyShip->AttackMax      = 15;
	EnemyShip->MissileCharges = 2;
	EnemyShip->MissileDamageMin = 18;
	EnemyShip->MissileDamageMax = 30;

	// --- Create and show HUD widget ---
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		BattleWidget = CreateWidget<UBattleWidget>(PC, UBattleWidget::StaticClass());
		if (BattleWidget)
		{
			BattleWidget->AddToViewport();
			BattleWidget->RefreshHP(PlayerShip->GetHPPercent(), EnemyShip->GetHPPercent());
		}

		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}

	AddLog(TEXT("Combat spatial commencé !"));
	AddLog(TEXT("Choisissez votre action."));
}

void ABattleGameMode::PlayerAttack()
{
	if (bBattleOver || !PlayerShip || !EnemyShip) return;

	if (BattleWidget) BattleWidget->SetButtonsEnabled(false);

	const int32 Dmg = PlayerShip->RollAttack();
	EnemyShip->TakeDamage(Dmg);
	AddLog(FString::Printf(TEXT("Vous attaquez : -%d PV à l'ennemi."), Dmg));

	if (BattleWidget) BattleWidget->RefreshHP(PlayerShip->GetHPPercent(), EnemyShip->GetHPPercent());

	if (!EnemyShip->IsAlive())
	{
		EndBattle(true);
		return;
	}

	GetWorldTimerManager().SetTimer(EnemyTurnTimer, this, &ABattleGameMode::ExecuteEnemyTurn, 1.5f, false);
}

void ABattleGameMode::PlayerMissile()
{
	if (bBattleOver || !PlayerShip || !EnemyShip) return;
	if (PlayerShip->MissileCharges <= 0) return;

	if (BattleWidget) BattleWidget->SetButtonsEnabled(false);

	PlayerShip->MissileCharges--;
	const int32 Dmg = PlayerShip->RollMissile();
	EnemyShip->TakeDamage(Dmg);
	AddLog(FString::Printf(TEXT("Vous tirez un missile : -%d PV à l'ennemi !"), Dmg));

	if (BattleWidget)
	{
		BattleWidget->RefreshHP(PlayerShip->GetHPPercent(), EnemyShip->GetHPPercent());
		BattleWidget->UpdateMissileLabel(PlayerShip->MissileCharges);
	}

	if (!EnemyShip->IsAlive())
	{
		EndBattle(true);
		return;
	}

	GetWorldTimerManager().SetTimer(EnemyTurnTimer, this, &ABattleGameMode::ExecuteEnemyTurn, 1.5f, false);
}

void ABattleGameMode::ExecuteEnemyTurn()
{
	if (bBattleOver || !PlayerShip || !EnemyShip) return;

	int32 Dmg = 0;
	FString ActionMsg;

	// AI: use missile when HP below 50% and charges remain
	if (EnemyShip->GetHPPercent() < 0.5f && EnemyShip->MissileCharges > 0)
	{
		EnemyShip->MissileCharges--;
		Dmg = EnemyShip->RollMissile();
		ActionMsg = FString::Printf(TEXT("L'ennemi tire un missile : -%d PV !"), Dmg);
	}
	else
	{
		Dmg = EnemyShip->RollAttack();
		ActionMsg = FString::Printf(TEXT("L'ennemi attaque : -%d PV."), Dmg);
	}

	PlayerShip->TakeDamage(Dmg);
	AddLog(ActionMsg);

	if (BattleWidget) BattleWidget->RefreshHP(PlayerShip->GetHPPercent(), EnemyShip->GetHPPercent());

	if (!PlayerShip->IsAlive())
	{
		EndBattle(false);
		return;
	}

	// Restore buttons; disable missile if charges exhausted
	if (BattleWidget)
	{
		BattleWidget->SetButtonsEnabled(true);
		if (PlayerShip->MissileCharges <= 0)
		{
			BattleWidget->SetMissileButtonEnabled(false);
		}
	}
}

void ABattleGameMode::AddLog(const FString& Message)
{
	LogLines.Add(Message);
	while (LogLines.Num() > 6)
	{
		LogLines.RemoveAt(0);
	}

	if (BattleWidget)
	{
		BattleWidget->RefreshLog(LogLines);
	}
}

void ABattleGameMode::EndBattle(bool bPlayerWon)
{
	bBattleOver = true;

	AddLog(bPlayerWon
		? TEXT("Vous avez détruit le vaisseau ennemi !")
		: TEXT("Votre vaisseau est détruit..."));

	if (BattleWidget)
	{
		BattleWidget->SetButtonsEnabled(false);
		BattleWidget->ShowResult(bPlayerWon ? TEXT("VICTOIRE !") : TEXT("DÉFAITE..."));
	}
}
