// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "MyAnimInstance.h"
#include "MyWeapon.h"
#include "WarriorStatComponent.h"
#include "DrawDebugHelpers.h"
#include "MyCharacterSetting.h"
#include "MyPlayerController.h"
#include "MyGameInstance.h"
#include "MyPlayerState.h"
#include "MyHUDWidget.h"



// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	WarriorStat = CreateDefaultSubobject<UWarriorStatComponent>(TEXT("WARRIORSTAT"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f),
		FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT
	("/Game/MyCharacter/Animation/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));
	if (WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

	GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);
	GetCapsuleComponent()->SetCapsuleRadius(100);


	SetControlMode(0);
	IsAttacking = false;
	IsSAttacking = false;
	MaxCombo = 3;
	AttackEndComboState();

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("MyCharacter"));

	AttackRange = 200.0f;
	AttackRadius = 50.0f;

	auto DefaultSetting = GetDefault<UMyCharacterSetting>();

	AssetIndex = 2;

	

	/*CharacterAssetToLoad = DefaultSetting->WarriorAssets[AssetIndex];
	auto MyGameInstance = Cast<UMyGameInstance>(GetGameInstance());
	ABCHECK(nullptr != MyGameInstance);
	AssetStreamingHandle = MyGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad,
		FStreamableDelegate::CreateUObject(this, &AMyCharacter::OnAssetLoadCompleted));

	

	SetActorHiddenInGame(true);
	SetCanBeDamaged(false);*/

	DeadTimer = 5.0f;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	MyPlayerController = Cast<AMyPlayerController>(GetController());

	
	
	FName WeaponSocket(TEXT("Bip-R-HandSocket"));
	
	
	//ABLOG(Warning, TEXT("WEAPON"));
	auto CurWeapon = GetWorld()->SpawnActor<AMyWeapon>(FVector::ZeroVector,
		FRotator::ZeroRotator);

	if (nullptr != CurWeapon)
	{
		ABLOG(Warning, TEXT("WEAPON!"));
		CurWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			WeaponSocket);

	}

	auto DefaultSetting = GetDefault<UMyCharacterSetting>();

	AssetIndex = 2;

	CharacterAssetToLoad = DefaultSetting->WarriorAssets[AssetIndex];
	auto MyGameInstance = Cast<UMyGameInstance>(GetGameInstance());
	ABCHECK(nullptr != MyGameInstance);
	AssetStreamingHandle = MyGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad,
		FStreamableDelegate::CreateUObject(this, &AMyCharacter::OnAssetLoadCompleted));
	
	SetWarriorState(ECharacterState::LOADING);
	
	
}

void AMyCharacter::SetWarriorState(ECharacterState NewState)
{
	ABCHECK(CurrentState != NewState);
	CurrentState = NewState;

	switch (CurrentState)
	{
	case ECharacterState::LOADING:
	{
		DisableInput(MyPlayerController);

		MyPlayerController->GetHUDWidget()->BindCharacterStat(WarriorStat);

		auto MyPlayerState = Cast<AMyPlayerState>(GetPlayerState());
		ABCHECK(nullptr != MyPlayerState);
		WarriorStat->SetNewLevel(MyPlayerState->GetCharacterLevel());
		
		
		SetActorHiddenInGame(true);
		SetCanBeDamaged(false);
		break;
	}
	case ECharacterState::READY:
	{
		SetActorHiddenInGame(false);
		SetCanBeDamaged(true);

		WarriorStat->OnHPIsZero.AddLambda([this]()->void {
			SetWarriorState(ECharacterState::DEAD);
			});

		SetControlMode(0);
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		EnableInput(MyPlayerController);

		break;
	}
	case ECharacterState::DEAD:
	{
		SetActorEnableCollision(false);
		GetMesh()->SetHiddenInGame(false);
		MyAnim->SetDeadAnim();
		SetCanBeDamaged(false);

		DisableInput(MyPlayerController);

		GetWorld()->GetTimerManager().
			SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void {
			MyPlayerController->RestartLevel();

				}), DeadTimer, false);

		break;
	}
	}
}


ECharacterState AMyCharacter::GetWarriorState() const
{
	return CurrentState;
}

void AMyCharacter::OnAssetLoadCompleted()
{
	USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	AssetStreamingHandle.Reset();
	ABCHECK(nullptr != AssetLoaded);
	GetMesh()->SetSkeletalMesh(AssetLoaded);

	SetWarriorState(ECharacterState::READY);
	
	
}

