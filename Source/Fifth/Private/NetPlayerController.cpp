// Fill out your copyright notice in the Description page of Project Settings.

#include "NetPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MyHUDWidget.h"
#include "WarriorOfFire.h"
#include "ATank.h"
//#include "Blueprint/UserWidget.h"
#include <string>

ANetPlayerController::ANetPlayerController()
{
	SessionId = FMath::RandRange(0, 10000);

	// ������ ����
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

	// �ӽ� ��ƼŬ
	DestroyEmiiter = Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
		TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Sparks.P_Sparks'")));
	HitEmiiter = Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
		TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Sparks.P_Sparks'")));

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

	// �÷��̾� ���� �۽�
	//if (!SendPlayerInfo()) return;

	// ���� ����ȭ
	if (!UpdateWorldInfo()) return;

	// ���� �� �۽�
	if (!SendMonsterSet()) return;

	// ä�� ����ȭ
	if (bIsChatNeedUpdate)
	{
		//UpdateChat();
	}

	if (bNewPlayerEntered)
	{
		UpdateNewPlayer();
	}

	// ���� ������Ʈ
	if (ci->players[SessionId].UELevel != 0)
		UpdateMonsterSet();
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

	// ĳ���� ���
	auto player = Cast<ANetCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	UE_LOG(LogClass, Log, TEXT("BeginPlay GetPlayerCharacter"));
	if (!player)
		return;

	player->SetSessionId(SessionId);
	UE_LOG(LogClass, Log, TEXT("BeginPlay SetSessionId"));

	auto MyLocation = player->GetActorLocation();
	auto MyRotation = player->GetActorRotation();

	cCharacter tempCharacter;
	// ��ġ
	tempCharacter.SessionId = SessionId;
	tempCharacter.X = MyLocation.X;
	tempCharacter.Y = MyLocation.Y;
	tempCharacter.Z = MyLocation.Z;
	// ȸ��
	tempCharacter.Yaw = MyRotation.Yaw;
	tempCharacter.Pitch = MyRotation.Pitch;
	tempCharacter.Roll = MyRotation.Roll;
	// �ӵ�
	tempCharacter.VX = 0;
	tempCharacter.VY = 0;
	tempCharacter.VZ = 0;
	// �Ӽ�
	tempCharacter.IsAlive = player->GetIsAlived();
	tempCharacter.IsAttacking = player->GetIsAttacking();
	tempCharacter.HealthValue = player->GetHealthValue();

	tempCharacter.UELevel = 1; // [TODO] UE Level ID �ʿ�.

	UE_LOG(LogClass, Log, TEXT("EnrollPlayer"));
	Socket->EnrollPlayer(tempCharacter);

	// Recv ������ ����
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

void ANetPlayerController::HitMonster(const int& MonsterId)
{
	UE_LOG(LogClass, Log, TEXT("Monster Hit Called %d"), MonsterId);

	Socket->HitMonster(MonsterId);
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

void ANetPlayerController::RecvMonsterSet(MonsterSet* MonstersInfo_)
{
	if (MonstersInfo_ != nullptr)
	{
		MonsterSetInfo = MonstersInfo_;
	}
}

void ANetPlayerController::RecvDestroyMonster(Monster* MonsterInfo_)
{
	if (MonsterInfo_ != nullptr)
	{
		MonsterInfo = MonsterInfo_;
		bIsNeedToDestroyMonster = true;
	}
}

bool ANetPlayerController::SendPlayerInfo()
{
	auto tempPlayer = Cast<ANetCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!tempPlayer)
		return false;

	// �÷��̾��� ��ġ�� ������	
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

	tempCharacter.IsAlive = tempPlayer->GetIsAlived();
	tempCharacter.IsAttacking = tempPlayer->GetIsAttacking();
	tempCharacter.HealthValue = tempPlayer->GetHealthValue();

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

	// �÷��̾� ������Ʈ
	UpdatePlayerInfo(ci->players[SessionId]);

	// �ٸ� �÷��̾� ������Ʈ
	TArray<AActor*> SpawnedCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANetCharacter::StaticClass(), SpawnedCharacters);

	if (false)
	{	
		//for (auto & player : ci->players)
		//{			
		//	if (player.first == SessionId || !player.second.IsAlive)
		//		continue;

		//	FVector SpawnLocation;
		//	SpawnLocation.X = player.second.X;
		//	SpawnLocation.Y = player.second.Y;
		//	SpawnLocation.Z = player.second.Z;

		//	FRotator SpawnRotation;
		//	SpawnRotation.Yaw = player.second.Yaw;
		//	SpawnRotation.Pitch = player.second.Pitch;
		//	SpawnRotation.Roll = player.second.Roll;

		//	FActorSpawnParameters SpawnParams;
		//	SpawnParams.Owner = this;
		//	SpawnParams.Instigator = GetPawn();
		//	SpawnParams.Name = FName(*FString(to_string(player.second.SessionId).c_str()));

		//	ANetPlayerController* SpawnCharacter = world->SpawnActor<ASungminWorldCharacter>(WhoToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
		//	SpawnCharacter->SpawnDefaultController();

		//	SpawnCharacter->SessionId = player.second.SessionId;
		//	SpawnCharacter->HealthValue = player.second.HealthValue;
		//	SpawnCharacter->IsAlive = player.second.IsAlive;
		//	SpawnCharacter->IsAttacking = player.second.IsAttacking;	
		//}

		//nPlayers = CharactersInfo->players.size();
	} 
	else
	{
		for (auto& Character : SpawnedCharacters)
		{
			ANetCharacter* OtherPlayer = Cast<ANetCharacter>(Character);

			if (!OtherPlayer || OtherPlayer->GetSessionId() == -1 || OtherPlayer->GetSessionId() == SessionId)
			{
				continue;
			}

			cCharacter * info = &ci->players[OtherPlayer->GetSessionId()];

			if (info->IsAlive)
			{
				if (OtherPlayer->GetHealthValue() != info->HealthValue)
				{
					UE_LOG(LogClass, Log, TEXT("other player damaged."));
					// �ǰ� ��ƼŬ ��ȯ
					FTransform transform(OtherPlayer->GetActorLocation());
					UGameplayStatics::SpawnEmitterAtLocation(
						world, HitEmiiter, transform, true
					);
					// �ǰ� �ִϸ��̼� �÷���
					OtherPlayer->PlayTakeDamageAnim();
					OtherPlayer->UpdateHealth(info->HealthValue);
				}

				// �������϶� Ÿ�� �ִϸ��̼� �÷���
				if (info->IsAttacking)
				{
					UE_LOG(LogClass, Log, TEXT("other player hit."));
					OtherPlayer->PlayAttackAnim();
				}

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
				UE_LOG(LogClass, Log, TEXT("other player dead."));
				FTransform transform(Character->GetActorLocation());
				UGameplayStatics::SpawnEmitterAtLocation(
					world, DestroyEmiiter, transform, true
				);
				Character->Destroy();
			}
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
		// ĳ���� �Ӽ� ������Ʈ
		if (tempPlayer->GetHealthValue() != info.HealthValue)
		{
			UE_LOG(LogClass, Log, TEXT("Player damaged"));
			// �ǰ� ��ƼŬ ����
			FTransform transform(tempPlayer->GetActorLocation());
			UGameplayStatics::SpawnEmitterAtLocation(
				world, HitEmiiter, transform, true
			);
			// �ǰ� �ִϸ��̼� ����
			tempPlayer->PlayTakeDamageAnim();
			tempPlayer->UpdateHealth(info.HealthValue);
		}
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

	// ���� �� OtherNetworkCharacter �ҷ���
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

void ANetPlayerController::UpdateMonsterSet()
{
	// �����Ͱ� �ƴ� ��
	if (ci->players[SessionId].IsMaster == true)
		return;
	if (MonsterSetInfo == nullptr)
		return;

	UE_LOG(LogClass, Log, TEXT("UpdateMonsterSet"));
	UWorld* const world = GetWorld();
	if (world)
	{
		TArray<AActor*> SpawnedMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AATank::StaticClass(), SpawnedMonsters);

		for (auto actor : SpawnedMonsters)
		{
			AATank* monster = Cast<AATank>(actor);
			if (monster)
			{
				const Monster* monsterInfo = &MonsterSetInfo->monsters[monster->Id];

				FVector Location;
				Location.X = monsterInfo->X;
				Location.Y = monsterInfo->Y;
				Location.Z = monsterInfo->Z;

				monster->MoveToLocation(Location);

				if (monsterInfo->IsAttacking)
				{
					monster->PlayAttackAnim();
				}
			}
		}
	}
}
void ANetPlayerController::DestroyMonster()
{
	UWorld* const world = GetWorld();
	if (world)
	{
		// ������ ���Ϳ��� ã�� �ı�
		TArray<AActor*> SpawnedMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AATank::StaticClass(), SpawnedMonsters);

		for (auto Actor : SpawnedMonsters)
		{
			AATank* Monster = Cast<AATank>(Actor);
			if (Monster && Monster->Id == MonsterInfo->Id)
			{
				//[TODO] dead
				//Monster->Dead();
				break;
			}
		}

		// ������Ʈ �� �ʱ�ȭ
		MonsterInfo = nullptr;
		bIsNeedToDestroyMonster = false;
	}
}

bool ANetPlayerController::SendMonsterSet()
{
	// �����Ͱ� �� ��
	UWorld* const world = GetWorld();
	if (world == nullptr)
		return false;
	if (ci == nullptr)
		return false;

	if (ci->players[SessionId].IsMaster == false)
		return false;

	MonsterSet sendMonsterSet;

	TArray<AActor*> SpawnedMonsters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AATank::StaticClass(), SpawnedMonsters);

	for (auto actor : SpawnedMonsters)
	{
		AATank* monster = Cast<AATank>(actor);
		if (isTankActionStart == false)
		{
			monster->StartAction();
		}
		if (monster)
		{
			const auto& Location = monster->GetActorLocation();
			const auto& Rotation = monster->GetActorRotation();
			const auto& Velocity = monster->GetVelocity();


			sendMonsterSet.monsters[monster->Id].X = Location.X;
			sendMonsterSet.monsters[monster->Id].Y = Location.Y;
			sendMonsterSet.monsters[monster->Id].Z = Location.Z;
		}
	}
	isTankActionStart = true;
	Socket->SendSyncMonster(sendMonsterSet);
	return true;

}