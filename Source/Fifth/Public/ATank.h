// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "GameFramework/Character.h"
#include "ATank.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class FIFTH_API AATank : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AATank();

	void SetTankState(ECharacterState NewState);
	ECharacterState GetTankState() const;

	void PlayAttackAnim();
	void PlayTakeDamageAnim();
	// 몬스터를 위치로 이동
	void MoveToLocation(const FVector& dest);
	void StartAction();
	bool GetIsAttacking();

	//int		Id;				// 고유 id
	bool isStartAction = false;
	bool isMasterStartAction = false;

	UPROPERTY(VisibleAnywhere)
		class USphereComponent* PlayerLocationSphere;

	float SphereRadius;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
			const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SetControlMode(int32 ControlMode);

public:	
	UPROPERTY(VisibleAnywhere, Category = UI)
		class UWidgetComponent* HPBarWidget;
	UPROPERTY(EditAnywhere, Category = ID)
		int32 ID;
	UPROPERTY(EditAnywhere, Category = UELevel)
		int32 UELevel;


	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class
		AController* EventInstigator, AActor* DamageCauser) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = Stat)
		class UTankStatComponent* TankStat;

	virtual void PossessedBy(AController* NewController) override;
	void Attack();
	void Damaged();

	float GetTankHpRatio();
	void SetTankHpRatio(float ratio);

	FOnAttackEndDelegate OnAttackEnd;

	
	

private:
	

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		bool IsAttacking;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Damage, Meta = (AllowPrivateAccess = true))
		bool IsDamaging;

	UPROPERTY()
		class UATankAnimInstance* ATAnim;

	UFUNCTION()
		void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
		void OnDamagedMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void AttackCheck();
	void OnAssetLoadCompleted();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		float AttackRadius;

	FSoftObjectPath CharacterAssetToLoad = FSoftObjectPath(nullptr);
	TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
		ECharacterState CurrentState;

	int32 AssetIndex = 0;

	UPROPERTY()
		class ATankAIController* TankAIController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		float DeadTimer;

	FTimerHandle DeadTimerHandle = {};

};
