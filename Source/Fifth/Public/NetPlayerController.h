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

	UPROPERTY(EditAnywhere)
		class AResultTrigger* CurrentResult;

	void ChangeInputMode(bool bGameMode = true);

	// Reference UMG Asset in the Editor
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> ResultWidgetClass;

	// declare widget
	class UUserWidget* ResultInfoWidget;

	// 채팅 함수
	UFUNCTION(BlueprintCallable, Category = "Chat")
		void ChatToServer(FString Text);

	// 세션 아이디 반화
	UFUNCTION(BlueprintPure, Category = "Properties")
		int GetSessionId();

	// HUD 화면에서 쓸 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Properties", Meta = (BlueprintProtect = "true"))
		TSubclassOf<class UUserWidget> HUDWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Properties", Meta = (BlueprintProtect = "true"))
		TSubclassOf<class UUserWidget> GameOverWidgetClass;

	// HUD 객체
	UPROPERTY()
		class UUserWidget* CurrentWidget;

	UPROPERTY()
		class UUserWidget* GameOverWidget;

	// 스폰시킬 다른 캐릭터
	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class ACharacter> WhoToSpawn;

	// 파괴될 때 파티클
	UPROPERTY(EditAnywhere, Category = "Spawning")
		UParticleSystem* DestroyEmiiter;

	// 타격할 때 파티클
	UPROPERTY(EditAnywhere, Category = "Spawning")
		UParticleSystem* HitEmiiter;

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* aPawn) override;

	// 세션아이디에 매칭되는 액터 반환
	AActor* FindActorBySessionId(TArray<AActor*> ActorArray, const int& SessionId);

	// 소켓에게 다른 캐릭터 타격 정보 전달
	UFUNCTION(BlueprintCallable, Category = "Interaction")
		void HitCharacter(const int& SessionId, const ANetCharacter* DamagedCharacter);

	// 소켓으로부터 월드 정보 수신
	void RecvWorldInfo(cCharactersInfo* ci);

	// 소켓으로부터 채팅 정보 수신
	void RecvChat(const string* chat);

	// 새 플레이어 업데이트
	void RecvNewPlayer(cCharactersInfo* NewPlayer);

	// 몬스터 업데이트
	void RecvMonsterSet(MonsterSet* MonstersInfo);
	// 기존 몬스터 파괴
	void RecvDestroyMonster(Monster* MonsterInfo);
	// 몬스터 타격 정보 전달
	bool HitMonster(const int& MonsterId);
	void RecvActionSkill(int sessionID, int id);
	void SendActionSkill(int sessionID, int id);
	void RecvDestructKey(int keyID);
	void SendDestructKey(int sessionID, int keyID);
	bool GetIsMaster();

private:
	FInputModeGameOnly GameInputMode;
	FInputModeUIOnly UIInputMode;

	ClientSocket* Socket;			// 서버와 접속할 소켓
	bool			bIsConnected;	// 서버와 접속 유무
	int				SessionId;		// 캐릭터의 세션 고유 아이디 (랜덤값)
	cCharactersInfo* ci;			// 다른 캐릭터의 정보

	bool SendPlayerInfo();			// 플레이어 위치 송신
	bool UpdateWorldInfo();		// 월드 동기화
	void UpdatePlayerInfo(const cCharacter& info);		// 플레이어 동기화	

	// 채팅 업데이트
	bool bIsChatNeedUpdate;
	const string* chat;
	//void UpdateChat();

	// 새 플레이어 입장
	bool bNewPlayerEntered;
	cCharactersInfo* PlayerInfos;
	void UpdateNewPlayer();


	// 몬스터 업데이트
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

	int skillSessionID;
	int skillID;

	int destructKeyID;
	int keyCount;

	bool isClearStage = false;
};
