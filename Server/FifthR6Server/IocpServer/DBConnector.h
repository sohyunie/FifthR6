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

	// DB �� ����
	bool Connect(
		const string&	Server,
		const string&	User,
		const string&	Password,
		const string&	Database,
		const int&		Port
	);
	// DB ���� ����
	void Close();
	
	// ���� ������ ã��
	int SearchAccount(const string& Id, const string& Password);

private:
	MYSQL * Conn;		// Ŀ����
	MYSQL_RES * Res;	// �����
	MYSQL_ROW Row;		// ��� row
};

