// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"

UMyAnimInstance::UMyAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsDead = false;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(
		TEXT("/Game/MyCharacter/Animation/WarriorOfFire_OneHandSword_Combo.WarriorOfFire_OneHandSword_Combo"));
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		SATTACK_MONTAGE(TEXT("/Game/MyCharacter/Animation/WarriorOfFire_SAttack.WarriorOfFire_SAttack"));
	if (SATTACK_MONTAGE.Succeeded())
	{
		SAttackMontage = SATTACK_MONTAGE.Object;
	}
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();

	if (!::IsValid(Pawn)) return;


	if (!IsDead)
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		
	}
}

void UMyAnimInstance::PlayAttackMontage()
{
	ABCHECK(!IsDead);
	Montage_Play(AttackMontage, 1.0f);
	
}

void UMyAnimInstance::PlaySAttackMontage()
{
	ABCHECK(!IsDead);
	Montage_Play(SAttackMontage, 1.0f);

}



void UMyAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(!IsDead);
	ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("NewSection : %d "), NewSection));
}

void UMyAnimInstance::AnimNotify_P_Sparks()
{
	
}

void UMyAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UMyAnimInstance::AnimNotify_SAttackHitCheck()
{
	OnSAttackHitCheck.Broadcast();
}

void UMyAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName UMyAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Section : %d "), Section));
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

