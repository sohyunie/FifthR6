// Fill out your copyright notice in the Description page of Project Settings.


#include "ManAnimInstance.h"

UManAnimInstance::UManAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsDead = false;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(
		TEXT("/Game/MyCharacter/Animation/Man_Attack_Montage.Man_Attack_Montage"));
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}

void UManAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();

	if (!::IsValid(Pawn)) return;


	if (!IsDead)
	{
		
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		auto Character = Cast<ACharacter>(Pawn);
		
	}
}

void UManAnimInstance::PlayAttackMontage()
{
	ABCHECK(!IsDead);
	Montage_Play(AttackMontage, 1.0f);
	
}

void UManAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(!IsDead);
	ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("NewSection : %d "), NewSection));
}

void UManAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UManAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName UManAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Section : %d "), Section));
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}