void AMyCharacter::SetControlMode(int32 ControlMode)
{
	if (ControlMode == 0)
	{

		SpringArm->TargetArmLength = 450.0f;
		SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	}
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsAttacking || IsSAttacking)
	{
		//ABLOG(Warning, TEXT("TICK"));
		
		//FVector MyCharacter = GetActorLocation() + (100.0f, 0.0f, 0.0f);
		SetActorLocation(GetActorLocation() + GetControlRotation().Vector());
		

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Player Location: %s"), *MyCharacter.ToString()));
	}

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed,this, &AMyCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("SAttack"), EInputEvent::IE_Pressed, this, &AMyCharacter::SAttack);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AMyCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AMyCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMyCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMyCharacter::Turn);

}

void AMyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MyAnim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != MyAnim);

	MyAnim->OnMontageEnded.AddDynamic(this, &AMyCharacter::OnAttackMontageEnded);
	MyAnim->OnMontageEnded.AddDynamic(this, &AMyCharacter::OnSAttackMontageEnded);

	MyAnim->OnNextAttackCheck.AddLambda([this]()->void {
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;

		if (IsComboInputOn)
		{
			//ABLOG(Warning, TEXT("OnNextAttackCheck"));
			AttackStartComboState();
			beChecked = true;
			//MyAnim->JumpToAttackMontageSection(CurrentCombo);
		}
		
		});

	
	MyAnim->OnIsChecked.AddLambda([this]()->void {
		ABLOG(Warning, TEXT("OnIsChecked"));
		//CanNextCombo = false;

		if (beChecked)
		{
			ABLOG(Warning, TEXT("OnbeChecked"));
			//AttackStartComboState();
			MyAnim->JumpToAttackMontageSection(CurrentCombo);
			beChecked = false;
		}
		else {
			StopAnimMontage();
			ABLOG(Warning, TEXT("STOP"));
		}
		});

	


	MyAnim->OnAttackHitCheck.AddUObject(this, &AMyCharacter::AttackCheck);
	MyAnim->OnSAttackHitCheck.AddUObject(this, &AMyCharacter::SAttackCheck);

	WarriorStat->OnHPIsZero.AddLambda([this]()->void {
		ABLOG(Warning, TEXT("OnHPIsZero"));
		MyAnim->SetDeadAnim();
		SetActorEnableCollision(false);
		});
}


float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	WarriorStat->SetDamage(FinalDamage);

	return FinalDamage;
}

void AMyCharacter::UpDown(float NewAxisValue)
{
	if (!IsAttacking && !IsSAttacking) {
		
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
	}
	/*else {
		ABLOG(Warning, TEXT("Attacking"));
	}*/
	
}

void AMyCharacter::LeftRight(float NewAxisValue)
{
	if (!IsAttacking && !IsSAttacking) {
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
	}
	/*else {
		ABLOG(Warning, TEXT("Attacking"));
	}*/
}

void AMyCharacter::LookUp(float NewAxisValue)
{
	AddControllerPitchInput(NewAxisValue);
}

void AMyCharacter::Turn(float NewAxisValue)
{
	AddControllerYawInput(NewAxisValue);
}

void AMyCharacter::Attack()
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
		MyAnim->PlayAttackMontage();
		//beChecked = true;
		//MyAnim->JumpToAttackMontageSection(CurrentCombo);//강제 이동이 아닌 조건성립으로
		IsAttacking = true;
	}

}

void AMyCharacter::SAttack()
{
	if (IsSAttacking) return;
	
	MyAnim->PlaySAttackMontage();
	IsSAttacking = true;
}

void AMyCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//ABCHECK(IsAttacking);
	//ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
}

void AMyCharacter::OnSAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//ABCHECK(IsSAttacking);
	//ABCHECK(CurrentCombo > 0);
	IsSAttacking = false;
	//AttackEndComboState();
}

void AMyCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AMyCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AMyCharacter::AttackCheck()
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
		if (HitResult.Actor.IsValid())
		{
			ABLOG(Warning, TEXT("Hit Actor Name: %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;

			
		
			HitResult.Actor->TakeDamage(WarriorStat->GetAttack(), DamageEvent, GetController(), this);
		}
	}
}


void AMyCharacter::SAttackCheck()
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
		if (HitResult.Actor.IsValid())
		{
			ABLOG(Warning, TEXT("Hit Actor Name: %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;

			
			HitResult.Actor->TakeDamage(WarriorStat->GetSAttack(), DamageEvent, GetController(), this);
			
		}
	}
}


