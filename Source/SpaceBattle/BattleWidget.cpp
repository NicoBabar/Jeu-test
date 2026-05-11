#include "BattleWidget.h"
#include "BattleGameMode.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"

void UBattleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// "White" is a 1x1 white texture in CoreStyle — tinting it gives solid colors
	const FSlateBrush* W = FCoreStyle::Get().GetBrush("White");

	auto ColorBrush = [W](FLinearColor C) -> FSlateBrush
	{
		FSlateBrush B = *W;
		B.TintColor = FSlateColor(C);
		return B;
	};

	// Root overlay — fills the viewport
	UOverlay* Root = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), FName("Root"));
	WidgetTree->RootWidget = Root;

	// Full-screen dark background
	UBorder* BgBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	BgBorder->Background = ColorBrush(FLinearColor(0.03f, 0.03f, 0.08f, 1.f));
	BgBorder->SetPadding(FMargin(0.f));
	{
		UOverlaySlot* S = Root->AddChildToOverlay(BgBorder);
		S->SetHorizontalAlignment(HAlign_Fill);
		S->SetVerticalAlignment(VAlign_Fill);
	}

	// Centered card — fixed 560x640
	USizeBox* CardBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName("CardBox"));
	CardBox->SetWidthOverride(560.f);
	CardBox->SetHeightOverride(640.f);
	{
		UOverlaySlot* S = Root->AddChildToOverlay(CardBox);
		S->SetHorizontalAlignment(HAlign_Center);
		S->SetVerticalAlignment(VAlign_Center);
	}

	UBorder* CardBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName("Card"));
	CardBorder->Background = ColorBrush(FLinearColor(0.08f, 0.08f, 0.12f, 1.f));
	CardBorder->SetPadding(FMargin(24.f));
	CardBox->SetContent(CardBorder);

	UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	CardBorder->SetContent(VBox);

	// Font helpers
	const FSlateFontInfo F16 = FCoreStyle::GetDefaultFontStyle("Bold", 16);
	const FSlateFontInfo F13 = FCoreStyle::GetDefaultFontStyle("Bold", 13);
	const FSlateFontInfo F12 = FCoreStyle::GetDefaultFontStyle("Bold", 12);
	const FSlateFontInfo F11 = FCoreStyle::GetDefaultFontStyle("Regular", 11);

	// HP bar style builder
	auto MakeBarStyle = [&](FLinearColor Fill) -> FProgressBarStyle
	{
		FProgressBarStyle Style;
		Style.BackgroundImage = ColorBrush(FLinearColor(0.12f, 0.12f, 0.12f, 1.f));
		Style.FillImage       = ColorBrush(Fill);
		Style.MarqueeImage    = ColorBrush(FLinearColor::Transparent);
		return Style;
	};

	// Button style builder
	auto MakeBtnStyle = [&](FLinearColor Base) -> FButtonStyle
	{
		FButtonStyle Style;
		Style.Normal   = ColorBrush(Base);
		Style.Hovered  = ColorBrush(FLinearColor(Base.R + 0.1f, Base.G + 0.1f, Base.B + 0.1f, 1.f));
		Style.Pressed  = ColorBrush(FLinearColor(Base.R - 0.1f, Base.G - 0.1f, Base.B - 0.1f, 1.f));
		Style.Disabled = ColorBrush(FLinearColor(0.25f, 0.25f, 0.25f, 0.6f));
		Style.NormalPadding  = FMargin(12.f, 8.f);
		Style.PressedPadding = FMargin(13.f, 9.f, 11.f, 7.f);
		return Style;
	};

	// Label helper: adds a text block directly to VBox
	auto AddLabel = [&](const FString& Str, FLinearColor Col, const FSlateFontInfo& Font, FMargin Pad)
	{
		UTextBlock* T = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		T->SetText(FText::FromString(Str));
		T->SetColorAndOpacity(FSlateColor(Col));
		T->SetFont(Font);
		T->SetJustification(ETextJustify::Center);
		UVerticalBoxSlot* S = VBox->AddChildToVerticalBox(T);
		S->SetPadding(Pad);
		S->SetHorizontalAlignment(HAlign_Fill);
	};

	// ---- ENEMY SECTION ----
	AddLabel(TEXT("ENNEMI"), FLinearColor(0.9f, 0.3f, 0.3f, 1.f), F16, FMargin(0.f, 0.f, 0.f, 8.f));

	EnemyHPBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), FName("EnemyHP"));
	EnemyHPBar->SetPercent(1.f);
	EnemyHPBar->SetWidgetStyle(MakeBarStyle(FLinearColor(0.85f, 0.1f, 0.1f, 1.f)));
	{
		UVerticalBoxSlot* S = VBox->AddChildToVerticalBox(EnemyHPBar);
		S->SetPadding(FMargin(0.f, 0.f, 0.f, 18.f));
		S->SetHorizontalAlignment(HAlign_Fill);
	}

	// ---- LOG SECTION ----
	AddLabel(TEXT("[ Journal de combat ]"), FLinearColor(0.55f, 0.55f, 0.55f, 1.f), F12, FMargin(0.f, 0.f, 0.f, 4.f));

	LogTextBlocks.Empty();
	for (int32 i = 0; i < 6; ++i)
	{
		UTextBlock* LT = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		LT->SetText(FText::FromString(TEXT(" ")));
		LT->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f, 1.f)));
		LT->SetFont(F11);
		LT->SetJustification(ETextJustify::Left);
		UVerticalBoxSlot* S = VBox->AddChildToVerticalBox(LT);
		S->SetPadding(FMargin(4.f, 1.f));
		S->SetHorizontalAlignment(HAlign_Fill);
		LogTextBlocks.Add(LT);
	}

	// ---- PLAYER SECTION ----
	AddLabel(TEXT("JOUEUR"), FLinearColor(0.3f, 0.85f, 0.3f, 1.f), F16, FMargin(0.f, 18.f, 0.f, 8.f));

	PlayerHPBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), FName("PlayerHP"));
	PlayerHPBar->SetPercent(1.f);
	PlayerHPBar->SetWidgetStyle(MakeBarStyle(FLinearColor(0.1f, 0.85f, 0.1f, 1.f)));
	{
		UVerticalBoxSlot* S = VBox->AddChildToVerticalBox(PlayerHPBar);
		S->SetPadding(FMargin(0.f, 0.f, 0.f, 22.f));
		S->SetHorizontalAlignment(HAlign_Fill);
	}

	// ---- BUTTONS ----
	UHorizontalBox* BtnRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	{
		UVerticalBoxSlot* S = VBox->AddChildToVerticalBox(BtnRow);
		S->SetHorizontalAlignment(HAlign_Center);
	}

	auto MakeBtn = [&](const FString& Label, FLinearColor Color) -> UButton*
	{
		UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		Btn->SetStyle(MakeBtnStyle(Color));

		UTextBlock* Lbl = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Lbl->SetText(FText::FromString(Label));
		Lbl->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Lbl->SetFont(F13);
		Btn->SetContent(Lbl);

		UHorizontalBoxSlot* HS = BtnRow->AddChildToHorizontalBox(Btn);
		HS->SetPadding(FMargin(0.f, 0.f, 12.f, 0.f));
		return Btn;
	};

	AttackButton = MakeBtn(TEXT("Attaquer"), FLinearColor(0.15f, 0.35f, 0.75f, 1.f));
	AttackButton->OnClicked.AddDynamic(this, &UBattleWidget::OnAttackClicked);

	MissileButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), FName("MissileBtn"));
	MissileButton->SetStyle(MakeBtnStyle(FLinearColor(0.70f, 0.30f, 0.05f, 1.f)));
	MissileLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName("MissileLabel"));
	MissileLabel->SetText(FText::FromString(TEXT("Missile (x2)")));
	MissileLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	MissileLabel->SetFont(F13);
	MissileButton->SetContent(MissileLabel);
	MissileButton->OnClicked.AddDynamic(this, &UBattleWidget::OnMissileClicked);
	BtnRow->AddChildToHorizontalBox(MissileButton)->SetPadding(FMargin(0.f));

	// ---- RESULT OVERLAY (hidden until end) ----
	ResultOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), FName("ResultOverlay"));
	ResultOverlay->SetVisibility(ESlateVisibility::Collapsed);
	{
		UOverlaySlot* S = Root->AddChildToOverlay(ResultOverlay);
		S->SetHorizontalAlignment(HAlign_Fill);
		S->SetVerticalAlignment(VAlign_Fill);
	}

	UBorder* ResultBg = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	ResultBg->Background = ColorBrush(FLinearColor(0.f, 0.f, 0.f, 0.80f));
	ResultBg->SetPadding(FMargin(0.f));
	{
		UOverlaySlot* S = ResultOverlay->AddChildToOverlay(ResultBg);
		S->SetHorizontalAlignment(HAlign_Fill);
		S->SetVerticalAlignment(VAlign_Fill);
	}

	ResultText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName("ResultText"));
	ResultText->SetText(FText::GetEmpty());
	ResultText->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.9f, 0.1f, 1.f)));
	ResultText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 32));
	ResultText->SetJustification(ETextJustify::Center);
	{
		UOverlaySlot* S = ResultOverlay->AddChildToOverlay(ResultText);
		S->SetHorizontalAlignment(HAlign_Center);
		S->SetVerticalAlignment(VAlign_Center);
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
		MissileLabel->SetText(FText::FromString(FString::Printf(TEXT("Missile (x%d)"), Charges)));
}

void UBattleWidget::OnAttackClicked()
{
	if (ABattleGameMode* GM = Cast<ABattleGameMode>(GetWorld()->GetAuthGameMode()))
		GM->PlayerAttack();
}

void UBattleWidget::OnMissileClicked()
{
	if (ABattleGameMode* GM = Cast<ABattleGameMode>(GetWorld()->GetAuthGameMode()))
		GM->PlayerMissile();
}
