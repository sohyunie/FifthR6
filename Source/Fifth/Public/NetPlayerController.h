// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "GameFramework/PlayerController.h"
#include "ClientSocket.h"
#include "NetCharacter.h"
#include "NetPlayerController.generated.h"

/**
 *
 */
UCLASS()
class FIFTH_API ANetPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANetPlayerController();
	~ANetPlayerController();

	// ä�� �Լ�
	UFUNCTION(BlueprintCallable, Category = "Chat")
		void ChatToServer(FString Text);

	// ���� ���̵� ��ȭ
	UFUNCTION(BlueprintPure, Category = "Properties")
		int GetSessionId();

	// HUD ȭ�鿡�� �� ���� Ŭ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Properties", Meta = (BlueprintProtect = "true"))
		TSubclassOf<class UUserWidget> HUDWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Properties", Meta = (BlueprintProtect = "true"))
		TSubclassOf<class UUserWidget> GameOverWidgetClass;

	// HUD ��ü
	UPROPERTY()
		class UUserWidget* CurrentWidget;

	UPROPERTY()
		class UUserWidget* GameOverWidget;

	// ������ų �� ĳ����
	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class ACharacter> WhoToSpawnOwn;

	// ������ų �ٸ� ĳ����
	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class ACharacter> WhoToSpawnOther;

	// �ı��� �� ��ƼŬ
	UPROPERTY(EditAnywhere, Category = "Spawning")
		UParticleSystem* DestroyEmiiter;

	// Ÿ���� �� ��ƼŬ
	UPROPERTY(EditAnywhere, Category = "Spawning")
		UParticleSystem* HitEmiiter;

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* aPawn) override;

	// ���Ǿ��̵� ��Ī�Ǵ� ���� ��ȯ
	AActor* FindActorBySessionId(TArray<AActor*> ActorArray, const int& SessionId);

	// ���Ͽ��� �ٸ� ĳ���� Ÿ�� ���� ����
	UFUNCTION(BlueprintCallable, Category = "Interaction")
		void HitCharacter(const int& SessionId, const ANetCharacter* DamagedCharacter);

	// �������κ��� ���� ���� ����
	void RecvWorldInfo(cCharactersInfo* ci);

	// �������κ��� ä�� ���� ����
	void RecvChat(const string* chat);

	// �� �÷��̾� ������Ʈ
	void RecvNewPlayer(cCharactersInfo* NewPlayer);

	// ���� ������Ʈ
	void RecvMonsterSet(MonsterSet* MonstersInfo);
	// ���� ���� �ı�
	void RecvDestroyMonster(Monster* MonsterInfo);
	// ���� Ÿ�� ���� ����
	bool HitMonster(const int& MonsterId);
	void RecvActionSkill(int sessionID, int id);
	void SendActionSkill(int sessionID, int id);
	bool GetIsMaster();

private:
	ClientSocket* Socket;			// ������ ������ ����
	bool			bIsConnected;	// ������ ���� ����
	int				SessionId;		// ĳ������ ���� ���� ���̵� (������)
	cCharactersInfo* ci;			// �ٸ� ĳ������ ����

	bool SendPlayerInfo();			// �÷��̾� ��ġ �۽�
	bool UpdateWorldInfo();		// ���� ����ȭ
	void UpdatePlayerInfo(const cCharacter& info);		// �÷��̾� ����ȭ	

	// ä�� ������Ʈ
	bool bIsChatNeedUpdate;
	const string* chat;
	//void UpdateChat();

	// �� �÷��̾� ����
	bool bNewPlayerEntered;
	cCharactersInfo* PlayerInfos;
	void UpdateNewPlayer();


	// ���� ������Ʈ
	MonsterSet* MonsterSetInfo;
	int nMonsters;
	void UpdateMonsterSet();

	Monster* MonsterInfo;

	bool bIsNeedToDestroyMonster;
	void DestroyMonster();
	bool UpdateMonster();

	int nPlayers;

	int attackSessionID;
	int attackMonsterID;
};
