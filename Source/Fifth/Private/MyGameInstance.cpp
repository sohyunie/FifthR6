// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"


UMyGameInstance::UMyGameInstance()
{
	FString WarriorDataPath = TEXT("/Game/GameData/WarriorData.WarriorData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_WARRIOR(*WarriorDataPath);
	ABCHECK(DT_WARRIOR.Succeeded());
	WarriorTable = DT_WARRIOR.Object;
	ABCHECK(WarriorTable->GetRowMap().Num() > 0);

	FString BossTankDataPath = TEXT("/Game/GameData/BossTankData.BossTankData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_BOSSTANK(*BossTankDataPath);
	ABCHECK(DT_BOSSTANK.Succeeded());
	BossTankTable = DT_BOSSTANK.Object;
	ABCHECK(BossTankTable->GetRowMap().Num() > 0);

	FString ShadowDataPath = TEXT("/Game/GameData/ShadowData.ShadowData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_SHADOW(*ShadowDataPath);
	ABCHECK(DT_SHADOW.Succeeded());
	ShadowTable = DT_SHADOW.Object;
	ABCHECK(ShadowTable->GetRowMap().Num() > 0);

	FString TankDataPath = TEXT("/Game/GameData/TankData.TankData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_TANK(*TankDataPath);
	ABCHECK(DT_TANK.Succeeded());
	TankTable = DT_TANK.Object;
	ABCHECK(TankTable->GetRowMap().Num() > 0);

	FString ManDataPath = TEXT("/Game/GameData/ManData.ManData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_MAN(*ManDataPath);
	ABCHECK(DT_MAN.Succeeded());
	ManTable = DT_MAN.Object;
	ABCHECK(ManTable->GetRowMap().Num() > 0);
}

void UMyGameInstance::Init()
{
	Super::Init();
	ABLOG(Warning, TEXT("SAttack of Level 5 Warrior : %f"),
		GetWarriorData(5)->SAttack);

	Super::Init();
	ABLOG(Warning, TEXT("Skill of Level 5 BossTank : %f"),
		GetBossTankData(5)->Skill);

	Super::Init();
	ABLOG(Warning, TEXT("Attack of Level 5 Shadow : %f"),
		GetShadowData(5)->Attack);

	Super::Init();
	ABLOG(Warning, TEXT("MaxHP of Level 6 Tank : %f"),
		GetTankData(6)->MaxHP);

	Super::Init();
	ABLOG(Warning, TEXT("Attack of Level 1 Man : %f"),
		GetManData(1)->Attack);

}

FWarriorData* UMyGameInstance::GetWarriorData(int32 Level)
{
	return WarriorTable->FindRow<FWarriorData>(*FString::FromInt(Level), TEXT(""));
}

FBossTankData* UMyGameInstance::GetBossTankData(int32 Level)
{
	return BossTankTable->FindRow<FBossTankData>(*FString::FromInt(Level), TEXT(""));
}

FShadowData* UMyGameInstance::GetShadowData(int32 Level)
{
	return ShadowTable->FindRow<FShadowData>(*FString::FromInt(Level), TEXT(""));
}

FTankData* UMyGameInstance::GetTankData(int32 Level)
{
	return TankTable->FindRow<FTankData>(*FString::FromInt(Level), TEXT(""));
}

FManData* UMyGameInstance::GetManData(int32 Level)
{
	return ManTable->FindRow<FManData>(*FString::FromInt(Level), TEXT(""));
}

