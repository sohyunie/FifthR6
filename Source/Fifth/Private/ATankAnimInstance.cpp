// Fill out your copyright notice in the Description page of Project Settings.


#include "ATankAnimInstance.h"


UATankAnimInstance::UATankAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsDead = false;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> 
		ATTACK_MONTAGE(TEXT("/Game/MyCharacter/Animation/NPCAnimations/TankAnimations/Tank_Attack1_Montage.Tank_Attack1_Montage"));
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		DAMAGED_MONTAGE(TEXT("/Game/MyCharacter/Animation/NPCAnimations/TankAnimations/Tank_Damaged_Montage.Tank_Damaged_Montage"));
	if (DAMAGED_MONTAGE.Succeeded())
	{
		DamagedMontage = DAMAGED_MONTAGE.Object;
	}
}

void UATankAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
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

void UATankAnimInstance::PlayAttackMontage()
{
	//ABCHECK(!IsDead);
	Montage_Play(AttackMontage, 1.0f);
}

void UATankAnimInstance::PlayDamagedMontage()
{
	//ABLOG(Warning, TEXT("Hey!!"));
	//ABCHECK(!IsDead);
	Montage_Play(DamagedMontage, 1.0f);
	
}

void UATankAnimInstance::AnimNotify_AttackHitCheck()
{
	
	OnAttackHitCheck.Broadcast();
}