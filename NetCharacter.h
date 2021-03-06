// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "ClientSocket.h"
#include "GameFramework/Character.h"
#include "NetCharacter.generated.h"

UCLASS()
class FIFTH_API ANetCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANetCharacter();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetControlMode(int32 ControlMode);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class
		AController* EventInstigator, AActor* DamageCauser) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
		UStaticMeshComponent* Weapon;

	UPROPERTY(VisibleAnywhere, Category = Stat)
		class UWarriorStatComponent* WarriorStat;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		UCameraComponent* Camera;

	//UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SAttack, Meta = (AllowPrivateAccess = true))
		//bool SAttackCheck{ false };

	void SetWarriorState(ECharacterState NewState);
	ECharacterState GetWarriorState() const;

	int GetSessionId();
	void SetSessionId(int SessionId);
	bool GetIsAlived();
	void SetIsAlived(bool _isAlived);
	float GetHealthValue();
	void UpdateHealth(float _healthValue);

	void PlayAttackAnim();
	void PlayTakeDamageAnim();
	bool GetIsAttacking();

private:
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);

	void Attack();
	void SAttack();

	UFUNCTION()
		void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
		void OnSAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void AttackStartComboState();
	void AttackEndComboState();
	void AttackCheck();
	void SAttackCheck();

	void OnAssetLoadCompleted();

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SAttack, Meta = (AllowPrivateAccess = true))
		bool IsSAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Damage, Meta = (AllowPrivateAccess = true))
		bool IsDamaging;

	//???? ?????? ???? ???? ???? ????
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		bool beChecked;

	double VelSum = 0.f;
	double Velocity = 5.f;


	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		bool CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		bool IsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		int32 CurrentCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		int32 MaxCombo;

	UPROPERTY()
		class UMyAnimInstance* MyAnim;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		float AttackRadius;

	FSoftObjectPath CharacterAssetToLoad = FSoftObjectPath(nullptr);
	TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;


	int32 AssetIndex = 0;
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State,
		Meta = (AllowPrivateAccess = true))
		ECharacterState CurrentState;

	//UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State,
		//Meta = (AllowPrivateAccess = true))
		//bool bIsPlayer;

	UPROPERTY()
		class ANetPlayerController* NetPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, Meta = (AllowPrivateAccess = true))
		float DeadTimer;

	FTimerHandle DeadTimerHandle = {};

	// network
	int sessionID;
	bool isAlived;
	bool bIsAttacking;
	float HealthValue;
};