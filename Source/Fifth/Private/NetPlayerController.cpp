// Fill out your copyright notice in the Description page of Project Settings.

#include "NetPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MyHUDWidget.h"
#include "WarriorOfFire.h"
//#include "Blueprint/UserWidget.h"
#include <string>

ANetPlayerController::ANetPlayerController()
{
	SessionId = FMath::RandRange(0, 10000);

	// 서버와 연결
	Socket = ClientSocket::GetSingleton();
	Socket->InitSocket();
	bIsConnected = Socket->Connect("127.0.0.1", 8000);
	if (bIsConnected)
	{
		UE_LOG(LogClass, Log, TEXT("IOCP Server connect success!"));
		Socket->SetPlayerController(this);
	}

	bIsChatNeedUpdate = false;
	bNewPlayerEntered = false;

	WhoToSpawn = AWarriorOfFire::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
}

ANetPlayerController::~ANetPlayerController()
{
}

void ANetPlayerController::ChatToServer(FString Text)
{
	UE_LOG(LogClass, Log, TEXT("%s"), *Text);
	Socket->SendChat(SessionId, TCHAR_TO_UTF8(*Text));
}

int ANetPlayerController::GetSessionId()
{
	return SessionId;
}

void ANetPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);



	if (!bIsConnected) return;

	// 플레이어 정보 송신
	if (!SendPlayerInfo()) return;

	// 월드 동기화
	if (!UpdateWorldInfo()) return;

	// 채팅 동기화
	if (bIsChatNeedUpdate)
	{
		//UpdateChat();
	}

	if (bNewPlayerEntered)
	{
		UpdateNewPlayer();
	}
}

void ANetPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	UE_LOG(LogClass, Log, TEXT("BeginPlay NetPlayer Start"));
	//if (HUDWidgetClass != nullptr)
	//{
	//	CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
	//	if (CurrentWidget != nullptr)
	//	{
	//		CurrentWidget->AddToViewport();
	//	}
	//}

	// 캐릭터 등록
	auto player = Cast<ANetCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	UE_LOG(LogClass, Log, TEXT("BeginPlay GetPlayerCharacter"));
	if (!player)
		return;

	player->SetSessionId(SessionId);
	UE_LOG(LogClass, Log, TEXT("BeginPlay SetSessionId"));

	auto MyLocation = player->GetActorLocation();
	auto MyRotation = player->GetActorRotation();

	cCharacter tempCharacter;
	// 위치
	tempCharacter.SessionId = SessionId;
	tempCharacter.X = MyLocation.X;
	tempCharacter.Y = MyLocation.Y;
	tempCharacter.Z = MyLocation.Z;
	// 회전
	tempCharacter.Yaw = MyRotation.Yaw;
	tempCharacter.Pitch = MyRotation.Pitch;
	tempCharacter.Roll = MyRotation.Roll;
	// 속도
	tempCharacter.VX = 0;
	tempCharacter.VY = 0;
	tempCharacter.VZ = 0;
	// 속성
	tempCharacter.IsAlive = player->GetIsAlived();
	//tempCharacter.HealthValue = player->HealthValue;
	//tempCharacter.IsAttacking = player->IsAttacking();

	UE_LOG(LogClass, Log, TEXT("EnrollPlayer start"));
	Socket->EnrollPlayer(tempCharacter);
	UE_LOG(LogClass, Log, TEXT("EnrollPlayer start"));

	// Recv 스레드 시작
	Socket->StartListen();
	UE_LOG(LogClass, Log, TEXT("BeginPlay End"));
}

void ANetPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Socket->LogoutPlayer(SessionId);
	Socket->CloseSocket();
	Socket->StopListen();
}

AActor* ANetPlayerController::FindActorBySessionId(TArray<AActor*> ActorArray, const int& sessionId)
{
	for (const auto& Actor : ActorArray)
	{
		ANetCharacter* swc = Cast<ANetCharacter>(Actor);
		if (swc && swc->GetSessionId() == sessionId)
			return Actor;
	}
	return nullptr;
}

