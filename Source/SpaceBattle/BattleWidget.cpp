#include "BattleWidget.h"
#include "BattleGameMode.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"

void UBattleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), FName("Root"));
	WidgetTree->RootWidget = Root;

	// Full-screen dark background
	UImage* Bg = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	Bg->SetColorAndOpacity(FLinearColor(0.03f, 0.03f, 0.08f, 1.f));
	{
		UCanvasPanelSlot* S = Root->AddChildToCanvas(Bg);
		S->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		S->SetOffsets(FMargin(0.f));
	}

	// Centered card (560x640)
	UBorder* Card = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName("Card"));
	Card->SetBrushColor(FLinearColor(0.08f, 0.08f, 0.12f, 1.f));
	Card->SetPadding(FMargin(24.f));
	{
		UCanvasPanelSlot* S = Root->AddChildToCanvas(Card);
		S->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
		S->SetAlignment(FVector2D(0.5f, 0.5f));
		S->SetSize(FVector2D(560.f, 640.f));
	}

	UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	Card->SetContent(VBox);

	const FSlateFontInfo Font16Bold = FCoreStyle::GetDefaultFontStyle("Bold", 16);
	const FSlateFontInfo Font13Bold = FCoreStyle::GetDefaultFontStyle("Bold", 13);
	const FSlateFontInfo Font12Bold = FCoreStyle::GetDefaultFontStyle("Bold", 12);
	const FSlateFontInfo Font11Reg  = FCoreStyle::GetDefaultFontStyle("Regular", 11);
	const FSlateFontInfo Font32Bold = FCoreStyle::GetDefaultFontStyle("Bold", 32);

	// ---- ENEMY SECTION ----
	{
		UTextBlock* T = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		T->SetText(FText::FromString(TEXT("ENNEMI")));
		T->SetColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.3f, 0.3f, 1.f)));
		T->SetFont(Font16Bold);
		T->SetJustification(ETextJustify::Center);
		UVerticalBoxSlot* S = VBox->AddChildToVerticalBox(T);
		S->SetPadding(FMargin(0.f, 0.f, 0.f, 8.f));
		S->SetHorizontalAlignment(HAlign_Fill);
	}

	EnemyHPBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), FName("EnemyHPBar"));
	EnemyHPBar->SetPercent(1.f);
	{
		FProgressBarStyle Style = FCoreStyle::Get().GetWidgetStyle<FProgressBarStyle>("ProgressBar");
		Style.FillImage.TintColor = FSlateColor(FLinearColor(0.85f, 0.1f, 0.1f, 1.f));
		EnemyHPBar->SetWidgetStyle(Style);
		UVerticalBoxSlot* S = VBox->AddChildToVerticalBox(EnemyHPBar);
		S->SetPadding(FMargin(0.f, 0.f, 0.f, 20.f));
		S->SetHorizontalAlignment(HAlign_Fill);
	}

	// ---- LOG SECTION ----
	{
		UTextBlock* T = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		T->SetText(FText::FromString(TEXT("[ Journal de combat ]")));
		T->SetColorAndOpacity(FSlateColor(FLinearColor(0.55f, 0.55f, 0.55f, 1.f)));
		T->SetFont(Font12Bold);
		T->SetJustification(ETextJustify::Center);
		UVerticalBoxSlot* S = VBox->AddChildToVerticalBox(T);
		S->SetPadding(FMargin(0.f, 0.f, 0.f, 4.f));
		S->SetHorizontalAlignment(HAlign_Fill);
	}

	LogTextBlocks.Empty();
	for (int32 i = 0; i < 6; ++i)
	{
		UTextBlock* LT = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		LT->SetText(FText::FromString(TEXT(" ")));
		LT->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f, 1.f)));
		LT->SetFont(Font11Reg);
		LT->SetJustification(ETextJustify::Left);
		UVerticalBoxSlot* S = VBox->AddChildToVerticalBox(LT);
		S->SetPadding(FMargin(4.f, 1.f));
		S->SetHorizontalAlignment(HAlign_Fill);
		LogTextBlocks.Add(LT);
	}

	// ---- PLAYER SECTION ----
	{
		UTextBlock* T = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		T->SetText(FText::FromString(TEXT("JOUEUR")));
		T->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 0.85f, 0.3f, 1.f)));
		T->SetFont(Font16Bold);
		T->SetJustification(ETextJustify::Center);
		UVerticalBoxSlot* S = VBox->AddChildToVerticalBox(T);
		S->SetPadding(FMargin(0.f, 20.f, 0.f, 8.f));
		S->SetHorizontalAlignment(HAlign_Fill);
	}

	PlayerHPBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), FName("PlayerHPBar"));
	PlayerHPBar->SetPercent(1.f);
	{
		FProgressBarStyle Style = FCoreStyle::Get().GetWidgetStyle<FProgressBarStyle>("ProgressBar");
		Style.FillImage.TintColor = FSlateColor(FLinearColor(0.1f, 0.85f, 0.1f, 1.f));
		PlayerHPBar->SetWidgetStyle(Style);
		UVerticalBoxSlot* S = VBox->AddChildToVerticalBox(PlayerHPBar);
		S->SetPadding(FMargin(0.f, 0.f, 0.f, 24.f));
		S->SetHorizontalAlignment(HAlign_Fill);
	}

	// ---- BUTTON ROW ----
	UHorizontalBox* ButtonRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	{
		UVerticalBoxSlot* S = VBox->AddChildToVerticalBox(ButtonRow);
		S->SetHorizontalAlignment(HAlign_Center);
		S->SetPadding(FMargin(0.f));
	}

	// Attack button (blue)
	AttackButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), FName("AttackBtn"));
	{
		FButtonStyle BS = FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button");
		BS.Normal.TintColor  = FSlateColor(FLinearColor(0.15f, 0.35f, 0.75f, 1.f));
		BS.Hovered.TintColor = FSlateColor(FLinearColor(0.25f, 0.45f, 0.85f, 1.f));
		BS.Pressed.TintColor = FSlateColor(FLinearColor(0.10f, 0.25f, 0.60f, 1.f));
		AttackButton->SetStyle(BS);

		UTextBlock* Lbl = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Lbl->SetText(FText::FromString(TEXT("Attaquer")));
		Lbl->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Lbl->SetFont(Font13Bold);
		AttackButton->SetContent(Lbl);
	}
	AttackButton->OnClicked.AddDynamic(this, &UBattleWidget::OnAttackClicked);
	{
		UHorizontalBoxSlot* HS = ButtonRow->AddChildToHorizontalBox(AttackButton);
		HS->SetPadding(FMargin(0.f, 0.f, 12.f, 0.f));
	}

	// Missile button (orange)
	MissileButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), FName("MissileBtn"));
	{
		FButtonStyle BS = FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button");
		BS.Normal.TintColor  = FSlateColor(FLinearColor(0.70f, 0.30f, 0.05f, 1.f));
		BS.Hovered.TintColor = FSlateColor(FLinearColor(0.85f, 0.40f, 0.10f, 1.f));
		BS.Pressed.TintColor = FSlateColor(FLinearColor(0.55f, 0.20f, 0.00f, 1.f));
		MissileButton->SetStyle(BS);

		MissileLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName("MissileLabel"));
		MissileLabel->SetText(FText::FromString(TEXT("Missile (x2)")));
		MissileLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		MissileLabel->SetFont(Font13Bold);
		MissileButton->SetContent(MissileLabel);
	}
	MissileButton->OnClicked.AddDynamic(this, &UBattleWidget::OnMissileClicked);
	{
		UHorizontalBoxSlot* HS = ButtonRow->AddChildToHorizontalBox(MissileButton);
		HS->SetPadding(FMargin(0.f));
	}

	// ---- RESULT OVERLAY (hidden until battle ends) ----
	ResultOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), FName("ResultOverlay"));
	ResultOverlay->SetVisibility(ESlateVisibility::Collapsed);
	{
		UCanvasPanelSlot* CS = Root->AddChildToCanvas(ResultOverlay);
		CS->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		CS->SetOffsets(FMargin(0.f));
	}

	UImage* ResultBg = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	ResultBg->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.80f));
	{
		UOverlaySlot* OS = ResultOverlay->AddChildToOverlay(ResultBg);
		OS->SetHorizontalAlignment(HAlign_Fill);
		OS->SetVerticalAlignment(VAlign_Fill);
	}

	ResultText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName("ResultText"));
	ResultText->SetText(FText::GetEmpty());
	ResultText->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.9f, 0.1f, 1.f)));
	ResultText->SetFont(Font32Bold);
	ResultText->SetJustification(ETextJustify::Center);
	{
		UOverlaySlot* OS = ResultOverlay->AddChildToOverlay(ResultText);
		OS->SetHorizontalAlignment(HAlign_Center);
		OS->SetVerticalAlignment(VAlign_Center);
	}
}

