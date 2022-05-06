// Fill out your copyright notice in the Description page of Project Settings.


#include "ATank.h"
#include "ATankAnimInstance.h"
#include "TankAIController.h"
#include "TankStatComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"
#include "TankWidget.h"
#include "TankSetting.h"
#include "MyGameInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


// Sets default values
AATank::AATank()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TankStat = CreateDefaultSubobject<UTankStatComponent>(TEXT("TANKSTAT"));
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));

	HPBarWidget->SetupAttachment(GetMesh());

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f),
		FRotator(0.0f, -90.0f, 0.0f));

	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 300.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT
	("/Game/UI/UI_HPBar.UI_HPBar_C"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f,50.0f));
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ATank(TEXT
	("/Game/MyCharacter/Characters/Tank.Tank"));
	if (ATank.Succeeded())
	{
		//ABLOG(Warning, TEXT("SUCCEEDED1"));
		GetMesh()->SetSkeletalMesh(ATank.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> ATank_ANIM(TEXT
	("/Game/MyCharacter/Animation/NPCAnimations/TankAnimations/TankAnimBlueprint.TankAnimBlueprint_C"));
	if (ATank_ANIM.Succeeded())
	{
		//ABLOG(Warning, TEXT("SUCCEEDED2"));
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
	

	AssetIndex = 0;
	SetActorHiddenInGame(true);
	HPBarWidget->SetHiddenInGame(true);
	SetCanBeDamaged(false);

	
	
	DeadTimer = 5.0f;
}

void AATank::OnAssetLoadCompleted()
{
	USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	AssetStreamingHandle.Reset();
	ABCHECK(nullptr != AssetLoaded);
	GetMesh()->SetSkeletalMesh(AssetLoaded);

	SetTankState(ECharacterState::READY);
	

}

// Called when the game starts or when spawned
void AATank::BeginPlay()
{
	Super::BeginPlay();

	TankAIController = Cast<ATankAIController>(GetController());
	ABCHECK(nullptr != TankAIController);

	auto DefaultSetting = GetDefault<UTankSetting>();

	AssetIndex = 0;

	CharacterAssetToLoad = DefaultSetting->TankAssets[AssetIndex];
	auto MyGameInstance = Cast<UMyGameInstance>(GetGameInstance());
	ABCHECK(nullptr != MyGameInstance);
	AssetStreamingHandle = MyGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad,
		FStreamableDelegate::CreateUObject(this, &AATank::OnAssetLoadCompleted));
	
	

	SetTankState(ECharacterState::LOADING);
}

void AATank::SetTankState(ECharacterState NewState)
{
	ABCHECK(CurrentState != NewState);
	CurrentState = NewState;

	switch (CurrentState)
	{
	case ECharacterState::LOADING:
	{
		SetActorHiddenInGame(true);
		HPBarWidget->SetHiddenInGame(true);
		SetCanBeDamaged(false);
		break;
	}
	case ECharacterState::READY:
	{
		SetActorHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(false);
		SetCanBeDamaged(true);

		TankStat->OnHPIsZero.AddLambda([this]()->void {
			SetTankState(ECharacterState::DEAD);
			});

		auto TankWidget = Cast<UTankWidget>(HPBarWidget->GetUserWidgetObject());
		ABCHECK(nullptr != TankWidget);
		TankWidget->BindTankStat(TankStat);

		SetControlMode(0);
		GetCharacterMovement()->MaxWalkSpeed = 400.0f;
		TankAIController->RunAI();

		break;
	}
	case ECharacterState::DEAD:
	{
		SetActorEnableCollision(false);
		GetMesh()->SetHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(true);
		ATAnim->SetDeadAnim();
		SetCanBeDamaged(false);

		TankAIController->StopAI();

		GetWorld()->GetTimerManager().SetTimer
		(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void {
			Destroy();

			}), DeadTimer, false);


		break;
	}
	}
}


ECharacterState AATank::GetTankState() const
{
	return CurrentState;
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

	if (IsDamaging)
	{

		SetActorLocation(GetActorLocation() + GetWorld()->GetFirstPlayerController()->GetPawn()
			->GetControlRotation().Vector()/**10*/);
	}

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
	
	ABLOG(Warning, TEXT("ACCESSGRANTED!!!"));
	UNiagaraSystem* HitEffect =
		Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(), NULL,
			TEXT("/Game/Effect/Hit.Hit")));
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect,
		this->GetActorLocation() + FVector(50.0f, 20.0f, 0.0f), this->GetActorRotation());
	

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
	if (IsDamaging == false) {
		if (IsAttacking) return;

		ATAnim->PlayAttackMontage();
		IsAttacking = true;
	}
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

	OnAttackEnd.Broadcast();

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


	if (bResult)
	{
		//ABLOG(Warning, TEXT("1Ok!!"));
		if (HitResult.Actor.IsValid())
		{
			ABLOG(Warning, TEXT("Hit Actor Name: %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(TankStat->GetAttack(), DamageEvent, GetController(), this);
			
			
			Damaged();
		}
	}
}