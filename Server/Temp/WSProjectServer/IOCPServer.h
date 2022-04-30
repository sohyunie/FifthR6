#pragma once

#include "Session.h"
#include "DBHandler.h"
#include "Room.h"

class IOCPServer
{
public:
	IOCPServer(const EndPoint& ep);
	virtual ~IOCPServer();

	void Run();
	
	void Disconnect(int id);
	void AcceptNewClient(int id, SOCKET sck);

	void ProcessPackets(int id, RingBuffer& msgQueue);	
	void ProcessJoinPacket(cs_packet_game_join& pck, int myId);
	void ProcessAttackPacket(int id, const std::unordered_set<int>& viewlist);

	void SendNewPlayerPosition(std::shared_ptr<Room> room, int newPlayerUid);
	void SendNearPlayersInfoToNewPlayer(int sender);

	void HandlePlayersInSight(
		const std::unordered_set<int>& sights,
		const std::unordered_set<int>& viewlist, int myId);
	void HandleDisappearedPlayers(
		const std::unordered_set<int>& sights,
		const std::unordered_set<int>& viewlist, int myId);

	void SendLoginOkPacket(int id);
	void SendJoinPacket(int id);
	void SendLoginFailPacket(int id, char reason);
	void SendPutObjectPacket(int sender, int target);
	void SendMovePacket(int sender, int target);
	void SendRotatePacket(int ID);
	void SendRemovePacket(int sender, int target);
	void SendStatusChangePacket(int sender);
	void SendBattleResultPacket(int sender, int target, int val, char type);

	bool IsNear(int a_id, int b_id);

	void MovePosition(XMFLOAT3& position, char direction);

	void HandleCompletionInfoByOperation(WSAOVERLAPPEDEX* over, int id, int bytes);
	void HandleRevivedPlayer(int id);

	static void AddTimer(int obj_id, int player_id, EventType type, int direction, int duration);
	void ActivatePlayerMoveEvent(int target, int player);

	void InsertIntoSectorWithoutLock(int id);
	void InsertIntoSectorWithLock(int id);
	void EraseFromSectorWidthLock(int id);
	std::pair<short, short> GetSectorIndex(int id);

	int GetAvailableID();

	static void NetworkThreadFunc(IOCPServer& server);
	static void TimerThreadFunc(IOCPServer& server);

	static const int MaxThreads = 1;

private:
	Socket mListenSck;
	IOCP mIOCP;

	//static std::array<std::shared_ptr<Session>, MAX_USER + MAX_NPC> gClients;
	static std::unordered_map<int, std::shared_ptr<Room>> gRooms;
	static std::unordered_map<int, std::shared_ptr<Session>> gClients;
	static std::array<std::array<std::unordered_set<int>, SECTOR_WIDTH>, SECTOR_HEIGHT> gSectors;
	static concurrency::concurrent_priority_queue<TimerEvent> gTimerQueue;

	std::mutex mSectorLock;

	std::vector<std::thread> mThreads;
	std::thread mTimerThread;
	std::atomic_bool mLoop;

	DBHandler mDBHandler;

	int clientCount = 10000; //임시 코드
	int roomCount = 0;
	const XMFLOAT3 defaultPosition[4] = { XMFLOAT3(-10, 0, 10) , XMFLOAT3(10, 0, 10) ,XMFLOAT3(-10, 0, -10) ,XMFLOAT3(10, 0, -10) };
};