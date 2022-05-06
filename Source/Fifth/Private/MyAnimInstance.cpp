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

	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		TATTACK_MONTAGE(TEXT("/Game/MyCharacter/Animation/WarriorOfFire_TwoHand_Attack.WarriorOfFire_TwoHand_Attack"));
	if (TATTACK_MONTAGE.Succeeded())
	{
		TAttackMontage = TATTACK_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		DAMAGED_MONTAGE(TEXT("/Game/MyCharacter/Animation/WarriorOfFire_Damaging.WarriorOfFire_Damaging"));
	if (DAMAGED_MONTAGE.Succeeded())
	{
		DamagedMontage = DAMAGED_MONTAGE.Object;
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

void UMyAnimInstance::PlayTAttackMontage()
{
	ABCHECK(!IsDead);
	Montage_Play(TAttackMontage, 1.0f);

}

void UMyAnimInstance::PlayDamagedMontage()
{
	//ABLOG(Warning, TEXT("Hey!!"));
	//ABCHECK(!IsDead);
	Montage_Play(DamagedMontage, 1.0f);

}


void UMyAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(!IsDead);
	ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("NewSection : %d "), NewSection));
}

void UMyAnimInstance::JumpToTAttackMontageSection(int32 NewSection)
{
	ABCHECK(!IsDead);
	ABCHECK(Montage_IsPlaying(TAttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), TAttackMontage);
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("NewSection : %d "), NewSection));
}

void UMyAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UMyAnimInstance::AnimNotify_SAttackHitCheck()
{
	OnSAttackHitCheck.Broadcast();
}

void UMyAnimInstance::AnimNotify_TAttackHitCheck()
{
	OnTAttackHitCheck.Broadcast();
}

void UMyAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

void UMyAnimInstance::AnimNotify_IsChecked()
{
	OnIsChecked.Broadcast();
}

void UMyAnimInstance::AnimNotify_TNextAttackCheck()
{
	OnTNextAttackCheck.Broadcast();
}

void UMyAnimInstance::AnimNotify_TIsChecked()
{
	OnTIsChecked.Broadcast();
}

FName UMyAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Section : %d "), Section));
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 3), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

FName UMyAnimInstance::GetTAttackMontageSectionName(int32 Section)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Section : %d "), Section));
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 3), NAME_None);
	return FName(*FString::Printf(TEXT("TAttack%d"), Section));
}
