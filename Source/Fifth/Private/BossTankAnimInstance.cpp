// Fill out your copyright notice in the Description page of Project Settings.


#include "BossTankAnimInstance.h"

UBossTankAnimInstance::UBossTankAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsDead = false;

	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		ATTACK_MONTAGE(TEXT("/Game/MyCharacter/Animation/NPCAnimations/TankAnimations/BossTankAni/BossTank_Attack3.BossTank_Attack3"));
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		KICK_MONTAGE(TEXT("/Game/MyCharacter/Animation/NPCAnimations/TankAnimations/BossTankAni/BossTank_Kick.BossTank_Kick"));
	if (KICK_MONTAGE.Succeeded())
	{
		KickMontage = KICK_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		DAMAGED_MONTAGE(TEXT("/Game/MyCharacter/Animation/NPCAnimations/TankAnimations/BossTankAni/BossTank_Damaged.BossTank_Damaged"));
	if (DAMAGED_MONTAGE.Succeeded())
	{
		DamagedMontage = DAMAGED_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		SKILL_MONTAGE(TEXT("/Game/MyCharacter/Animation/NPCAnimations/TankAnimations/BossTankAni/BossTank_Skill.BossTank_Skill"));
	if (SKILL_MONTAGE.Succeeded())
	{
		SkillMontage = SKILL_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage>
		SPECIAL_MONTAGE(TEXT("/Game/MyCharacter/Animation/NPCAnimations/TankAnimations/BossTankAni/BossTank_SpecialAttack.BossTank_SpecialAttack"));
	if (SPECIAL_MONTAGE.Succeeded())
	{
		SpecialMontage = SPECIAL_MONTAGE.Object;
	}
}

void UBossTankAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
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

void UBossTankAnimInstance::PlayAttackMontage()
{
	//ABCHECK(!IsDead);
	Montage_Play(AttackMontage, 1.0f);
}

void UBossTankAnimInstance::PlayKickMontage()
{
	//ABCHECK(!IsDead);
	Montage_Play(KickMontage, 1.0f);
}

void UBossTankAnimInstance::PlayDamagedMontage()
{
	//ABCHECK(!IsDead);
	Montage_Play(DamagedMontage, 1.0f);
}

void UBossTankAnimInstance::PlaySkillMontage()
{
	//ABCHECK(!IsDead);
	Montage_Play(SkillMontage, 1.0f);
}

void UBossTankAnimInstance::PlaySpecialMontage()
{
	//ABCHECK(!IsDead);
	Montage_Play(SpecialMontage, 1.0f);
}

void UBossTankAnimInstance::AnimNotify_AttackHitCheck()
{

	OnAttackHitCheck.Broadcast();
}

void UBossTankAnimInstance::AnimNotify_KickHitCheck()
{

	OnKickHitCheck.Broadcast();
}

void UBossTankAnimInstance::AnimNotify_SkillHitCheck()
{

	OnSkillHitCheck.Broadcast();
}