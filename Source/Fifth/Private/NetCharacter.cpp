// Fill out your copyright notice in the Description page of Project Settings.


#include "NetCharacter.h"
#include "MyAnimInstance.h"
#include "MyWeapon.h"
#include "WarriorStatComponent.h"
#include "DrawDebugHelpers.h"
#include "MyCharacterSetting.h"
#include "NetPlayerController.h"
#include "MyGameInstance.h"
#include "MyPlayerState.h"
#include "MyHUDWidget.h"
#include "ATank.h"
#include "BossTank.h"
#include "FireBall.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "OverlapRangeActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Camera/PlayerCameraManager.h"
#include "MyMatineeCameraShake.h"
#include "ExitKey.h"
#include "SaveCom.h"
#include "Kismet/GameplayStatics.h"
#include "MySaveGame.h"
#include "Door.h"


// Sets default values
ANetCharacter::ANetCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	WarriorStat = CreateDefaultSubobject<UWarriorStatComponent>(TEXT("WARRIORSTAT"));

	
	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("/Game/UI/Skill_Curve.Skill_Curve"));
	SkillCurve = Curve.Object;
	

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));

	//Camera->SetupAttachment(GetMesh());



	ViewRotator = 0.0f;
	ViewArm = CreateDefaultSubobject<USpringArmComponent>("ViewArm");
	//ViewArm->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, (TEXT("Bip-Head")));
	ViewArm->SetupAttachment(GetMesh(),(TEXT("Bip-Head")));

	ViewArm->TargetArmLength = -50.f;
	//ViewArm->CameraLagSpeed = 3.f;
	ViewArm->CameraRotationLagSpeed = 10.f;
	ViewArm->bEnableCameraLag = false;
	ViewArm->bEnableCameraRotationLag = true;
	ViewArm->AddRelativeRotation(FRotator(90.f, 0, -90.f));
	ViewArm->bUsePawnControlRotation = true;
	Camera = CreateDefaultSubobject<UCameraComponent>("CAMERA");
	//Camera->AttachToComponent(ViewArm, FAttachmentTransformRules::KeepRelativeTransform);
	Camera->SetupAttachment(ViewArm);
	

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f),
		FRotator(0.0f, -90.0f, 0.0f));

	//GetMesh()->SetOwnerNoSee(true);
	
	Scene->SetupAttachment(GetMesh());

	FName BallSocket(TEXT("Bip-L-Finger2"));
	//Scene->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		//BallSocket);
	Scene->SetupAttachment(GetMesh(),
		BallSocket);

	//FName HeadSocket(TEXT("Bip-Head"));
	//Camera->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		//HeadSocket);
	//Camera->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));

	//Camera->SetRelativeLocation(FVector(15.0f, 16.0f,125.0f + BaseEyeHeight));
	//Camera->SetRelativeRotation(FRotator(0.0f, -90.0f, -100.0f));
	//Camera->bUsePawnControlRotation = true;
	

	//Sound
	//AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SOUND"));;
	//AudioComponent->SetupAttachment(Mesh);
	
	static ConstructorHelpers::FObjectFinder<USoundWave> Fire(TEXT("SoundWave'/Game/MySound/Sound-Effect-Laser_256k.Sound-Effect-Laser_256k'"));

	if (Fire.Succeeded())
	{
		Fire_Sound = Fire.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundWave> AOE(TEXT("SoundWave'/Game/MySound/Explosion-Sound-Effect-HD_256k.Explosion-Sound-Effect-HD_256k'"));

	if (AOE.Succeeded())
	{
		Magic_Sound = AOE.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundWave> SWORD(TEXT("SoundWave'/Game/MySound/SWORDCLASH.SWORDCLASH'"));

	if (SWORD.Succeeded())
	{
		Sword_Sound = SWORD.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundWave> Key(TEXT("SoundWave'/Game/MySound/Video-Game-Beep-Sound-Effect_256k.Video-Game-Beep-Sound-Effect_256k'"));

	if (Key.Succeeded())
	{
		Key_Sound = Key.Object;
	}

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
	IsRAttacking = false;
	MaxCombo = 3;
	AttackEndComboState();

	//GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ANetCharacter::DeathOverlap);
	
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("MyCharacter"));

	AttackRange = 200.0f;
	AttackRadius = 50.0f;

	auto DefaultSetting = GetDefault<UMyCharacterSetting>();

	AssetIndex = 2;

	/*CharacterAssetToLoad = DefaultSetting->WarriorAssets[AssetIndex];
	auto MyGameInstance = Cast<UMyGameInstance>(GetGameInstance());
	ABCHECK(nullptr != MyGameInstance);
	AssetStreamingHandle = MyGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad,
		FStreamableDelegate::CreateUObject(this, &ANetCharacter::OnAssetLoadCompleted));



	SetActorHiddenInGame(true);
	SetCanBeDamaged(false);*/
	CurrentDoor = Cast<ADoor>(UGameplayStatics::GetActorOfClass(GetWorld(),ADoor::StaticClass()));

	DeadTimer = 5.0f;
	HealthValue = 1.0f;
	isAlived = true;
	bIsAttacking = false;

	static ConstructorHelpers::FClassFinder<UUserWidget> SaveHelp(
		TEXT("/Game/UI/SaveInfo.SaveInfo_C"));

	HelpWidgetClass = SaveHelp.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> ExitHelp(
		TEXT("/Game/UI/ExitKeyInfo.ExitKeyInfo_C"));

	ExitWidgetClass = ExitHelp.Class;

	//static ConstructorHelpers::FClassFinder<UUserWidget> Result(
		//TEXT("/Game/UI/ExitKeyInfo.ExitKeyInfo_C"));

	//ResultWidgetClass = Result.Class;
}

/*void ANetCharacter::DeathOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("SetHP")));
	ABLOG_S(Warning);
	WarriorStat->SetHP(0);

}*/

// Called when the game starts or when spawned
void ANetCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HelpWidgetClass)
	{
		InfoWidget = CreateWidget<UUserWidget>(GetWorld(), HelpWidgetClass);

		if (InfoWidget)
		{
			InfoWidget->AddToViewport();
		}

	}

	if (ExitWidgetClass)
	{
		ExitInfoWidget = CreateWidget<UUserWidget>(GetWorld(), ExitWidgetClass);

		if (ExitInfoWidget)
		{
			ExitInfoWidget->AddToViewport();
		}

	}

	FullHealth = 1000.f;
	Health = FullHealth;
	HealthPercentage = 1.0f;
	bCanBeDamaged = true;

	FullSkill = 20.f;
	Skill = FullSkill;
	SkillPercentage = 1.f;
	PreviousSkill = SkillPercentage;
	SkillValue = 0.f;
	bCanUseSkill = true;

	
	if (SkillCurve)
	{
		ABLOG(Warning, TEXT("CURVE"));
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineCallback.BindUFunction(this, FName("SetSkillValue"));
		TimelineFinishedCallback.BindUFunction(this, FName("SetSkillState"));

		MyTimeline.AddInterpFloat(SkillCurve, TimelineCallback);
		MyTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
	}

	NetPlayerController = Cast<ANetPlayerController>(GetController());



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
		FStreamableDelegate::CreateUObject(this, &ANetCharacter::OnAssetLoadCompleted));

	SetWarriorState(ECharacterState::LOADING);


}

