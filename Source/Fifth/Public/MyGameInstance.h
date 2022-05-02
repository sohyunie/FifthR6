// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "MyGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FWarriorData :public FTableRowBase
{
	GENERATED_BODY()

public:
	FWarriorData() : Level(1), MaxHP(200.0f), Attack(2.0f), SAttack(10.0f), Skill(40.0f){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float SAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float Skill;

};


USTRUCT(BlueprintType)
struct FTankData :public FTableRowBase
{
	GENERATED_BODY()

public:
	FTankData() : Level(1), MaxHP(12.0f), Attack(2.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float Attack;


};


USTRUCT(BlueprintType)
struct FShadowData :public FTableRowBase
{
	GENERATED_BODY()

public:
	FShadowData() : Level(1), MaxHP(200.0f), Attack(2.0f), SAttack(10.0f), Skill(40.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float SAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float Skill;

};


USTRUCT(BlueprintType)
struct FBossTankData :public FTableRowBase
{
	GENERATED_BODY()

public:
	FBossTankData() : Level(1), MaxHP(300.0f), Attack(3.0f), SAttack(15.0f), Skill(50.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float SAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float Skill;

};

USTRUCT(BlueprintType)
struct FManData :public FTableRowBase
{
	GENERATED_BODY()

public:
	FManData() : Level(1), MaxHP(100.0f), Attack(2.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float Attack;


};


/**
 * 
 */
UCLASS()
class FIFTH_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMyGameInstance();

	virtual void Init() override;
	FWarriorData* GetWarriorData(int32 Level);
	FBossTankData* GetBossTankData(int32 Level);
	FShadowData* GetShadowData(int32 Level);
	FTankData* GetTankData(int32 Level);
	FManData* GetManData(int32 Level);

	FStreamableManager StreamableManager;

	int uniqueMonsterID = 0;

private:
	UPROPERTY()
		class UDataTable* WarriorTable;
	UPROPERTY()
		class UDataTable* BossTankTable;
	UPROPERTY()
		class UDataTable* ShadowTable;
	UPROPERTY()
		class UDataTable* TankTable;
	UPROPERTY()
		class UDataTable* ManTable;
	
};
