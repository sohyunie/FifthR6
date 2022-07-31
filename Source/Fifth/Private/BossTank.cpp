// Fill out your copyright notice in the Description page of Project Settings.


#include "BossTank.h"
#include "BossTankAnimInstance.h"
#include "BossStatComponent.h"
#include "DrawDebugHelpers.h"
#include "BossAIController.h"
#include "MyGameInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "ClientSocket.h"
#include "NetCharacter.h"
#include "NetPlayerController.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ABossTank::ABossTank()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BossStat = CreateDefaultSubobject<UBossStatComponent>(TEXT("BOSSSTAT"));

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f),
		FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BossTank(TEXT
	("/Game/MyCharacter/Characters/BossTank.BossTank"));
	if (BossTank.Succeeded())
	{
		//ABLOG(Warning, TEXT("SUCCEEDED1"));
		GetMesh()->SetSkeletalMesh(BossTank.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> BossTank_ANIM(TEXT
	("/Game/MyCharacter/Animation/NPCAnimations/TankAnimations/BossTankAni/BossTankAnimBlueprint.BossTankAnimBlueprint_C"));
	if (BossTank_ANIM.Succeeded())
	{
		//ABLOG(Warning, TEXT("SUCCEEDED2"));
		GetMesh()->SetAnimInstanceClass(BossTank_ANIM.Class);
	}

	IsAttacking = false;
	IsDamaging = false;
	IsKicking = false;
	IsSkillUsing = false;
	IsSpecialAttacking = false;
	IsScreaming = false;

	AttackRange = 200.0f;
	AttackRadius = 50.0f;

	GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);
	GetCapsuleComponent()->SetCapsuleRadius(100);

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("BossTank"));

	AIControllerClass = ABossAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarObj(
		TEXT("/Game/UI/Boss_HP.Boss_HP_C"));

	HUDWidgetClass = HealthBarObj.Class;
}

// Called when the game starts or when spawned
void ABossTank::BeginPlay()
{
	Super::BeginPlay();

	FullHealth = 1000.f;
	Health = FullHealth;
	HealthPercentage = 1.0f;
	bCanBeDamaged = true;

	if (HUDWidgetClass)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		//if (CurrentWidget)
		//{
			//CurrentWidget->AddToViewport();
		//}

	}



	auto MyGameInstance = Cast<UMyGameInstance>(GetGameInstance());
	Id = MyGameInstance->uniqueMonsterID++;

	UE_LOG(LogClass, Log, TEXT("Monster : %d"), Id);

}

void ABossTank::SetControlMode(int32 ControlMode)
{
	if (ControlMode == 0)
	{

		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);

	}
}

// Called every frame
void ABossTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDamaging)
	{

		SetActorLocation(GetActorLocation() + GetWorld()->GetFirstPlayerController()->GetPawn()
			->GetControlRotation().Vector()/**10*/);

	}

}

// Called to bind functionality to input
void ABossTank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABossTank::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	BTAnim = Cast<UBossTankAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != BTAnim);

	BTAnim->OnMontageEnded.AddDynamic(this, &ABossTank::OnAttackMontageEnded);

	BossStat->OnHPIsZero.AddLambda([this]()->void {
		ABLOG(Warning, TEXT("OnHPIsZero"));
		BTAnim->SetDeadAnim();
		SetActorEnableCollision(false);
	});

	BTAnim->OnAttackHitCheck.AddUObject(this, &ABossTank::AttackCheck);
}

float ABossTank::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	// Health Sync 마스터에서 담당
	ABLOG(Warning, TEXT("BOSSDAMAGED"));
	ANetPlayerController* PlayerController = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController->GetIsMaster())
	{
		bCanBeDamaged = false;
		UpdateMyHealth(-DamageAmount);
		DamageTimer();

		Damaged();


		//float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		//ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

		BossStat->SetDamage(DamageAmount);


		UNiagaraSystem* HitEffect =
			Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(), NULL,
				TEXT("/Game/Effect/Hit.Hit")));
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect,
			this->GetActorLocation() + FVector(50.0f, 20.0f, 0.0f), this->GetActorRotation());
		//return FinalDamage;
		return DamageAmount;
	}

	//float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);
	bCanBeDamaged = false;
	UpdateMyHealth(-DamageAmount);
	DamageTimer();

	Damaged();
	BossStat->SetDamage(DamageAmount);

	//return FinalDamage;
	return DamageAmount;
}


void ABossTank::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetControlMode(0);
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

void ABossTank::Attack()
{
	if (IsAttacking) return;

	BTAnim->PlayAttackMontage();
	IsAttacking = true;
}

void ABossTank::Kick()
{
	if (IsKicking) return;

	BTAnim->PlayKickMontage();
	IsKicking = true;
}

void ABossTank::Damaged()
{
	if (IsDamaging) return;

	BTAnim->PlayDamagedMontage();
	IsAttacking = true;
}

void ABossTank::Skill()
{
	if (IsSkillUsing) return;

	BTAnim->PlaySkillMontage();
	IsAttacking = true;
}

void ABossTank::SpecialAttack()
{
	if (IsSpecialAttacking) return;

	BTAnim->PlaySpecialMontage();
	IsAttacking = true;
}

void ABossTank::Screaming()
{
	if (IsScreaming) return;

	BTAnim->PlayScreamMontage();
	IsScreaming = true;
}