void ANetCharacter::SetWarriorState(ECharacterState NewState)
{
	ABCHECK(CurrentState != NewState);
	CurrentState = NewState;

	switch (CurrentState)
	{
	case ECharacterState::LOADING:
	{
		DisableInput(NetPlayerController);

		//NetPlayerController->GetHUDWidget()->BindCharacterStat(WarriorStat);

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
		GetCharacterMovement()->MaxWalkSpeed = 700.0f;
		EnableInput(NetPlayerController);

		break;
	}
	case ECharacterState::DEAD:
	{
		SetActorEnableCollision(false);
		GetMesh()->SetHiddenInGame(false);
		MyAnim->SetDeadAnim();
		SetCanBeDamaged(false);

		DisableInput(NetPlayerController);

		GetWorld()->GetTimerManager().
			SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void {
			NetPlayerController->RestartLevel();

				}), DeadTimer, false);

		break;
	}
	}
}


ECharacterState ANetCharacter::GetWarriorState() const
{
	return CurrentState;
}

void ANetCharacter::OnAssetLoadCompleted()
{
	//USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	//AssetStreamingHandle.Reset();
	//ABCHECK(nullptr != AssetLoaded);
	//GetMesh()->SetSkeletalMesh(AssetLoaded);

	SetWarriorState(ECharacterState::READY);


}

