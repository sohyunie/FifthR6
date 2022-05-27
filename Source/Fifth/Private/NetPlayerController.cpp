// Fill out your copyright notice in the Description page of Project Settings.

#include "NetPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MyHUDWidget.h"
#include "WarriorOfFire.h"
#include "WarriorOfThunder.h"
#include "WarriorOfWater.h"
#include "ATank.h"
#include "BossTank.h"
//#include "Blueprint/UserWidget.h"
#include <string>

ANetPlayerController::ANetPlayerController()
{
	SessionId = FMath::RandRange(0, 10000);

	// ������ ����
	Socket = ClientSocket::GetSingleton();
	Socket->InitSocket();
	bIsConnected = Socket->Connect("14.51.90.214", 8080);
	if (bIsConnected)
	{
		UE_LOG(LogClass, Log, TEXT("IOCP Server connect success!"));
		Socket->SetPlayerController(this);
	}

	bIsChatNeedUpdate = false;
	bNewPlayerEntered = false;
	nPlayers = -1;

	WhoToSpawn = AWarriorOfFire::StaticClass();

	// �ӽ� ��ƼŬ
	//DestroyEmiiter = Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
	//	TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Sparks.P_Sparks'")));
	//HitEmiiter = Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
	//	TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Sparks.P_Sparks'")));

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

bool ANetPlayerController::GetIsMaster()
{
	if (ci == nullptr)
		return true;

	UE_LOG(LogClass, Log, TEXT("%s"), ci->players[SessionId].IsMaster ? "1" : "0");
	return ci->players[SessionId].IsMaster;
}

void ANetPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);



	if (!bIsConnected) return;

	// �÷��̾� ���� �۽�
	if (!SendPlayerInfo()) return;

	// ���� ����ȭ
	if (!UpdateWorldInfo()) return;

	// ���� �� �۽�, ����
	if (!UpdateMonster()) return;

	// ���� �ı�
	if (bIsNeedToDestroyMonster)
		DestroyMonster();
	// ä�� ����ȭ
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

