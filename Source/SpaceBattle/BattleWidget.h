#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UButton;
class UOverlay;

UCLASS()
class SPACEBATTLE_API UBattleWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UProgressBar* EnemyHPBar;

	UPROPERTY()
	UProgressBar* PlayerHPBar;

	UPROPERTY()
	TArray<UTextBlock*> LogTextBlocks;

	UPROPERTY()
	UButton* AttackButton;

	UPROPERTY()
	UButton* MissileButton;

	UPROPERTY()
	UTextBlock* MissileLabel;

	UPROPERTY()
	UTextBlock* ResultText;

	UPROPERTY()
	UOverlay* ResultOverlay;

	void RefreshHP(float PlayerPct, float EnemyPct);
	void RefreshLog(const TArray<FString>& Lines);
	void ShowResult(const FString& Result);
	void SetButtonsEnabled(bool bEnabled);
	void SetMissileButtonEnabled(bool bEnabled);
	void UpdateMissileLabel(int32 Charges);

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnAttackClicked();

	UFUNCTION()
	void OnMissileClicked();
};