void ANetCharacter::SetControlMode(int32 ControlMode)
{
	if (ControlMode == 0)
	{
		ViewArm->bUsePawnControlRotation = true;

		//ViewArm->SetRelativeTransform(FTransform(FRotator(90, 0, (-88 + ViewRotator)), FVector(-5, -15, 10), FVector(0, 0, 0)));
		//Camera->SetRelativeTransform(FTransform(FRotator(ViewUp, 0, 0), FVector(0, 0, 0), FVector(0, 0, 0)));
		//bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		
	}
}

// Called every frame
void ANetCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult Hit;
	FVector Start = Camera->GetComponentLocation();

	FVector ForwardVector = Camera->GetForwardVector();
	FVector End = (ForwardVector * 200.f) + Start;
	FCollisionQueryParams CollisionParams;

	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
	{
		if (Hit.bBlockingHit)
		{
			if (Hit.GetActor()->GetClass()->IsChildOf(ASaveCom::StaticClass()))
			{
				InfoWidget->SetVisibility(ESlateVisibility::Visible);
				CurrentSave = Cast<ASaveCom>(Hit.GetActor());
			}
			else if (Hit.GetActor()->GetClass()->IsChildOf(AExitKey::StaticClass()))
			{
				ExitInfoWidget->SetVisibility(ESlateVisibility::Visible);
				CurrentKey = Cast<AExitKey>(Hit.GetActor());
			}
		}

		
	}
	else 
	{
		InfoWidget->SetVisibility(ESlateVisibility::Hidden);
		CurrentSave = NULL;

		ExitInfoWidget->SetVisibility(ESlateVisibility::Hidden);
		CurrentKey = NULL;
	}

	
	MyTimeline.TickTimeline(DeltaTime);

	if (IsAttacking || IsSAttacking)
	{
		//ABLOG(Warning, TEXT("TICK"));

		if (VelSum >= 200.0f) {
			Velocity = 0.0f;
			//ABLOG(Warning, TEXT("111"));
		}
		else {
			VelSum += Velocity * 1.5f;
			//ABLOG(Warning, TEXT("222"));
		}
		SetActorLocation(GetActorLocation() + GetControlRotation().Vector() * Velocity);


	}
	else {
		Velocity = 20.0f;
		VelSum = 0.f;
		//ABLOG(Warning, TEXT("Vel3"));
	}

}