void UBattleWidget::RefreshHP(float PlayerPct, float EnemyPct)
{
	if (PlayerHPBar) PlayerHPBar->SetPercent(PlayerPct);
	if (EnemyHPBar)  EnemyHPBar->SetPercent(EnemyPct);
}

void UBattleWidget::RefreshLog(const TArray<FString>& Lines)
{
	for (int32 i = 0; i < LogTextBlocks.Num(); ++i)
	{
		if (!LogTextBlocks[i]) continue;
		LogTextBlocks[i]->SetText(
			i < Lines.Num() ? FText::FromString(Lines[i]) : FText::FromString(TEXT(" "))
		);
	}
}

void UBattleWidget::ShowResult(const FString& Result)
{
	if (ResultText)    ResultText->SetText(FText::FromString(Result));
	if (ResultOverlay) ResultOverlay->SetVisibility(ESlateVisibility::Visible);
}

void UBattleWidget::SetButtonsEnabled(bool bEnabled)
{
	if (AttackButton)  AttackButton->SetIsEnabled(bEnabled);
	if (MissileButton) MissileButton->SetIsEnabled(bEnabled);
}

void UBattleWidget::SetMissileButtonEnabled(bool bEnabled)
{
	if (MissileButton) MissileButton->SetIsEnabled(bEnabled);
}

void UBattleWidget::UpdateMissileLabel(int32 Charges)
{
	if (MissileLabel)
	{
		MissileLabel->SetText(FText::FromString(FString::Printf(TEXT("Missile (x%d)"), Charges)));
	}
}

void UBattleWidget::OnAttackClicked()
{
	if (ABattleGameMode* GM = Cast<ABattleGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->PlayerAttack();
	}
}

void UBattleWidget::OnMissileClicked()
{
	if (ABattleGameMode* GM = Cast<ABattleGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->PlayerMissile();
	}
}
