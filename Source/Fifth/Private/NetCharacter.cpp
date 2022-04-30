// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "NetCharacter.h"
//#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
//#include "Classes/Components/SphereComponent.h"
#include "Engine/World.h"
#include "NetPlayerController.h"
#include "TimerManager.h"

//////////////////////////////////////////////////////////////////////////
// ANetCharacter

ANetCharacter::ANetCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)		

	// 체력, 에너지, 기분 기정
	HealthValue = 0.5f;
	EnergyValue = 0.5f;
	MoodValue = 0.5f;

	bIsAlive = true;
	bIsAttacking = false;
	SessionId = -1;
	HitEnable = true;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANetCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	// PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ANetCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Hit", IE_Pressed, this, &ANetCharacter::HitOtherCharacter);

	PlayerInputComponent->BindAxis("MoveForward", this, &ANetCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANetCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ANetCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ANetCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ANetCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ANetCharacter::TouchStopped);

	// VR headset functionality
	//PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ANetCharacter::OnResetVR);
}

void ANetCharacter::ResetHitEnable()
{
	HitEnable = true;
}

void ANetCharacter::Jump()
{
	ACharacter::Jump();

	if (!GetCharacterMovement()->IsFalling())
	{
		if (EnergyValue > 0)
		{
			EnergyValue -= 0.05f;
		}
		else
		{
			EnergyValue = 0;
		}
	}
}

void ANetCharacter::UpdateHealth(float HealthChange)
{
	HealthValue += HealthChange;
}

float ANetCharacter::GetHealth()
{
	return HealthValue;
}

bool ANetCharacter::IsAlive()
{
	return bIsAlive;
}

bool ANetCharacter::IsFalling()
{
	return GetCharacterMovement()->IsFalling();
}

int ANetCharacter::GetSessionId()
{
	return SessionId;
}

void ANetCharacter::SetSessionId(int SessionId_)
{
	SessionId = SessionId_;
}

void ANetCharacter::SetAttacking(bool attack)
{
	bIsAttacking = attack;
}

bool ANetCharacter::IsAttacking()
{
	return bIsAttacking;
}

//void ANetCharacter::OnResetVR()
//{
//	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
//}

void ANetCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ANetCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ANetCharacter::HitOtherCharacter()
{
	if (HitEnable)
	{
		HitEnable = false;
		GetWorldTimerManager().SetTimer(HitTimerHandle, this, &ANetCharacter::ResetHitEnable, 0.5f, false, 0.5f);
	}
	else
	{
		return;
	}


	// Sphere 내 다른 캐릭터의 목록을 가져온다
	// Hit 잠시 주석
	//TArray<UActorComponent*> Comps;
	//TArray<AActor*> NearCharacters;
	//this->GetComponents(Comps);
	//for (auto Comp : Comps)
	//{
	//	USphereComponent* HitSphere = Cast<USphereComponent>(Comp);
	//	if (HitSphere)
	//	{
	//		HitSphere->GetOverlappingActors(NearCharacters);
	//		break;
	//	}
	//}

	// 오버래핑된 캐릭터들에게 Hit 이벤트를 작동한다
	//for (auto Character : NearCharacters)
	//{
	//	ANetCharacter* OtherCharacter = Cast<ANetCharacter>(Character);
	//	if (OtherCharacter && OtherCharacter->GetSessionId() != -1 && OtherCharacter->GetSessionId() != SessionId)
	//	{
	//		ANetPlayerController* PlayerController = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
	//		PlayerController->HitCharacter(OtherCharacter->GetSessionId(), OtherCharacter);
	//	}
	//}
}

void ANetCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ANetCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ANetCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ANetCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
