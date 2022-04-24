// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnSAttackHitCheckDelegate);

/**
 * 
 */

UCLASS()
class FIFTH_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UMyAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage();
	void PlaySAttackMontage();

	void JumpToAttackMontageSection(int32 NewSection);

public:
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;
	FOnSAttackHitCheckDelegate OnSAttackHitCheck;
	void SetDeadAnim() { IsDead = true; }

private:
	UFUNCTION()
		void AnimNotify_AttackHitCheck();

	UFUNCTION()
		void AnimNotify_SAttackHitCheck();

	UFUNCTION()
		void AnimNotify_P_Sparks();

	UFUNCTION()
		void AnimNotify_NextAttackCheck();

	FName GetAttackMontageSectionName(int32 Section);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
		float CurrentPawnSpeed;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage*  AttackMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = SAttack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* SAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
		bool IsDead;
};