// Called to bind functionality to input
void ANetCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &ANetCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("SAttack"), EInputEvent::IE_Pressed, this, &ANetCharacter::SAttack);
	PlayerInputComponent->BindAction(TEXT("RAttack"), EInputEvent::IE_Pressed, this, &ANetCharacter::RAttack);
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &ANetCharacter::Fire);
	PlayerInputComponent->BindAction(TEXT("Save"), EInputEvent::IE_Pressed, this, &ANetCharacter::SaveGame);
	PlayerInputComponent->BindAction(TEXT("GetKey"), EInputEvent::IE_Pressed, this, &ANetCharacter::GetKey);
	PlayerInputComponent->BindAction(TEXT("GoTitle"), EInputEvent::IE_Pressed, this, &ANetCharacter::GoTitle);
	PlayerInputComponent->BindAction(TEXT("Cheat_One"), EInputEvent::IE_Pressed, this, &ANetCharacter::Cheat_One);
	PlayerInputComponent->BindAction(TEXT("Cheat_Two"), EInputEvent::IE_Pressed, this, &ANetCharacter::Cheat_Two);
	PlayerInputComponent->BindAction(TEXT("Cheat_Three"), EInputEvent::IE_Pressed, this, &ANetCharacter::Cheat_Three);
	PlayerInputComponent->BindAction(TEXT("Cheat_Four"), EInputEvent::IE_Pressed, this, &ANetCharacter::Cheat_Four);
	PlayerInputComponent->BindAction(TEXT("Cheat_Five"), EInputEvent::IE_Pressed, this, &ANetCharacter::Cheat_Five);
	PlayerInputComponent->BindAction(TEXT("Cheat_Six"), EInputEvent::IE_Pressed, this, &ANetCharacter::Cheat_Six);
	PlayerInputComponent->BindAction(TEXT("Cheat_Seven"), EInputEvent::IE_Pressed, this, &ANetCharacter::Cheat_Seven);
	PlayerInputComponent->BindAction(TEXT("Cheat_Eight"), EInputEvent::IE_Pressed, this, &ANetCharacter::Cheat_Eight);
	PlayerInputComponent->BindAction(TEXT("Cheat_Nine"), EInputEvent::IE_Pressed, this, &ANetCharacter::Cheat_Nine);
	PlayerInputComponent->BindAction(TEXT("Cheat_Zero"), EInputEvent::IE_Pressed, this, &ANetCharacter::Cheat_Zero);
	PlayerInputComponent->BindAction(TEXT("Cheat_Y"), EInputEvent::IE_Pressed, this, &ANetCharacter::Cheat_Y);
	PlayerInputComponent->BindAction(TEXT("Cheat_U"), EInputEvent::IE_Pressed, this, &ANetCharacter::Cheat_U);
	PlayerInputComponent->BindAction(TEXT("Cheat_I"), EInputEvent::IE_Pressed, this, &ANetCharacter::Cheat_I); 
	PlayerInputComponent->BindAction(TEXT("Cheat_O"), EInputEvent::IE_Pressed, this, &ANetCharacter::Cheat_O);
	PlayerInputComponent->BindAction(TEXT("Cheat_P"), EInputEvent::IE_Pressed, this, &ANetCharacter::Cheat_P);
	PlayerInputComponent->BindAction(TEXT("Cheat_L"), EInputEvent::IE_Pressed, this, &ANetCharacter::Cheat_L);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &ANetCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &ANetCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ANetCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ANetCharacter::Turn);

}

void ANetCharacter::Cheat_One()
{
	SetActorLocation(FVector(-43329.0f, 25602.0f, -562.0f));
}

void ANetCharacter::Cheat_Two()
{
	SetActorLocation(FVector(-18484.0f, 26796.0f, -2806.0f));
}

void ANetCharacter::Cheat_Three()
{
	SetActorLocation(FVector(-21499.0f, 31844.0f, -469.0f));
}

void ANetCharacter::Cheat_Four()
{
	SetActorLocation(FVector(-28868.0f, 27612.0f, 662.0f));
}

void ANetCharacter::Cheat_Five()
{
	SetActorLocation(FVector(-20741.0f, 13290.0f, 1131.0f));
}

void ANetCharacter::Cheat_Six()
{
	SetActorLocation(FVector(3267.0f, -3688.0f, 1358.0f));
}

void ANetCharacter::Cheat_Seven()
{
	SetActorLocation(FVector(-6341.0f, 7896.0f, 1211.0f));
}

void ANetCharacter::Cheat_Eight()
{
	SetActorLocation(FVector(21285.0f, 6653.0f, 1557.0f));
}

void ANetCharacter::Cheat_Nine()
{
	SetActorLocation(FVector(41362.0f, 22786.0f, 1524.0f));
}

void ANetCharacter::Cheat_Zero()
{
	SetActorLocation(FVector(43567.0f, 37221.0f, 1514.0f));
}

void ANetCharacter::Cheat_Y()
{
	SetActorLocation(FVector(54406.0f, 27319.0f, 1524.0f));
}

void ANetCharacter::Cheat_U()
{
	SetActorLocation(FVector(56348.0f, 15996.0f, 1529.0f));
}

void ANetCharacter::Cheat_I()
{
	SetActorLocation(FVector(48672.0f, 39473.0f, 1514.0f));
}

void ANetCharacter::Cheat_O()
{
	SetActorLocation(FVector(51888.0f, 18286.0f, 6151.0f));
}

void ANetCharacter::Cheat_P()
{
	SetActorLocation(FVector(74660.0f, 18503.0f, 1524.0f));
}

void ANetCharacter::Cheat_L()
{
	SetActorLocation(FVector(83836.0f, 16071.0f, 1524.0f));
}

void ANetCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MyAnim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != MyAnim);

	MyAnim->OnMontageEnded.AddDynamic(this, &ANetCharacter::OnAttackMontageEnded);
	MyAnim->OnMontageEnded.AddDynamic(this, &ANetCharacter::OnSAttackMontageEnded);
	MyAnim->OnMontageEnded.AddDynamic(this, &ANetCharacter::OnRAttackMontageEnded);
	MyAnim->OnMontageEnded.AddDynamic(this, &ANetCharacter::OnFireMontageEnded);

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
			Velocity = 50.0f;
			VelSum = 0.f;
			beChecked = false;
		}
		else {
			StopAnimMontage();
			ABLOG(Warning, TEXT("STOP"));
		}
		});




	MyAnim->OnAttackHitCheck.AddUObject(this, &ANetCharacter::AttackCheck);
	MyAnim->OnSAttackHitCheck.AddUObject(this, &ANetCharacter::SAttackCheck);

	WarriorStat->OnHPIsZero.AddLambda([this]()->void {
		ABLOG(Warning, TEXT("OnHPIsZero"));
		MyAnim->SetDeadAnim();
		SetActorEnableCollision(false);
		});
}


float ANetCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	bCanBeDamaged = false;
	redFlash = true;
	UpdateMyHealth(-DamageAmount);
	DamageTimer();
	return DamageAmount;

	//float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	//WarriorStat->SetDamage(FinalDamage);

	//return FinalDamage;
}

void ANetCharacter::UpDown(float NewAxisValue)
{
	if (!IsAttacking && !IsSAttacking) {

		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
	}
	/*else {
		ABLOG(Warning, TEXT("Attacking"));
	}*/

}

void ANetCharacter::LeftRight(float NewAxisValue)
{
	if (!IsAttacking && !IsSAttacking) {
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
	}
	/*else {
		ABLOG(Warning, TEXT("Attacking"));
	}*/
}

void ANetCharacter::LookUp(float NewAxisValue)
{
	AddControllerPitchInput(NewAxisValue);
}

void ANetCharacter::Turn(float NewAxisValue)
{
	AddControllerYawInput(NewAxisValue);
}

void ANetCharacter::Attack()
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
		
		bIsAttacking = true;
		//[TODO] Action to server
		
		//beChecked = true;
		//MyAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}

}

void ANetCharacter::SAttack()
{
	if (IsSAttacking) return;

	MyAnim->PlaySAttackMontage();
	IsSAttacking = true;
}

void ANetCharacter::GoTitle()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title"));
}

void ANetCharacter::GetKey()
{
	if (CurrentKey)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Key_Sound, GetActorLocation());
	//ABLOG(Warning, TEXT("DDD"));
		if (KeyCount == 4) {
			CurrentDoor->DestructDoor();
			//UParticleSystem* Portal =
				//Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
					//TEXT("/Game/Effect/P_Portal.P_Portal")));

			//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Portal,
				//FVector((16657.025391f, 5849.000977f, 212.000000f)));

			ABLOG(Warning, TEXT("PR"));
		}
	++KeyCount;
	ABLOG(Warning, TEXT("Key : %d"), KeyCount);
	
	CurrentKey->DestructKey();
	if (sessionID == Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController())->GetSessionId())
		Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController())->SendDestructKey(sessionID, CurrentKey->ID);
	}
	else {
		//ABLOG(Warning, TEXT("NULL"));
		return;
	}
}

void ANetCharacter::RAttack()
{
	if (IsRAttacking) return;

	//if (!FMath::IsNearlyZero(Skill, 0.001f) && bCanUseSkill)
	//{
		ABLOG(Warning, TEXT("RATTACK"));
		
		UGameplayStatics::PlaySoundAtLocation(this, Magic_Sound, GetActorLocation());
		//MyAnim->PlayFireMontage();

		MyAnim->PlayRAttackMontage();
		
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(UMyMatineeCameraShake::StaticClass(), 1.f);
			//GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(MyShake, 1.f);
		IsRAttacking = true;
		//}

		MyTimeline.Stop();
		GetWorldTimerManager().ClearTimer(SkillTimerHandle);
		SetSkillChange(-20.f);
		GetWorldTimerManager().SetTimer(SkillTimerHandle, this, &ANetCharacter::UpdateSkill, 5.f, false);

		if(sessionID == Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController())->GetSessionId())
			Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController())->SendActionSkill(sessionID, 1);
	//}
}

