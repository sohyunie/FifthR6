// Fill out your copyright notice in the Description page of Project Settings.


#include "Shadow.h"
#include "ShadowAnimInstance.h"
#include "MyAIController.h"
#include "ShadowStatComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AShadow::AShadow()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ShadowStat = CreateDefaultSubobject<UShadowStatComponent>(TEXT("SHADOWSTAT"));

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f),
		FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Shadow(TEXT
	("/Game/MyCharacter/Characters/Shadow.Shadow"));
	if (Shadow.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(Shadow.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> Shadow_ANIM(TEXT
	("/Game/MyCharacter/Animation/ShadowAnimBlueprint.ShadowAnimBlueprint_C"));
	if (Shadow_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(Shadow_ANIM.Class);
	}

	GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);
	GetCapsuleComponent()->SetCapsuleRadius(100);


	SetControlMode(0);
	IsAttacking = false;
	MaxCombo = 3;
	AttackEndComboState();

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Shadow"));

	AttackRange = 200.0f;
	AttackRadius = 50.0f;

	AIControllerClass = AMyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AShadow::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShadow::SetControlMode(int32 ControlMode)
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
void AShadow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShadow::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AShadow::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ShadowAnim = Cast<UShadowAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != ShadowAnim);

	ShadowAnim->OnMontageEnded.AddDynamic(this, &AShadow::OnAttackMontageEnded);

	ShadowAnim->OnNextAttackCheck.AddLambda([this]()->void {
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;

		if (IsComboInputOn)
		{
			//ABLOG(Warning, TEXT("OnNextAttackCheck"));
			AttackStartComboState();
			ShadowAnim->JumpToAttackMontageSection(CurrentCombo);
		}
		});

	ShadowAnim->OnAttackHitCheck.AddUObject(this, &AShadow::AttackCheck);

	ShadowStat->OnHPIsZero.AddLambda([this]()->void {
		ABLOG(Warning, TEXT("OnHPIsZero"));
		ShadowAnim->SetDeadAnim();
		SetActorEnableCollision(false);
		});
}


float AShadow::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	ShadowStat->SetDamage(FinalDamage);

	return FinalDamage;
}

void AShadow::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetControlMode(0);
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

void AShadow::Attack()
{
	if (IsAttacking)
	{
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		ABCHECK(CurrentCombo == 0);
		AttackStartComboState();
		ShadowAnim->PlayAttackMontage();
		ShadowAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}

}

void AShadow::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
	OnAttackEnd.Broadcast();
}

void AShadow::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AShadow::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AShadow::AttackCheck()
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
		if (HitResult.Actor.IsValid())
		{
			ABLOG(Warning, TEXT("Hit Actor Name: %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(ShadowStat->GetAttack(), DamageEvent, GetController(), this);
		}
	}
}
