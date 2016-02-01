#pragma once

#include "stdafx.h"
#include "TLog.h"

namespace SQLite
{
	struct exception_base {};
	struct exception: exception_base
	{
		std::string msg;
		exception(std::string&& msg) : msg(msg) {}
		exception(const std::string &msg) : msg(msg) {}
	};
	struct wexception: exception_base
	{
		std::wstring msg;
		wexception(std::wstring&& msg) : msg(msg) {}
		wexception(const std::wstring& msg) : msg(msg){}
	};
	struct TDB
	{
		sqlite3 *db = nullptr;
		TDB::~TDB()
		{
			Close();
		}
		void TDB::Close()
		{
			if (db) {
				sqlite3_close(db);
				db = nullptr;
			}
		}
		inline void TDB::Raise(const std::string& msg)
		{
			std::string erm(msg);
			erm += " failed. ";
			erm += sqlite3_errmsg(db);
			throw exception(erm);
		}
		inline void TDB::Raise(std::string&& msg)
		{
			std::string erm(msg);
			erm += " failed. ";
			erm += sqlite3_errmsg(db);
			throw exception(erm);
		}
		inline void TDB::Raise(const char *msg)
		{
			std::string erm(msg);
			erm += " failed. ";
			erm += sqlite3_errmsg(db);
			throw exception(erm);
		}
		inline void TDB::Raise(const char* msg, const char* msg2)
		{
			std::string erm(msg);
			erm += " failed. ";
			erm += msg2;
			erm += " ";
			erm += sqlite3_errmsg(db);
			throw exception(erm);
		}
		void TDB::Open(const wchar_t* DatabaseFileName)
		{
			Close();
			if (SQLITE_OK != sqlite3_open16(DatabaseFileName, &db))
			{
				throw wexception(
					std::wstring(L"sqlite3_open(") // todo: make utf16 version
					+ (DatabaseFileName ? DatabaseFileName : L"<NULL>")
					+ L") failed: "
					+ ((wchar_t*)sqlite3_errmsg16(db))
					);
			}

		}
		void TDB::Open(const char* DatabaseFileName)
		{
			Close();
			if (SQLITE_OK != sqlite3_open(DatabaseFileName, &db))
			{
				throw exception(std::string("sqlite3_open(")
					+ (DatabaseFileName ? DatabaseFileName : "<NULL>")
					+ ") failed: " + sqlite3_errmsg(db) );
			}
		}
		struct TTransaction
		{
			TDB &db;
			bool isActive = true;
			TTransaction::TTransaction(SQLite::TDB& db) : db(db) {}
			TTransaction::~TTransaction()
			{
				if (!isActive)
					return;
				char* erm = nullptr;
				if (SQLITE_OK != sqlite3_exec(db.db, "rollback", nullptr, nullptr, &erm) )
				{
					Log("SQLite::TDB::TTransaction::~TTransaction(), sqlite3_exec(db.db, rollback, nullptr, nullptr, &erm) failed.\n");
					if (erm) {
						Log(erm);
						sqlite3_free(erm);
					}
				}				
			}
			void TTransaction::Commit()
			{
				if (!isActive)
					db.Raise("TTransaction::Commit() failed. isActive==false");

				char *erm = 0;
				int res = sqlite3_exec(db.db, "commit", 0, 0, &erm);
				if (res != SQLITE_OK) {
					std::string s("sqlite3_exec(db.db, \"commit\") failed: ");
					if (erm) {
						s += erm;
						sqlite3_free(erm);
					}
					db.Raise(s);
				}
				isActive = false;
			}
		};

		TTransaction TDB::BeginTransaction()
		{
			Exec("begin");
			return TTransaction(*this);
		}
		void TDB::Exec(const char* sql)
		{
			char *erm = 0;
			if (SQLITE_OK != sqlite3_exec(db, sql, 0, 0, &erm))
			{
				std::string e("void TDB::Exec(const char* sql) failed. ");
				if (erm) {
					e += erm;
					sqlite3_free(erm);
				}
				Raise(e);
			}
		}
		struct TStmt
		{
			TDB &db;
			sqlite3_stmt* stmt = nullptr;
			TStmt::TStmt(TDB& db) :db(db), stmt(nullptr) {} // todo: check constructors for all class's, make non_copybal or add copy-constructor
			TStmt(TStmt&& st) :db(st.db), stmt(st.stmt) { st.stmt = 0; }
			TStmt::~TStmt()
			{
				Finalize();
			}
			void TStmt::Bind(int ParamNo, const char* val)
			{
				int ret = sqlite3_bind_text(stmt, ParamNo, val, -1, SQLITE_STATIC);
				if (SQLITE_OK != ret)
					db.Raise("TStmt::Bind(int ParamNo, const char* val)");
			}
			void TStmt::Bind(int ParamNo, i32 val)
			{
				int ret = sqlite3_bind_int(stmt, ParamNo, val);
				if (SQLITE_OK != ret)
					db.Raise("TStmt::Bind(int ParamNo, i32 val)");
			}
			void TStmt::Bind(int ParamNo, ui32 val)
			{
				int ret = sqlite3_bind_int64(stmt, ParamNo, val);
				if (SQLITE_OK != ret)
					db.Raise("TStmt::Bind(int ParamNo, ui32 val)");
			}
			int TStmt::Step()
			{
				int ret = sqlite3_step(stmt);
				return ret;
			}
			void TStmt::Reset()
			{
				int er = sqlite3_reset(stmt);
				if (er != SQLITE_OK)
					db.Raise("TStmt::Reset(), sqlite3_reset failed.");
			}
			void TStmt::Finalize()
			{
				if (stmt) {
					sqlite3_finalize(stmt);
					stmt = nullptr;
				}
			}
			void TStmt::Get(int col, int& val, bool& IsNull)
			{
				int type = sqlite3_column_type(stmt, col);
				if (type == SQLITE_NULL){
					IsNull = true;
					return;
				}
				IsNull = false;
				if (SQLITE_INTEGER != type)
					db.Raise("TStmt::Get int falied. Not intger value in column");

				val = sqlite3_column_int(stmt, col);
			}
			void TStmt::Get(int col, ui16& val, bool& IsNull)
			{
				int type = sqlite3_column_type(stmt, col);
				if (type == SQLITE_NULL) {
					IsNull = true;
					return;
				}
				IsNull = false;
				if (SQLITE_INTEGER != type)
					db.Raise("TStmt::Get ui16 falied. Not intger value in column");

				int v = sqlite3_column_int(stmt, col);
				if (v > Max<ui16>())
					db.Raise("TStmt::Get ui16 falied. Out of range.");

				val = ui16(v);
			}
			void TStmt::Get(int col, std::string&, bool& IsNull);
		};
		TStmt TDB::Prepare(const char *sql)
		{
			sqlite3_stmt *stmt = 0;
			const char *tail = nullptr;
			if (SQLITE_OK != sqlite3_prepare_v2(db, sql, -1, &stmt, &tail))
				Raise("TDB::Prepare");
			TStmt Stmt(*this);
			Stmt.stmt = stmt;
			return Stmt;
		}
	};

}//namespace sqlite3