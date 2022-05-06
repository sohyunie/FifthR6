// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "AIController.h"
#include "TankAIController.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API ATankAIController : public AAIController
{
	GENERATED_BODY()

public:
	ATankAIController();
	virtual void OnPossess(APawn* InPawn) override;

	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;
	static const FName SelectAttackNumberKey;

	void RunAI();
	void StopAI();

private:
	UPROPERTY()
		class UBlackboardData* BBAsset;

	UPROPERTY()
		class UBehaviorTree* BTAsset;
	
};
