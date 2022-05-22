// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnIsCheckedDelegate);
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
	void PlayTAttackMontage();
	void PlayDamagedMontage();

	void JumpToAttackMontageSection(int32 NewSection);
	void JumpToTAttackMontageSection(int32 NewSection);

public:
	FOnIsCheckedDelegate OnIsChecked;
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;
	FOnNextAttackCheckDelegate OnTNextAttackCheck;
	FOnAttackHitCheckDelegate OnTAttackHitCheck;
	FOnIsCheckedDelegate OnTIsChecked;
	FOnSAttackHitCheckDelegate OnSAttackHitCheck;
	void SetDeadAnim() { IsDead = true; }

private:
	

	UFUNCTION()
		void AnimNotify_AttackHitCheck();

	UFUNCTION()
		void AnimNotify_SAttackHitCheck();

	UFUNCTION()
		void AnimNotify_TAttackHitCheck();

	UFUNCTION()
		void AnimNotify_NextAttackCheck();
	UFUNCTION()
		void AnimNotify_TNextAttackCheck();

	UFUNCTION()
		void AnimNotify_IsChecked();
	UFUNCTION()
		void AnimNotify_TIsChecked();

	FName GetAttackMontageSectionName(int32 Section);
	FName GetTAttackMontageSectionName(int32 Section);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
		bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
		float CurrentPawnSpeed;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage*  AttackMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = SAttack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* SAttackMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = TAttack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* TAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Damage, Meta = (AllowPrivateAccess = true))
		UAnimMontage* DamagedMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
		bool IsDead;
};
