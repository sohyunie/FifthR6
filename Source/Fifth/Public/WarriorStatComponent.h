// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "Components/ActorComponent.h"
#include "WarriorStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIFTH_API UWarriorStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWarriorStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

public:
	void SetNewLevel(int32 NewLevel);
	void SetDamage(float NewDamage);
	float GetAttack();
	float GetSAttack();

	FOnHPIsZeroDelegate OnHPIsZero;

	//UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SAttack, Meta = (AllowPrivateAccess = true))
		//bool SAttackCheck{ false };

private:
	struct FWarriorData* CurrentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
		int32 Level;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
		float CurrentHP;

	
};
