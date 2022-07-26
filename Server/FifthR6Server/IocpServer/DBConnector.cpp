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
		printf_s("[DB] DB ���� ����\n");
		return false;
	}

	return true;
}

void DBConnector::Close()
{
	mysql_close(Conn);
}

int DBConnector::SearchAccount(const string & Id, const string & Password)
{
	bool bResult = false;
	string sql = "SELECT * FROM user_data.userinfo WHERE name = '";
	sql += Id + "' and password = '" + Password + "'";

	if (mysql_query(Conn, sql.c_str()))
	{
		printf_s("[DB] �˻� ����\n");
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
		printf_s("[ERROR] �ش� ���̵� ����\n");
		return 0;
	}
	mysql_free_result(Res);

	return 0;
}