bool ANetPlayerController::HitMonster(const int& MonsterId)
{
	UE_LOG(LogClass, Log, TEXT("Monster Hit Called %d"), MonsterId);
	if (ci!= nullptr && ci->players[SessionId].IsMaster)
	{
		if (ci->players[SessionId].IsMaster)
		{
			Socket->HitMonster(MonsterId);
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

void ANetPlayerController::RecvWorldInfo(cCharactersInfo* ci_)
{
	if (ci_ != nullptr)
	{
		ci = ci_;
		for (auto& player : ci->players)
		{
			//UE_LOG(LogClass, Log, TEXT("[%d] damaged. %f//%f"), player.first , player.second.HealthValue, player.second.X);

			if (player.second.IsAttacking)
			{
				attackSessionID = player.second.SessionId;
			}
		}
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
		PlayerInfos = NewPlayer_;
	}
}

void ANetPlayerController::RecvMonsterSet(MonsterSet* MonstersInfo_)
{
	if (MonstersInfo_ != nullptr)
	{
		MonsterSetInfo = MonstersInfo_;
		//for (auto& monster : MonsterSetInfo->monsters)
		//{
		//	if (monster.second.IsAttacking)
		//	{
		//		attackMonsterID = monster.second.Id;
		//	}
		//}
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

	tempCharacter.UELevel = 1; // [TODO] UE Level ID �ʿ�.

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
	if (nPlayers == -1)
	{
		for (auto& player : ci->players)
		{
			if (player.first == SessionId || !player.second.IsAlive)
				continue;

			FVector spawnLocation;
			spawnLocation.X = player.second.X;
			spawnLocation.Y = player.second.Y;
			spawnLocation.Z = player.second.Z;

			FRotator spawnRotation;
			spawnRotation.Yaw = player.second.Yaw;
			spawnRotation.Pitch = player.second.Pitch;
			spawnRotation.Roll = player.second.Roll;

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = this->GetPawn();
			SpawnParams.Name = FName(*FString(to_string(player.second.SessionId).c_str()));


			//UE_LOG(LogClass, Log, TEXT("Player damaged : %d"), PlayerInfos->players.size());
			//switch (PlayerInfos->players.size()) {
			//case 0:
			//	WhoToSpawn = AWarriorOfFire::StaticClass();
			//	break;
			//case 1:
			//	WhoToSpawn = AWarriorOfFire::StaticClass();
			//	break;
			//case 2:
			//	WhoToSpawn = AWarriorOfFire::StaticClass();
			//	break;
			//}

			ANetCharacter* SpawnCharacter = world->SpawnActor<ANetCharacter>(WhoToSpawn, spawnLocation, spawnRotation, SpawnParams);
			SpawnCharacter->SpawnDefaultController();
			SpawnCharacter->SetSessionId(player.second.SessionId);
		}

		nPlayers = ci->players.size();
	}
	else {
		for (auto& character : SpawnedCharacters)
		{
			ANetCharacter* OtherPlayer = Cast<ANetCharacter>(character);

			if (!OtherPlayer || OtherPlayer->GetSessionId() == -1 || OtherPlayer->GetSessionId() == SessionId)
			{
				continue;
			}

			cCharacter* info = &ci->players[OtherPlayer->GetSessionId()];
			if (info->UELevel == 0) { // ���� ������ ����
				UE_LOG(LogClass, Log, TEXT("---[%d]---. %f//%d"), info->SessionId, OtherPlayer->GetHealthValue(), info->UELevel);
				continue;
			}

			if (info->IsAlive)
			{
				if (OtherPlayer->GetHealthValue() != info->HealthValue)
				{
					// �ǰ� ��ƼŬ ��ȯ
					FTransform transform(OtherPlayer->GetActorLocation());
					//UGameplayStatics::SpawnEmitterAtLocation(
					//	world, HitEmiiter, transform, true
					//);
					// �ǰ� �ִϸ��̼� �÷���
					OtherPlayer->PlayTakeDamageAnim();
					OtherPlayer->UpdateHealth(info->HealthValue);
				}

				// �������϶� Ÿ�� �ִϸ��̼� �÷���
				//if (info->IsAttacking)
				//{
				//	UE_LOG(LogClass, Log, TEXT("Attack Motion"));
				//	OtherPlayer->PlayAttackAnim();
				//}

				if (attackSessionID == info->SessionId) {
					UE_LOG(LogClass, Log, TEXT("Attack Motion"));
					OtherPlayer->PlayAttackAnim();
					attackSessionID = -1;
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
				UE_LOG(LogClass, Log, TEXT("[%d] Die Info. %f"), info->SessionId, info->HealthValue, info->X);
				UE_LOG(LogClass, Log, TEXT("other player dead."));
				FTransform transform(character->GetActorLocation());
				UGameplayStatics::SpawnEmitterAtLocation(
					world, DestroyEmiiter, transform, true
				);
				character->Destroy();
			}
		}
	}
	
	return true;
}

void ANetPlayerController::UpdatePlayerInfo(const cCharacter& info)
{
	auto tempPlayer = Cast<ANetCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	UWorld* const world = GetWorld();
	if (info.UELevel == 0) { // ���� ������ ����
		UE_LOG(LogClass, Log, TEXT("---[%d]---. %d"), info.SessionId, info.UELevel);
		return;
	}
	if (!info.IsAlive)
	{
		UE_LOG(LogClass, Log, TEXT("[%d] Die Info. %f"), info.SessionId, info.HealthValue, info.X);
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
		//UE_LOG(LogClass, Log, TEXT("Player damaged : %d"), tempPlayer->GetSessionId());
		if (tempPlayer->GetHealthValue() != info.HealthValue)
		{
			//UE_LOG(LogClass, Log, TEXT("Player damaged : %f / %f"), tempPlayer->GetHealthValue(), info.HealthValue);
			// �ǰ� ��ƼŬ ����
			FTransform transform(tempPlayer->GetActorLocation());
			//UGameplayStatics::SpawnEmitterAtLocation(
			//	world, HitEmiiter, transform, true
			//);
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

	for (const auto& kvp : PlayerInfos->players)
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

				UE_LOG(LogClass, Log, TEXT("Player damaged : %d"), PlayerInfos->players.size());

				ANetCharacter* SpawnCharacter = world->SpawnActor<ANetCharacter>(WhoToSpawn, spawnLocation, spawnRotation, SpawnParams);
				SpawnCharacter->SpawnDefaultController();
				SpawnCharacter->SetSessionId(player->SessionId);
			}
		}
	}

	bNewPlayerEntered = false;
	PlayerInfos = nullptr;
}


void ANetPlayerController::OnPossess(APawn* aPawn)
{
	ABLOG_S(Warning);
	Super::OnPossess(aPawn);
}

void ANetPlayerController::UpdateMonsterSet()
{
	if (MonsterSetInfo == nullptr)
		return;

	UWorld* const world = GetWorld();
	if (world)
	{
		TArray<AActor*> BossMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABossTank::StaticClass(), BossMonsters);
		for (auto actor : BossMonsters)
		{
			ABossTank* monster = Cast<ABossTank>(actor);
			if (monster)
			{
				const Monster* monsterInfo = &MonsterSetInfo->monsters[monster->Id];
				if (monsterInfo->ueLevel == 0)
					continue;

				FVector Location;
				Location.X = monsterInfo->X;
				Location.Y = monsterInfo->Y;
				Location.Z = monsterInfo->Z;

				FVector CharacterVelocity;
				CharacterVelocity.X = monsterInfo->VX;
				CharacterVelocity.Y = monsterInfo->VY;
				CharacterVelocity.Z = monsterInfo->VZ;

				FRotator spawnRotation;
				spawnRotation.Yaw = monsterInfo->Yaw;
				spawnRotation.Pitch = monsterInfo->Pitch;
				spawnRotation.Roll = monsterInfo->Roll;

				monster->AddMovementInput(CharacterVelocity);
				monster->SetActorRotation(spawnRotation);
				monster->MoveToLocation(Location);

				if (monsterInfo->IsAttacking)
				{
					monster->PlayAttackAnim();
				}
			}
		}
		TArray<AActor*> SpawnedMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AATank::StaticClass(), SpawnedMonsters);

		for (auto actor : SpawnedMonsters)
		{
			AATank* monster = Cast<AATank>(actor);
			if (monster)
			{
				if (MonsterSetInfo == nullptr)
				{
					continue;
				}
				else
				{
					if (MonsterSetInfo->monsters.size() != 0)
					{
						if (!MonsterSetInfo->monsters.count(monster->Id)) {
							if (monster == NULL)
								continue;
							if (monster->Id == NULL)
								continue;
						}
					}
				}

				const Monster* monsterInfo = &MonsterSetInfo->monsters[monster->Id];
				if (monsterInfo->ueLevel == 0)
					continue;

				FVector Location;
				Location.X = monsterInfo->X;
				Location.Y = monsterInfo->Y;
				Location.Z = monsterInfo->Z;

				FVector CharacterVelocity;
				CharacterVelocity.X = monsterInfo->VX;
				CharacterVelocity.Y = monsterInfo->VY;
				CharacterVelocity.Z = monsterInfo->VZ;

				FRotator spawnRotation;
				spawnRotation.Yaw = monsterInfo->Yaw;
				spawnRotation.Pitch = monsterInfo->Pitch;
				spawnRotation.Roll = monsterInfo->Roll;

				monster->SetTankHpRatio(monsterInfo->Health);
				monster->AddMovementInput(CharacterVelocity);
				monster->SetActorRotation(spawnRotation);
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

		TArray<AActor*> BossMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABossTank::StaticClass(), BossMonsters);
		for (auto actor : BossMonsters)
		{
			if (MonsterInfo->ueLevel == 0)
				continue;
			ABossTank* Monster = Cast<ABossTank>(actor);
			if (Monster && Monster->Id == MonsterInfo->Id)
			{
				UE_LOG(LogClass, Log, TEXT("[%d] Health %f"), MonsterInfo->Id, MonsterInfo->Health);
				//Monster->GetTankHpRatio() = MonsterInfo->Health;
				//Monster->SetTankHpRatio(MonsterInfo->Health);
				Monster->PlayTakeDamageAnim();
				/*if (Monster->GetTankHpRatio() <= 0) {
					//Monster->Destroy();
					  //[TODO] dead
					  //Monster->Dead();
				}*/
				break;
			}
		}
		TArray<AActor*> SpawnedMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AATank::StaticClass(), SpawnedMonsters);
		for (auto Actor : SpawnedMonsters)
		{
			if (MonsterInfo->ueLevel == 0)
				continue;
			 AATank* Monster = Cast<AATank>(Actor);
			 if (Monster && Monster->Id == MonsterInfo->Id)
			 {
				  UE_LOG(LogClass, Log, TEXT("[%d] Health %f"), MonsterInfo->Id, MonsterInfo->Health);
				  //Monster->SetTankHpRatio(MonsterInfo->Health);
				  //Monster->GetTankHpRatio() = MonsterInfo->Health;
				  //Monster->SetTankHpRatio(MonsterInfo->Health);
				  Monster->PlayTakeDamageAnim();
				  if (Monster->GetTankHpRatio() <= 0) {
					  //Monster->Destroy();
			 			//[TODO] dead
			 			//Monster->Dead();
				  }
			 	break;
			 }
		}

		// ������Ʈ �� �ʱ�ȭ
		MonsterInfo = nullptr;
		bIsNeedToDestroyMonster = false;
	}
}

bool ANetPlayerController::UpdateMonster()
{
	// �����Ͱ� �� ��
	UWorld* const world = GetWorld();
	if (world == nullptr)
		return false;
	if (ci == nullptr)
		return false;

	//UE_LOG(LogClass, Log, TEXT("[isMaster] %d"), ci->players[SessionId].IsMaster ? 1 : 0);
	if (ci->players[SessionId].IsMaster)
	{
		MonsterSet sendMonsterSet;

		TArray<AActor*> BossMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABossTank::StaticClass(), BossMonsters);
		for (auto actor : BossMonsters)
		{
			ABossTank* monster = Cast<ABossTank>(actor);
			if (isTankActionStart == false)
			{
				UE_LOG(LogTemp, Warning, TEXT("monster->StartAction()"));
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
				sendMonsterSet.monsters[monster->Id].Id = monster->Id;


				sendMonsterSet.monsters[monster->Id].VX = Velocity.X;
				sendMonsterSet.monsters[monster->Id].VY = Velocity.Y;
				sendMonsterSet.monsters[monster->Id].VZ = Velocity.Z;


				sendMonsterSet.monsters[monster->Id].Yaw = Rotation.Yaw;
				sendMonsterSet.monsters[monster->Id].Pitch = Rotation.Pitch;
				sendMonsterSet.monsters[monster->Id].Roll = Rotation.Roll;

				//sendMonsterSet.monsters[monster->Id].Health = monster->GetTankHpRatio();
				sendMonsterSet.monsters[monster->Id].ueLevel = ci->players[SessionId].UELevel;
				sendMonsterSet.monsters[monster->Id].IsAttacking = monster->GetIsAttacking();
			}
		}


		TArray<AActor*> SpawnedMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AATank::StaticClass(), SpawnedMonsters);
		for (auto actor : SpawnedMonsters)
		{
			AATank* monster = Cast<AATank>(actor);
			if (isTankActionStart == false)
			{
				UE_LOG(LogTemp, Warning, TEXT("monster->StartAction()"));
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
				sendMonsterSet.monsters[monster->Id].Id = monster->Id;


				sendMonsterSet.monsters[monster->Id].VX = Velocity.X;
				sendMonsterSet.monsters[monster->Id].VY = Velocity.Y;
				sendMonsterSet.monsters[monster->Id].VZ = Velocity.Z;


				sendMonsterSet.monsters[monster->Id].Yaw = Rotation.Yaw;
				sendMonsterSet.monsters[monster->Id].Pitch = Rotation.Pitch;
				sendMonsterSet.monsters[monster->Id].Roll = Rotation.Roll;

				sendMonsterSet.monsters[monster->Id].Health = monster->GetTankHpRatio();
				sendMonsterSet.monsters[monster->Id].ueLevel = 1;
				sendMonsterSet.monsters[monster->Id].IsAttacking = monster->GetIsAttacking();
			}
		}
		isTankActionStart = true;
		Socket->SendSyncMonster(sendMonsterSet);
		return true;

	}
	else {
		// ���� ������Ʈ
		if (isTankActionStart == false)
		{
			WhoToSpawn = AWarriorOfWater::StaticClass();

			TArray<AActor*> BossMonsters;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABossTank::StaticClass(), BossMonsters);
			for (auto actor : BossMonsters)
			{
				ABossTank* monster = Cast<ABossTank>(actor);
				monster->StartAction();
			}

			TArray<AActor*> SpawnedMonsters;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AATank::StaticClass(), SpawnedMonsters);
			for (auto actor : SpawnedMonsters)
			{
				AATank* monster = Cast<AATank>(actor);
				monster->StartAction();
			}
			isTankActionStart = true;
		}

		UpdateMonsterSet();
		return true;
	}
}

void ANetPlayerController::RecvSyncCube(bool isOn)
{


}

void ANetPlayerController::SendSyncCube(bool isOn)
{
	Socket->SendSyncCube(isOn);
}