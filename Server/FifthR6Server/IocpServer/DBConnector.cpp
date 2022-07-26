#include "stdafx.h"
#include "DBConnector.h"


DBConnector::DBConnector()
{
}


DBConnector::~DBConnector()
{
}

bool DBConnector::Connect(const string & Server, const string & User, const string & Password, const string & Database, const int & Port)
{
	Conn = mysql_init(NULL);
	if (!mysql_real_connect(
		Conn, Server.c_str(), User.c_str(), Password.c_str(), Database.c_str(), Port, NULL, 0)
	)
	{
		printf_s("[DB] DB 접속 실패\n");
		return false;
	}

	return true;
}

void DBConnector::Close()
{
	mysql_close(Conn);
}
#define DB_ADDRESS		"localhost"
#define	DB_PORT			3306
#define DB_ID			"test"
#define DB_PW			"Kk471601"
#define DB_SCHEMA		"user_data"

int DBConnector::SearchAccount(const string & Id, const string & Password)
{
	// Connect 고민...
	if (Connect(DB_ADDRESS, DB_ID, DB_PW, DB_SCHEMA, DB_PORT))
	{
		printf_s("[INFO] DB ���� ����\n");
	}
	else {
		printf_s("[ERROR] DB ���� ����\n");

	}
	MYSQL_RES* Res;	// 결과값
	MYSQL_ROW Row;		// 결과 row
	bool bResult = false;
	string sql = "SELECT * FROM user_data.userinfo WHERE name = '";
	sql += Id + "' and password = '" + Password + "'";

	if (mysql_query(Conn, sql.c_str()))
	{
		printf_s("[DB] 검색 실패\n");
		return false;
	}

	Res = mysql_use_result(Conn);
	Row = mysql_fetch_row(Res);
	if (Row != NULL)
	{
		bResult = true;
		int id = atoi(Row[0]);
		return id;
	}
	else
	{
		printf_s("[ERROR] 해당 아이디 없음\n");
		return 0;
	}
	mysql_free_result(Res);

	return 0;
}