float ABossTank::GetHealth()
{
	return HealthPercentage;
}

void ABossTank::SetDamageState()
{
	bCanBeDamaged = true;
}

void ABossTank::DamageTimer()
{
	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &ABossTank::SetDamageState, 2.f, false);
}

void ABossTank::UpdateMyHealth(float HealthChange)
{
	Health = FMath::Clamp(Health += HealthChange, 0.0f, FullHealth);
	HealthPercentage = Health / FullHealth;

}

void ABossTank::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsAttacking = false;

	OnAttackEnd.Broadcast();
}

void ABossTank::OnDamagedMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsDamaging = false;
}

void ABossTank::OnKickMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsKicking = false;
}

void ABossTank::OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsSkillUsing = false;
}

void ABossTank::OnSpecialMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsSpecialAttacking = false;
}

void ABossTank::OnScreamMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsScreaming = false;
}

void ABossTank::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 200.0f,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(50.0f),
		Params);

/*#if ENABLE_DRAW_DEBUG

	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);

#endif*/


	//if (bResult)
	//{
	//	//ABLOG(Warning, TEXT("1Ok!!"));
	//	if (HitResult.Actor.IsValid())
	//	{
	//		ABLOG(Warning, TEXT("Hit Actor Name: %s"), *HitResult.Actor->GetName());

	//		FDamageEvent DamageEvent;
	//		HitResult.Actor->TakeDamage(BossStat->GetAttack(), DamageEvent, GetController(), this);
	//		//UParticleSystem* ElectricAttackBoomEffect =
	//			//Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
	//				//TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Sparks.P_Sparks'")));
	//		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ElectricAttackBoomEffect,
	//			//this->GetActorLocation());

	//		//ABLOG(Warning, TEXT("2Ok!!"));
	//		Damaged();
	//	}
	//}

	if (bResult)
	{
		//ABLOG(Warning, TEXT("1Ok!!"));
		if (HitResult.Actor.IsValid())
		{
			ABLOG(Warning, TEXT("Hit Actor Name: %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(BossStat->GetAttack(), DamageEvent, GetController(), this);

			// 플레이어 공격
			ANetCharacter* HitCharacter = Cast<ANetCharacter>(HitResult.Actor);
			if (HitCharacter && HitCharacter->GetSessionId() != -1)
			{
				ANetPlayerController* PlayerController = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
				PlayerController->HitCharacter(HitCharacter->GetSessionId(), HitCharacter);
			}

		}
	}
}

void ABossTank::SkillCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 200.0f,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(50.0f),
		Params);

#if ENABLE_DRAW_DEBUG

	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);

#endif


	if (bResult)
	{
		//ABLOG(Warning, TEXT("1Ok!!"));
		if (HitResult.Actor.IsValid())
		{
			ABLOG(Warning, TEXT("Hit Actor Name: %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(BossStat->GetSkill(), DamageEvent, GetController(), this);
			//UParticleSystem* ElectricAttackBoomEffect =
				//Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
					//TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Sparks.P_Sparks'")));
			//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ElectricAttackBoomEffect,
				//this->GetActorLocation());

			//ABLOG(Warning, TEXT("2Ok!!"));
			Damaged();
		}
	}
}

void ABossTank::KickCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 200.0f,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(50.0f),
		Params);

#if ENABLE_DRAW_DEBUG

	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);

#endif


	if (bResult)
	{
		//ABLOG(Warning, TEXT("1Ok!!"));
		if (HitResult.Actor.IsValid())
		{
			ABLOG(Warning, TEXT("Hit Actor Name: %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(BossStat->GetKick(), DamageEvent, GetController(), this);
			//UParticleSystem* ElectricAttackBoomEffect =
				//Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
					//TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Sparks.P_Sparks'")));
			//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ElectricAttackBoomEffect,
				//this->GetActorLocation());

			//ABLOG(Warning, TEXT("2Ok!!"));

			ANetCharacter* HitCharacter = Cast<ANetCharacter>(HitResult.Actor);
			if (HitCharacter && HitCharacter->GetSessionId() != -1)
			{
				ANetPlayerController* PlayerController = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
				PlayerController->HitCharacter(HitCharacter->GetSessionId(), HitCharacter);
			}

		}
	}
}

void ABossTank::StartAction()
{
	//TankAIController->RunAI();
}

/*float ABossTank::GetTankHpRatio()
{
	return BossStat->GetHPRatio();
}*/

bool ABossTank::GetIsAttacking()
{
	return IsAttacking;
}

void ABossTank::PlayTakeDamageAnim()
{
	UNiagaraSystem* HitEffect =
		Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(), NULL,
			TEXT("/Game/Effect/Hit.Hit")));
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect,

		this->GetActorLocation() + FVector(50.0f, 20.0f, 0.0f), this->GetActorRotation());
	return BTAnim->PlayDamagedMontage();
}

/*void ABossTank::SetTankHpRatio(float ratio)
{
	return BossStat->SetHpRatio(ratio);
}*/


void ABossTank::MoveToLocation(const FVector& dest)
{
	// [TODO] boss
	//if (BossAIController)
	//{
	//	BossAIController->MoveToLocation(dest);
	//}
}

void ABossTank::PlayAttackAnim()
{
	return BTAnim->PlayAttackMontage();
}

void ABossTank::SetTankHpRatio(float ratio)
{
	return BossStat->SetHpRatio(ratio);
}

