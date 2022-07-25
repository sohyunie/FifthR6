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
	// ������ ����
	Socket = ClientSocket::GetSingleton();
	SessionId = Socket->ID;
	// CharacterID 1~3��
	//Socket->CharacterID;

	Socket->SetPlayerController(this);

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

	bIsConnected = true;
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
	player->UELevel = 1;
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

	tempCharacter.UELevel = player->UELevel; // [TODO] UE Level ID �ʿ�.

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
		//UE_LOG(LogClass, Log, TEXT("[%d] RecvWorldInfo"), ci->players.size());
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
		UE_LOG(LogClass, Log, TEXT("RecvNewPlayer NewPlayer : %d"), PlayerInfos->players.size());
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

	tempCharacter.UELevel = tempPlayer->UELevel; // [TODO] UE Level ID �ʿ�.

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
	if (!ci->IsLock) {
		//UE_LOG(LogClass, Log, TEXT("Is Lock updateworldinfo"));
		UpdatePlayerInfo(ci->players[SessionId]);
	}

	// �ٸ� �÷��̾� ������Ʈ
	TArray<AActor*> SpawnedCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANetCharacter::StaticClass(), SpawnedCharacters);
	if (nPlayers == -1)
	{
		for (auto& player : ci->players)
		{
			if (player.first == SessionId || !player.second.IsAlive || !player.second.UELevel == 0)
				continue;
			UE_LOG(LogClass, Log, TEXT("---[%d]---. %d"), player.second.SessionId, player.second.UELevel);
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

			if (ci->IsLock)
			{
				UE_LOG(LogClass, Log, TEXT("Is Lock"));
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

	UE_LOG(LogClass, Log, TEXT("NewPlayer : %d"), PlayerInfos->players.size());
	int i = 0;
	for (const auto& kvp : PlayerInfos->players)
	{
		i++;
		UE_LOG(LogClass, Log, TEXT("NewPlayer : %d"), i);
		
		if (kvp.first == SessionId)
			continue;

		const cCharacter* player = &kvp.second;
		//if (player->UELevel == 0) {
		//	UE_LOG(LogClass, Log, TEXT("new player uelevel = 0."));
		//	continue;
		//}
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

				UE_LOG(LogClass, Log, TEXT("Create NewPlayer : %d"), PlayerInfos->players.size());
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
				//UE_LOG(LogClass, Log, TEXT("monster ID : [%d]."), monster->Id);
				const Monster* monsterInfo = &MonsterSetInfo->monsters[monster->Id];
				if (monsterInfo->UELevel == 0) {
					//MonsterSetInfo->monsters.erase(monster->Id);
					continue;
				}
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
					//UE_LOG(LogClass, Log, TEXT("monster ID : [%d]."), monster->ID);
					if (MonsterSetInfo->monsters.size() == 0) {
						UE_LOG(LogClass, Log, TEXT("size 0"));
						continue;
					}
					
					if (monster->ID < 0 || monster->ID > 200) {
						UE_LOG(LogClass, Log, TEXT("error id number : %d"), monster->ID);
						continue;
					}

					const Monster* monsterInfo = &MonsterSetInfo->monsters[monster->ID];
					if (monsterInfo->UELevel == 0) {
						//UE_LOG(LogClass, Log, TEXT("monster : ---[%d]--- size : (%d)"), monster->ID, MonsterSetInfo->monsters.size());
						//MonsterSetInfo->monsters.erase(monster->ID);
						continue;
					}

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
			if (MonsterInfo->UELevel == 0)
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
			if (MonsterInfo->UELevel == 0)
				continue;
			 AATank* Monster = Cast<AATank>(Actor);
			 if (Monster && Monster->ID == MonsterInfo->Id)
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
	if (!ci->IsLock && ci->players[SessionId].IsMaster)
	{
		MonsterSet sendMonsterSet;
		TArray<AActor*> BossMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABossTank::StaticClass(), BossMonsters);
		for (auto actor : BossMonsters)
		{
			ABossTank* monster = Cast<ABossTank>(actor);
			//if (ci->players[SessionId].UELevel != monster->UELevel)
			//	continue;
			if (monster->isMasterStartAction == false)
			{
				UE_LOG(LogTemp, Warning, TEXT("[Master] monster->StartAction()"));
				monster->StartAction();
				monster->isMasterStartAction = true;
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

				sendMonsterSet.monsters[monster->Id].UELevel = monster->UELevel;

				//sendMonsterSet.monsters[monster->Id].Health = monster->GetTankHpRatio();
				//sendMonsterSet.monsters[monster->Id].UELevel = ci->players[SessionId].UELevel;
				sendMonsterSet.monsters[monster->Id].IsAttacking = monster->GetIsAttacking();
			}
		}


		TArray<AActor*> SpawnedMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AATank::StaticClass(), SpawnedMonsters);
		for (auto actor : SpawnedMonsters)
		{
			AATank* monster = Cast<AATank>(actor);
			//UE_LOG(LogTemp, Warning, TEXT("monster->UELevel : %d"), ci->players[SessionId].UELevel)
			//UE_LOG(LogTemp, Warning, TEXT("ci->players[SessionId].UELevel : %d"), ci->players[SessionId].UELevel);

			//if (monster->UELevel != ci->players[SessionId].UELevel)
			//	continue;
			
			if (monster->isMasterStartAction == false)
			{
				UE_LOG(LogTemp, Warning, TEXT("[Master] monster->StartAction()"));
				monster->StartAction();
				monster->isMasterStartAction = true;
			}

			if (monster)
			{
				const auto& Location = monster->GetActorLocation();
				const auto& Rotation = monster->GetActorRotation();
				const auto& Velocity = monster->GetVelocity();

				sendMonsterSet.monsters[monster->ID].Id = monster->ID;

				sendMonsterSet.monsters[monster->ID].X = Location.X;
				sendMonsterSet.monsters[monster->ID].Y = Location.Y;
				sendMonsterSet.monsters[monster->ID].Z = Location.Z;


				sendMonsterSet.monsters[monster->ID].VX = Velocity.X;
				sendMonsterSet.monsters[monster->ID].VY = Velocity.Y;
				sendMonsterSet.monsters[monster->ID].VZ = Velocity.Z;

				sendMonsterSet.monsters[monster->ID].Yaw = Rotation.Yaw;
				sendMonsterSet.monsters[monster->ID].Pitch = Rotation.Pitch;
				sendMonsterSet.monsters[monster->ID].Roll = Rotation.Roll;

				sendMonsterSet.monsters[monster->ID].UELevel = monster->UELevel;

				sendMonsterSet.monsters[monster->ID].Health = monster->GetTankHpRatio();
				//sendMonsterSet.monsters[monster->ID].UELevel = 1;
				sendMonsterSet.monsters[monster->ID].IsAttacking = monster->GetIsAttacking();
			}									 
		}
		Socket->SendSyncMonster(sendMonsterSet);
		return true;

	}
	else
	{
		// ���� ������Ʈ
		TArray<AActor*> BossMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABossTank::StaticClass(), BossMonsters);
		for (auto actor : BossMonsters)
		{
			ABossTank* monster = Cast<ABossTank>(actor);
			if (monster->isStartAction == false) {
				monster->StartAction();
				monster->isStartAction = true;
			}
		}

		TArray<AActor*> SpawnedMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AATank::StaticClass(), SpawnedMonsters);
		for (auto actor : SpawnedMonsters)
		{
			AATank* monster = Cast<AATank>(actor);
			if (monster->isStartAction == false) {
				monster->StartAction();
				monster->isStartAction = true;
			}
		}

		UpdateMonsterSet();
		return true;
	}
}

void ANetPlayerController::RecvActionSkill(int sessionID, int id)
{
	UE_LOG(LogTemp, Warning, TEXT("RecvActionSkill"));
	TArray<AActor*> SpawnedCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANetCharacter::StaticClass(), SpawnedCharacters);
	for (auto& character : SpawnedCharacters)
	{
		ANetCharacter* player = Cast<ANetCharacter>(character);

		if (!player || player->GetSessionId() == -1 || player->GetSessionId() == SessionId)
		{
			continue;
		}
		if (player == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("RecvActionSkill player null"));
			return;
		}
		if (id == 1)
		{
			player->RAttack();
		}
		else if (id == 2)
		{
			player->Fire();
		}
	}
}

void ANetPlayerController::SendActionSkill(int sessionID, int id)
{
	Socket->SendActionSkill(sessionID, id);
}