void ANetCharacter::Fire()
{
	if (IsFireing) return;

	
	UGameplayStatics::PlaySoundAtLocation(this, Fire_Sound, GetActorLocation());
	MyAnim->PlayFireMontage();

	IsFireing = true;
	if (sessionID == Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController())->GetSessionId())
		Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController())->SendActionSkill(sessionID, 2);
}

void ANetCharacter::SaveGame()
{

	if (CurrentSave)
	{
		//Create an instance of our savegame class
		UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

		//Set the save game instance location equal to the players current location
		SaveGameInstance->PlayerLocation = this->GetActorLocation();

		//Save the savegameinstance
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MySlot"), 0);

		//Log a message to show we have saved the game
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Game Saved."));
	}
	else return;
}

void ANetCharacter::LoadGame()
{
	//Create an instance of our savegame class
	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	
	//Load the saved game into our savegameinstance variable
	SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot("MySlot",0));

	//Set the players position from the saved game file
	this->SetActorLocation(SaveGameInstance->PlayerLocation);

	//Log
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Game Loaded."));
}


float ANetCharacter::GetHealth()
{
	return HealthPercentage;
}

float ANetCharacter::GetSkill()
{
	return SkillPercentage;
}

/*FText ANetCharacter::GetHealthIntText()
{
	int32 HP = FMath::RoundHalfFromZero(HealthPercentage * 100);
	FString HPS = FString::FromInt(HP);
	FString HealthHUD = HPS + FString(TEXT("%"));
	FText HPText = FText::FromString(HealthHUD);
	return HPText;
}

FText ANetCharacter::GetSkillIntText()
{
	int32 SP = FMath::RoundHalfFromZero(SkillPercentage * 100);
	FString SPS = FString::FromInt(SP);
	FString FullSPS = FString::FromInt(FullSkill);
	FString SkillHUD = SPS + FString(TEXT("/") + FullSPS);
	FText SPText = FText::FromString(SkillHUD);
	return SPText;
}*/

void ANetCharacter::SetDamageState()
{
	bCanBeDamaged = true;
}

void ANetCharacter::DamageTimer()
{
	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &ANetCharacter::SetDamageState, 2.f, false);
}

void ANetCharacter::SetSkillValue()
{
	TimelineValue = MyTimeline.GetPlaybackPosition();
	CurveFloatValue = PreviousSkill + SkillValue * SkillCurve->GetFloatValue(TimelineValue);
	Skill = CurveFloatValue * FullHealth;
	Skill = FMath::Clamp(Skill, 0.f, FullSkill);
	SkillPercentage = CurveFloatValue;
	SkillPercentage = FMath::Clamp(SkillPercentage, 0.f, 1.f);

}

void ANetCharacter::SetSkillState()
{
	bCanUseSkill = true;
	SkillValue = 0.f;

}

bool ANetCharacter::PlayFlash()
{
	if (redFlash)
	{
		redFlash = false;
		return true;
	}

	return false;
}

/*void ANetCharacter::ReceivePointDamage(float Damage, const class UDamageType* DamageType, FVector HitLocation,
	FVector HitNormal, class UPrimitiveComponent* HitComponent, FName BoneName,
	FVector ShotFromDirection, class AController* InstigatedBy, AActor* DamageCauser,
	const FHitResult& HitInfo)
{
	bCanBeDamaged = false;
	redFlash = true;
	UpdateHealth(-Damage);
	DamageTimer();
}*/

void ANetCharacter::UpdateMyHealth(float HealthChange)
{
	Health = FMath::Clamp(Health += HealthChange, 0.0f, FullHealth);
	HealthPercentage = Health / FullHealth;

}

void ANetCharacter::UpdateSkill()
{
	PreviousSkill = SkillPercentage;
	SkillPercentage = Skill / FullSkill;
	SkillValue = 1.f;
	MyTimeline.PlayFromStart();
}

void ANetCharacter::SetSkillChange(float SkillChange)
{
	bCanUseSkill = false;
	PreviousSkill = SkillPercentage;
	SkillValue = SkillChange / FullSkill;
	MyTimeline.PlayFromStart();
}

void ANetCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//ABCHECK(IsAttacking);
	//ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
}

void ANetCharacter::OnSAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//ABCHECK(IsSAttacking);
	//ABCHECK(CurrentCombo > 0);
	IsSAttacking = false;
	//AttackEndComboState();
}

void ANetCharacter::OnRAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//ABCHECK(IsSAttacking);
	//ABCHECK(CurrentCombo > 0);
	IsRAttacking = false;
	//AttackEndComboState();
}

void ANetCharacter::OnFireMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//ABCHECK(IsSAttacking);
	//ABCHECK(CurrentCombo > 0);
	IsFireing = false;
	//AttackEndComboState();
}

void ANetCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void ANetCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void ANetCharacter::AttackCheck()
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

	if (bResult)
	{
		if (HitResult.Actor.IsValid())
		{
			UGameplayStatics::PlaySoundAtLocation(this, Sword_Sound, GetActorLocation());
			if (HitResult.Actor->IsA(AATank::StaticClass()))
			{
				GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(UMyMatineeCameraShake::StaticClass(), 1.f);
				AATank* Monster = Cast<AATank>(HitResult.Actor);
				if (Monster)
				{

					ANetPlayerController* PlayerController = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
					bool isMaster = PlayerController->HitMonster(Monster->ID);
					if (isMaster) {
						ABLOG(Warning, TEXT("Hit Actor Name: %s"), *HitResult.Actor->GetName());
						FDamageEvent DamageEvent;

						HitResult.Actor->TakeDamage(10.f, DamageEvent, GetController(), this);
					}
				}


			}

			/*if (HitResult.Actor->IsA(ABossTank::StaticClass()))
			{
				ABossTank* Monster = Cast<ABossTank>(HitResult.Actor);
				if (Monster)
				{

					ANetPlayerController* PlayerController = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
					bool isMaster = PlayerController->HitMonster(Monster->ID);
					if (isMaster) {
						ABLOG(Warning, TEXT("Hit Actor Name: %s"), *HitResult.Actor->GetName());
						FDamageEvent DamageEvent;

						HitResult.Actor->TakeDamage(10.f, DamageEvent, GetController(), this);
					}
				}


			}*/

			ANetPlayerController* PlayerController = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(10.f, DamageEvent, GetController(), this);
		}
	}
}


void ANetCharacter::SAttackCheck()
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
		// ���� Hit
		//if (HitResult.Actor.IsValid())
		//{
		//	ABLOG(Warning, TEXT("Hit Actor Name: %s"), *HitResult.Actor->GetName());
		//	FDamageEvent DamageEvent;

		//	HitResult.Actor->TakeDamage(WarriorStat->GetSAttack(), DamageEvent, GetController(), this);
		//}
		// Netwrok Hit
		//if (HitResult.Actor.IsValid())
		//{
		//	AATank* Monster = Cast<AATank>(HitResult.Actor);
		//	if (Monster)
		//	{
		//		ANetPlayerController* PlayerController = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
		//		PlayerController->HitMonster(Monster->Id);
		//	}
		//}
	}
}


// Network
int ANetCharacter::GetSessionId()
{
	return sessionID;
}

void ANetCharacter::SetSessionId(int SessionId_)
{
	sessionID = SessionId_;
}

bool ANetCharacter::GetIsAlived()
{
	return isAlived;
}

void ANetCharacter::SetIsAlived(bool _isAlived)
{
	isAlived = _isAlived;
}

void ANetCharacter::PlayAttackAnim()
{
	return MyAnim->PlayAttackMontage();
}

void ANetCharacter::PlayTakeDamageAnim()
{
	//[TODO] Dead Anim���� ���� �ʿ�
	return MyAnim->PlayDamagedMontage();
}

bool ANetCharacter::GetIsAttacking()
{
	bool returnValue = bIsAttacking;
	bIsAttacking = false;
	return returnValue;
}

void ANetCharacter::UpdateHealth(float _healthValue)
{
	HealthValue = _healthValue;
}

float ANetCharacter::GetHealthValue()
{
	return HealthValue;
}

void ANetCharacter::ChangeUELevel(int level)
{
	UELevel = level;
}