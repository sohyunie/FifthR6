#pragma once
#include <string>
#include <mysql.h>
#pragma comment (lib, "libmysql.lib")

using namespace std;

class DBConnector
{
public:
	DBConnector();
	~DBConnector();

	// DB 에 연결
	bool Connect(
		const string&	Server,
		const string&	User,
		const string&	Password,
		const string&	Database,
		const int&		Port
	);
	// DB 연결 종료
	void Close();
	
	// 유저 계정을 찾음
	int SearchAccount(const string& Id, const string& Password);

private:
	MYSQL * Conn;		// 커넥터
};

