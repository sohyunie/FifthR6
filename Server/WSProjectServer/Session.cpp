#include "common.h"
#include "Session.h"
#include "../WSProjectNetwork/WSAOverlappedEx.h"


Session::Session()
	: ID(-1), Info{},
	  mRecvOverlapped{}, mState(State::FREE),
	  Type(ClientType::PLAYER), 
	  Active(false), LastMoveTime(0)
{
}

Session::~Session()
{

}

void Session::Disconnect()
{
	mStateLock.lock();
	mState = State::FREE;
	mStateLock.unlock();
	Socket::Close();
}

void Session::AssignAcceptedID(int id, SOCKET sck)
{
	ID = id;
	mSocket = sck;
}

bool Session::CompareAndChangeState(State target_state, State new_state)
{
	return (mState.compare_exchange_strong(target_state, new_state));
}

void Session::InsertViewID(int id)
{
	ViewListLock.lock();
	mViewList.insert(id);
	ViewListLock.unlock();
}

void Session::EraseViewID(int id)
{
	ViewListLock.lock();
	mViewList.erase(id);
	ViewListLock.unlock();
}

bool Session::FindAndInsertViewID(int id)
{
	ViewListLock.lock();
	if (mViewList.find(id) != mViewList.end())
	{
		ViewListLock.unlock();
		return false;
	}
	mViewList.insert(id);
	ViewListLock.unlock();
	return true;
}

bool Session::FindAndEraseViewID(int id)
{
	ViewListLock.lock();
	if (mViewList.find(id) != mViewList.end())
	{
		mViewList.erase(id);
		ViewListLock.unlock();
		return true;
	}	
	ViewListLock.unlock();
	return false;
}

void Session::SendMsg(char* msg, int bytes)
{
	WSAOVERLAPPEDEX* send_over = new WSAOVERLAPPEDEX(OP::SEND, msg, bytes);
	Send(*send_over);
}

void Session::RecvMsg()
{
	mRecvOverlapped.Reset(OP::RECV);
	Recv(mRecvOverlapped);
}

bool Session::IsSame(int x, int y)
{
	return (Info.x == x && Info.y == y);
}

bool Session::IsStateWithoutLock(State state)
{
	return (mState == state);
}

void Session::Revive()
{
	Info.hp = Info.max_hp;
	mState = State::INGAME;
}

int Session::IncreaseEXP(int opLevel)
{
	//Info.exp += opLevel * opLevel * 2;
	IncreaseLevelWithCondition();
	return opLevel * opLevel * 2;
}

void Session::IncreaseLevelWithCondition()
{
	while (true) {
		//int expNeeded = 100 * pow(2, Info.level - 1);
		//if (expNeeded <= Info.exp)
		//{
		//	Info.hp = Info.max_hp;
		//}
		//else 
		break;
	}
}

void Session::SetAttackDuration(std::chrono::milliseconds time)
{
	mAttackedTime = std::chrono::system_clock::now();
	mAttackDuration = time;
}

bool Session::IsAttackTimeOut() const
{
	return (mAttackedTime + mAttackDuration < std::chrono::system_clock::now());
}

bool Session::IsStateWithLock(State state)
{
	mStateLock.lock();
	bool ret = (mState == state);
	mStateLock.unlock();
	return ret;
}