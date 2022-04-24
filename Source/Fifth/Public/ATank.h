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

	UPROPERTY(VisibleAnywhere, Category = Stat)
		class UTankStatComponent* TankStat;

	virtual void PossessedBy(AController* NewController) override;
	void Attack();
	void Damaged();

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

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		float AttackRadius;


};
