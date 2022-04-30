
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ClientSocket.h"
#include "NetCharacter.generated.h"

UCLASS()
class FIFTH_API ANetCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
public:
	ANetCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	// 체력
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Properties")
		float HealthValue;

	// 에너지
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Properties")
		float EnergyValue;

	// 기분
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Properties")
		float MoodValue;

	// 체력 업데이트
	UFUNCTION(BlueprintCallable, Category = "Properties")
		void UpdateHealth(float HealthChange);

	// 체력 가져오기
	UFUNCTION(BlueprintPure, Category = "Properties")
		float GetHealth();

	UFUNCTION(BlueprintPure, Category = "Properties")
		bool IsAlive();

	bool IsFalling();

	UFUNCTION(BlueprintCallable)
		int GetSessionId();

	void SetSessionId(int SessionId);

	// 피격 애니메이션
	UFUNCTION(BlueprintImplementableEvent)
		void PlayDamagedAnimation();

	// 타격 애니메이션
	UFUNCTION(BlueprintImplementableEvent)
		void PlayHitAnimation();

	UFUNCTION(BlueprintCallable)
		void HitOtherCharacter();

	UFUNCTION(BlueprintCallable)
		void SetAttacking(bool attack);

	bool IsAttacking();

protected:

	/** Resets HMD orientation in VR. */
	//void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);



protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	int		SessionId;
	bool	bIsAlive;
	bool	bIsAttacking;

	FTimerHandle HitTimerHandle;
	bool HitEnable;
	void ResetHitEnable();
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
private:
	// 플레이어 캐릭터 점프
	void Jump();
};

