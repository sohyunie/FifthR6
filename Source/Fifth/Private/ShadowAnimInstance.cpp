// Fill out your copyright notice in the Description page of Project Settings.


#include "ShadowAnimInstance.h"

UShadowAnimInstance::UShadowAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsDead = false;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(
		TEXT("/Game/MyCharacter/Animation/WarriorOfFire_OneHandSword_Combo.WarriorOfFire_OneHandSword_Combo"));
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}

void UShadowAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();

	if (!::IsValid(Pawn)) return;


	if (!IsDead)
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();

	}
}

void UShadowAnimInstance::PlayAttackMontage()
{
	ABCHECK(!IsDead);
	Montage_Play(AttackMontage, 1.0f);

}



void UShadowAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(!IsDead);
	ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("NewSection : %d "), NewSection));
}

void UShadowAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UShadowAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName UShadowAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Section : %d "), Section));
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}
