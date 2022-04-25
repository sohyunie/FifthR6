// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "Animation/AnimInstance.h"
#include "BossTankAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnKickHitCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnSkillHitCheckDelegate);

/**
 * 
 */
UCLASS()
class FIFTH_API UBossTankAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UBossTankAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage();
	void PlayKickMontage();
	void PlayDamagedMontage();
	void PlaySkillMontage();
	void PlaySpecialMontage();
	void PlayScreamMontage();

	void SetDeadAnim() { IsDead = true; }

	FOnAttackHitCheckDelegate OnAttackHitCheck;
	FOnKickHitCheckDelegate OnKickHitCheck;
	FOnSkillHitCheckDelegate OnSkillHitCheck;

private:
	UFUNCTION()
		void AnimNotify_AttackHitCheck();

	UFUNCTION()
		void AnimNotify_KickHitCheck();

	UFUNCTION()
		void AnimNotify_SkillHitCheck();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
		float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* KickMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* DamagedMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* SkillMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* SpecialMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* ScreamMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
		bool IsDead;
	
};
