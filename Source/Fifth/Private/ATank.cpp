// Fill out your copyright notice in the Description page of Project Settings.


#include "ATank.h"
#include "ATankAnimInstance.h"
#include "TankAIController.h"
#include "TankStatComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AATank::AATank()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TankStat = CreateDefaultSubobject<UTankStatComponent>(TEXT("TANKSTAT"));

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f),
		FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ATank(TEXT
	("/Game/MyCharacter/Characters/Tank.Tank"));
	if (ATank.Succeeded())
	{
		ABLOG(Warning, TEXT("SUCCEEDED1"));
		GetMesh()->SetSkeletalMesh(ATank.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> ATank_ANIM(TEXT
	("/Game/MyCharacter/Animation/NPCAnimations/TankAnimations/TankAnimBlueprint.TankAnimBlueprint_C"));
	if (ATank_ANIM.Succeeded())
	{
		ABLOG(Warning, TEXT("SUCCEEDED2"));
		GetMesh()->SetAnimInstanceClass(ATank_ANIM.Class);
	}

	GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);
	GetCapsuleComponent()->SetCapsuleRadius(100);

	IsAttacking = false;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ATank"));

	AttackRange = 200.0f;
	AttackRadius = 50.0f;

	AIControllerClass = ATankAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AATank::BeginPlay()
{
	Super::BeginPlay();
	
}

void AATank::SetControlMode(int32 ControlMode)
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
void AATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AATank::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ATAnim = Cast<UATankAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != ATAnim);

	ATAnim->OnMontageEnded.AddDynamic(this, &AATank::OnAttackMontageEnded);
	ATAnim->OnMontageEnded.AddDynamic(this, &AATank::OnDamagedMontageEnded);

	ATAnim->OnAttackHitCheck.AddUObject(this, &AATank::AttackCheck);

	TankStat->OnHPIsZero.AddLambda([this]()->void {
		ABLOG(Warning, TEXT("OnHPIsZero"));
		ATAnim->SetDeadAnim();
		SetActorEnableCollision(false);
		});

	

	
}

float AATank::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);
	Damaged();
	TankStat->SetDamage(FinalDamage);

	

	return FinalDamage;
}

void AATank::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetControlMode(0);
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

void AATank::Attack()
{
	if (IsAttacking) return;

	ATAnim->PlayAttackMontage();
	IsAttacking = true;
}

void AATank::Damaged()
{
	if (IsDamaging) return;
	
	ATAnim->PlayDamagedMontage();
	IsDamaging = true;
	
}

void AATank::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//ABCHECK(IsAttacking);
	
	IsAttacking = false;
	//IsDamaging = false;

	OnAttackEnd.Broadcast();
	
}

void AATank::OnDamagedMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//ABCHECK(IsDamaging);

	//IsAttacking = false;
	IsDamaging = false;

	

}

void AATank::AttackCheck()
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
			HitResult.Actor->TakeDamage(TankStat->GetAttack(), DamageEvent, GetController(), this);
			UParticleSystem* ElectricAttackBoomEffect =
				Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
					TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Sparks.P_Sparks'")));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ElectricAttackBoomEffect,
				this->GetActorLocation());
		
			//ABLOG(Warning, TEXT("2Ok!!"));
			Damaged();
		}
	}
}