void ANetPlayerController::HitCharacter(const int& sessionID, const ANetCharacter* DamagedCharacter)
{
	UE_LOG(LogClass, Log, TEXT("Damaged Called %d"), sessionID);

	UWorld* const world = GetWorld();

	if (DamagedCharacter != nullptr)
	{
		Socket->DamagePlayer(sessionID);
	}
}

void ANetPlayerController::RecvWorldInfo(cCharactersInfo* ci_)
{
	if (ci_ != nullptr)
	{
		ci = ci_;
	}

}

void ANetPlayerController::RecvChat(const string* chat_)
{
	if (chat_ != nullptr)
	{
		chat = chat_;
		bIsChatNeedUpdate = true;
	}
}

void ANetPlayerController::RecvNewPlayer(cCharactersInfo* NewPlayer_)
{
	if (NewPlayer_ != nullptr)
	{
		bNewPlayerEntered = true;
		NewPlayer = NewPlayer_;
	}
}

bool ANetPlayerController::SendPlayerInfo()
{
	auto tempPlayer = Cast<ANetCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!tempPlayer)
		return false;

	// 플레이어의 위치를 가져옴	
	const auto& Location = tempPlayer->GetActorLocation();
	const auto& Rotation = tempPlayer->GetActorRotation();
	const auto& Velocity = tempPlayer->GetVelocity();
	//const bool IsFalling = tempPlayer->IsFalling();

	cCharacter tempCharacter;
	tempCharacter.SessionId = SessionId;

	tempCharacter.X = Location.X;
	tempCharacter.Y = Location.Y;
	tempCharacter.Z = Location.Z;

	tempCharacter.Yaw = Rotation.Yaw;
	tempCharacter.Pitch = Rotation.Pitch;
	tempCharacter.Roll = Rotation.Roll;

	tempCharacter.VX = Velocity.X;
	tempCharacter.VY = Velocity.Y;
	tempCharacter.VZ = Velocity.Z;

	//tempCharacter.IsAlive = tempPlayer->IsAlive();
	//tempCharacter.HealthValue = tempPlayer->HealthValue;
	//tempCharacter.IsAttacking = tempPlayer->IsAttacking();

	Socket->SendPlayer(tempCharacter);

	return true;
}

bool ANetPlayerController::UpdateWorldInfo()
{
	UWorld* const world = GetWorld();
	if (world == nullptr)
		return false;

	if (ci == nullptr)
		return false;

	// 플레이어 업데이트
	UpdatePlayerInfo(ci->players[SessionId]);

	// 다른 플레이어 업데이트
	TArray<AActor*> SpawnedCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANetCharacter::StaticClass(), SpawnedCharacters);

	for (const auto& character : SpawnedCharacters)
	{
		ANetCharacter* OtherPlayer = Cast<ANetCharacter>(character);

		if (!OtherPlayer || OtherPlayer->GetSessionId() == -1 || OtherPlayer->GetSessionId() == SessionId)
		{
			continue;
		}

		cCharacter* info = &ci->players[OtherPlayer->GetSessionId()];

		if (info->IsAlive)
		{
			//if (OtherPlayer->HealthValue != info->HealthValue)
			//{
			//	// 피격 파티클 소환
			//	FTransform transform(OtherPlayer->GetActorLocation());
			//	UGameplayStatics::SpawnEmitterAtLocation(
			//		world, HitEmiiter, transform, true
			//	);
			//	// 피격 애니메이션 플레이
			//	OtherPlayer->PlayDamagedAnimation();
			//	OtherPlayer->HealthValue = info->HealthValue;
			//}

			//// 공격중일때 타격 애니메이션 플레이
			//if (info->IsAttacking)
			//{
			//	UE_LOG(LogClass, Log, TEXT("Other character is hitting"));
			//	OtherPlayer->PlayHitAnimation();
			//}

			FVector CharacterLocation;
			CharacterLocation.X = info->X;
			CharacterLocation.Y = info->Y;
			CharacterLocation.Z = info->Z;

			FRotator CharacterRotation;
			CharacterRotation.Yaw = info->Yaw;
			CharacterRotation.Pitch = info->Pitch;
			CharacterRotation.Roll = info->Roll;

			FVector CharacterVelocity;
			CharacterVelocity.X = info->VX;
			CharacterVelocity.Y = info->VY;
			CharacterVelocity.Z = info->VZ;

			OtherPlayer->AddMovementInput(CharacterVelocity);
			OtherPlayer->SetActorRotation(CharacterRotation);
			OtherPlayer->SetActorLocation(CharacterLocation);
		}
		else
		{
			UE_LOG(LogClass, Log, TEXT("Destroy Actor"));
			FTransform transform(character->GetActorLocation());
			UGameplayStatics::SpawnEmitterAtLocation(
				world, DestroyEmiiter, transform, true
			);
			character->Destroy();
		}
	}

	return true;
}

