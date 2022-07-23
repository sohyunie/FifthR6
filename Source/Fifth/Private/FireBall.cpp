// Fill out your copyright notice in the Description page of Project Settings.


#include "FireBall.h"
#include "Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "ATank.h"
#include "BossTank.h"

// Sets default values
AFireBall::AFireBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(15.0f);
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("FireBall"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &AFireBall::OnHit);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this,
		&AFireBall::OnOverlapBegin);
	RootComponent = CollisionComponent;

	Capsule = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Capsule"));
	Capsule ->SetupAttachment(CollisionComponent);

	

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FireBall(
		TEXT("/Game/FireBall/Models/TestCapsule.TestCapsule"));

	if (FireBall.Succeeded())
	{
		Capsule->SetStaticMesh(FireBall.Object);
	}

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 3000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.0f;
	ProjectileMovementComponent->ProjectileGravityScale = 0;

	InitialLifeSpan = 3.0f;

	
	
}

void AFireBall::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void AFireBall::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	/*if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		Destroy();
	}*/

	if (OtherActor->IsA(AATank::StaticClass()))
	{
		UNiagaraSystem* FireHitEffect =
			Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(), NULL,
				TEXT("/Game/FireBall/NiagaraSystems/NS_Hit_Electric.NS_Hit_Electric")));
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FireHitEffect,
			OtherActor->GetActorLocation(), this->GetActorRotation());

		AFireBall::Destroy();
		UGameplayStatics::ApplyPointDamage(OtherActor, 50.0f, OtherActor->GetActorLocation(), SweepResult, nullptr, this, nullptr);

		/*ABLOG(Warning, TEXT("HIT!!!"));
		bCanApplyDamage = true;
		MyCharacter = Cast<AActor>(OtherActor);
		MyHit = SweepResult;
		//GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ACampFire::ApplyFireDamage, 2.2f, true, 0.0f);

		UGameplayStatics::ApplyPointDamage(OtherActor, 100.0f, OtherActor->GetActorLocation(), SweepResult, nullptr, this, nullptr);
		bCanApplyDamage = false;
		//AOverlapRangeActor::Destroy();*/
	}
}

void AFireBall::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsA(AATank::StaticClass()))
	{
		ABLOG(Warning, TEXT("TANKHIT!!!"));

		UNiagaraSystem* FireHitEffect =
			Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(), NULL,
				TEXT("/Game/FireBall/NiagaraSystems/NS_Hit_Electric.NS_Hit_Electric")));
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FireHitEffect,
			OtherActor->GetActorLocation(), this->GetActorRotation());

		AFireBall::Destroy();
		UGameplayStatics::ApplyPointDamage(OtherActor, 50.0f, OtherActor->GetActorLocation(), Hit, nullptr, this, nullptr);

	}

	//if (OtherActor != this /* && OtherComponent->IsSimulatingPhysics()*/)
	//{
		//ABLOG(Warning, TEXT("!!!"));
		//OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
	//}
}

// Called when the game starts or when spawned
void AFireBall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFireBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

