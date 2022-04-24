// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "WarriorStatComponent.h"
#include "MyPlayerState.h"

void UMyHUDWidget::BindCharacterStat(UWarriorStatComponent* CharacterStat)
{
	ABCHECK(nullptr != CharacterStat);
	CurrentCharacterStat = CharacterStat;
	CharacterStat->OnHPChanged.AddUObject(this, &UMyHUDWidget::UpdateCharacterStat);
}

void UMyHUDWidget::BindPlayerState(AMyPlayerState* PlayerState)
{
	ABCHECK(nullptr != PlayerState);
	CurrentPlayerState = PlayerState;
	PlayerState->OnPlayerStateChanged.AddUObject(this, &UMyHUDWidget::UpdatePlayerState);
	//ABLOG(Warning, TEXT("BindPlayerState"));
}

void UMyHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbHP")));
	ABCHECK(nullptr != HPBar);

	SPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbSP")));
	ABCHECK(nullptr != SPBar);

	PlayerName = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtPlayerName")));
	ABCHECK(nullptr != PlayerName);

	PlayerLevel = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtLevel")));
	ABCHECK(nullptr != PlayerLevel);

	CurrentScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtCurrentScore")));
	ABCHECK(nullptr != CurrentScore);

	HighScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtHighScore")));
	ABCHECK(nullptr != HighScore);

}

void UMyHUDWidget::UpdateCharacterStat()
{
	ABCHECK(CurrentCharacterStat.IsValid());

	HPBar->SetPercent(CurrentCharacterStat->GetHPRatio());
}

void UMyHUDWidget::UpdatePlayerState()
{
	ABCHECK(CurrentPlayerState.IsValid());
	
	//FString PlayerNameText = FString::Printf(TEXT("Elsa"));
	//PlayerName->SetText(FText::FromString(PlayerNameText));

	
	//PlayerName->SetText(FText::FromString(CurrentPlayerState->GetPlayerName()));
	//ABCHECK(nullptr != PlayerName);
	//PlayerLevel->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetCharacterLevel())));
    //CurrentScore->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetGameScore())));
}