void ANetPlayerController::UpdatePlayerInfo(const cCharacter& info)
{
	auto tempPlayer = Cast<ANetCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	UWorld* const world = GetWorld();

	if (!info.IsAlive)
	{
		UE_LOG(LogClass, Log, TEXT("Player Die"));
		FTransform transform(tempPlayer->GetActorLocation());
		UGameplayStatics::SpawnEmitterAtLocation(
			world, DestroyEmiiter, transform, true
		);
		tempPlayer->Destroy();

		//CurrentWidget->RemoveFromParent();
		//GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
		//if (GameOverWidget != nullptr)
		//{
		//	GameOverWidget->AddToViewport();
		//}
	}
	else
	{
		// 캐릭터 속성 업데이트
		//if (tempPlayer->HealthValue != info.HealthValue)
		//{
		//	UE_LOG(LogClass, Log, TEXT("Player damaged"));
		//	// 피격 파티클 스폰
		//	FTransform transform(tempPlayer->GetActorLocation());
		//	UGameplayStatics::SpawnEmitterAtLocation(
		//		world, HitEmiiter, transform, true
		//	);
		//	// 피격 애니메이션 스폰
		//	tempPlayer->PlayDamagedAnimation();
		//	tempPlayer->HealthValue = info.HealthValue;
		//}
	}
}

//void ANetPlayerController::UpdateChat()
//{
//	UChatWindowWidget* temp = Cast<UChatWindowWidget>(CurrentWidget);
//	if (temp != nullptr)
//	{
//		UE_LOG(LogClass, Log, TEXT("Casting"));
//		temp->CallUpdateChat(
//			FText::FromString(*FString(chat->c_str()))
//		);
//	}
//	bIsChatNeedUpdate = false;
//}

void ANetPlayerController::UpdateNewPlayer()
{
	UWorld* const world = GetWorld();

	// 월드 내 OtherNetworkCharacter 불러옴
	TArray<AActor*> SpawnedCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANetCharacter::StaticClass(), SpawnedCharacters);

	for (const auto& kvp : NewPlayer->players)
	{
		if (kvp.first == SessionId)
			continue;

		const cCharacter* player = &kvp.second;
		if (player->IsAlive)
		{
			auto Actor = FindActorBySessionId(SpawnedCharacters, player->SessionId);
			if (Actor == nullptr)
			{
				FVector spawnLocation;
				spawnLocation.X = player->X;
				spawnLocation.Y = player->Y;
				spawnLocation.Z = player->Z;

				FRotator spawnRotation;
				spawnRotation.Yaw = player->Yaw;
				spawnRotation.Pitch = player->Pitch;
				spawnRotation.Roll = player->Roll;

				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = this->GetPawn();
				SpawnParams.Name = FName(*FString(to_string(player->SessionId).c_str()));

				ANetCharacter* SpawnCharacter = world->SpawnActor<ANetCharacter>(WhoToSpawn, spawnLocation, spawnRotation, SpawnParams);
				SpawnCharacter->SpawnDefaultController();
				SpawnCharacter->SetSessionId(player->SessionId);
			}
		}
	}

	bNewPlayerEntered = false;
	NewPlayer = nullptr;
}


void ANetPlayerController::OnPossess(APawn* aPawn)
{
	ABLOG_S(Warning);
	Super::OnPossess(aPawn);
}