// Fill out your copyright notice in the Description page of Project Settings.


#include "ManOfFire.h"
#include "ManAnimInstance.h"
#include "ManStatComponent.h"
#include "DrawDebugHelpers.h"
#include "ManSetting.h"
#include "MyPlayerController.h"
#include "WarriorOfFire.h"
#include "MyGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AManOfFire::AManOfFire()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	ManStat = CreateDefaultSubobject<UManStatComponent>(TEXT("MANSTAT"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f),
		FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> MAN_ANIM(TEXT
	("/Game/MyCharacter/Animation/ManAnimBlueprint.ManAnimBlueprint_C"));
	if (MAN_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(MAN_ANIM.Class);
	}

	GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);
	GetCapsuleComponent()->SetCapsuleRadius(100);

	SetControlMode(0);

	IsAttacking = false;
	IsTransforming = false;
	MaxCombo = 3;
	AttackEndComboState();

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ManOfFire"));

	AttackRange = 200.0f;
	AttackRadius = 50.0f;

	//ChangePawn = false;
}

// Called when the game starts or when spawned
void AManOfFire::BeginPlay()
{
	Super::BeginPlay();
	
}

void AManOfFire::SetControlMode(int32 ControlMode)
{
	if (ControlMode == 0)
	{

		SpringArm->TargetArmLength = 450.0f;
		SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	}
}

// Called every frame
void AManOfFire::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AManOfFire::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Attack"),EInputEvent::IE_Pressed, this, &AManOfFire::Attack);
	PlayerInputComponent->BindAction(TEXT("SAttack"), EInputEvent::IE_Pressed, this, &AManOfFire::Transform);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AManOfFire::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AManOfFire::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AManOfFire::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AManOfFire::Turn);

}

void AManOfFire::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	NewCon = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	ABCHECK(nullptr != NewCon);

	MyGame = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	ABCHECK(nullptr != MyGame);

	ManAnim = Cast<UManAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != ManAnim);

	ManAnim->OnMontageEnded.AddDynamic(this, &AManOfFire::OnAttackMontageEnded);
	ManAnim->OnMontageEnded.AddDynamic(this, &AManOfFire::OnTransformMontageEnded);

	ManAnim->OnNextAttackCheck.AddLambda([this]()->void {
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;

		if (IsComboInputOn)
		{
			//ABLOG(Warning, TEXT("OnNextAttackCheck"));
			AttackStartComboState();
			ManAnim->JumpToAttackMontageSection(CurrentCombo);
		}
		});

	ManAnim->OnAttackHitCheck.AddUObject(this, &AManOfFire::AttackCheck);

	ManStat->OnHPIsZero.AddLambda([this]()->void {
		ABLOG(Warning, TEXT("OnHPIsZero"));
		//ManAnim->SetDeadAnim();
		SetActorEnableCollision(false);
		});
}

void AManOfFire::UpDown(float NewAxisValue)
{
	AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
}

void AManOfFire::LeftRight(float NewAxisValue)
{
	AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
}

void AManOfFire::LookUp(float NewAxisValue)
{
	AddControllerPitchInput(NewAxisValue);
}

void AManOfFire::Turn(float NewAxisValue)
{
	AddControllerYawInput(NewAxisValue);
}

void AManOfFire::Attack()
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
		ManAnim->PlayAttackMontage();
		ManAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void AManOfFire::Transform()
{
	//ABLOG(Warning, TEXT("TRANSFORM!!!"));
	if (IsTransforming) return;

	ManAnim->PlayTransformMontage();

	IsTransforming = true;
	//ChangePawn = true;

	//AMyPlayerController* NewCon{};
	APawn* Newpawn = Cast<APawn>(AWarriorOfFire::StaticClass());
	NewCon->OnPossess(Newpawn);
	
	
}

void AManOfFire::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
}

void AManOfFire::OnTransformMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ABCHECK(IsTransforming);
	
	IsTransforming = false;
	
}

void AManOfFire::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AManOfFire::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AManOfFire::AttackCheck()
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
		ABLOG(Warning, TEXT("OOOOK"));
		if (HitResult.Actor.IsValid())
		{
			ABLOG(Warning, TEXT("Hit Actor Name: %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(ManStat->GetAttack(), DamageEvent, GetController(), this);
		}
	}
}

float AManOfFire::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	ManStat->SetDamage(FinalDamage);

	return FinalDamage;
}