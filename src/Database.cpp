#include "Database.hpp"
// Standard Lib
#include <stdio.h>
#include <string>
#include <iostream>
#include <stdexcept>
// Hashing
#include <bcrypt/BCrypt.hpp>
// Sqlite
#include <sqlite3.h>
// Logger
#include "Logger.hpp"

Database *Database::instance = nullptr;

Database::Database()
{
    rc = sqlite3_open("data/library.db", &db);
    if (rc != SQLITE_OK || db == nullptr)
    {
        Logger::dbLogger->error("Failed to open database: {}", sqlite3_errmsg(db));
        throw std::runtime_error("Database connection failed");
    }
    else
    {
        Logger::dbLogger->info("Database opened successfully.");
    }

    rc = createTables();
    if (rc != SQLITE_OK)
    {
        throw std::logic_error("Unable to setup database successfully! (rc variable != SQLITE_OK)");
    }
}

Database::~Database()
{
    sqlite3_close(db);
    sqlite3_free(errMsg);
}

Database *Database::getInstance()
{
    if (!instance)
        instance = new Database();
    return instance;
}

int Database::createTables()
{
    const char *sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE,
            email TEXT UNIQUE,
            password TEXT NOT NULL,
            role TEXT NOT NULL DEFAULT 'user',
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            checked_out_book_id INTEGER,
            FOREIGN KEY (checked_out_book_id) REFERENCES books(id)
        );

        CREATE TABLE IF NOT EXISTS books (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            author TEXT NOT NULL,
            available_copies INTEGER NOT NULL CHECK (available_copies >= 0)
        );
    )";

    if (!db)
    {
        Logger::dbLogger->error("Database connection is null before executing SQL.");
        return SQLITE_MISUSE;
    }

    errMsg = nullptr;
    rc = sqlite3_exec(db, sql, nullptr, 0, &errMsg);

    if (rc != SQLITE_OK)
    {
        Logger::dbLogger->error("SQL error creating database tables: {}", errMsg);
        sqlite3_free(errMsg);
        return rc;
    }
    Logger::dbLogger->info("Database tables created successfully.");
    return SQLITE_OK;
}

std::string Database::hashPassword(const std::string &password)
{
    return BCrypt::generateHash(password, 12);
}

bool Database::addUser(const std::string &username, const std::string &email, const std::string &password)
{
    const char *sql = "INSERT INTO users (username, email, password, created_at, updated_at)"
                      "VALUES (?, ?, ?, datetime('now'), datetime('now'));";

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        Logger::userLogger->error("Failed to prepare user creation statement: {}", sqlite3_errmsg(db));
        return false;
    }

    // Hash the password before storing
    std::string hashedPassword = hashPassword(password);

    // Bind values securely
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, hashedPassword.c_str(), -1, SQLITE_STATIC);

    // Execute statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        Logger::userLogger->error("Error executing user creation statement: {}", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    // Finalize and clean up
    sqlite3_finalize(stmt);
    Logger::userLogger->info("User {} created successfully.", username);
    return true;
}

User *Database::authenticateUser(const std::string &username, const std::string &password)
{
    std::string query = "SELECT id, username, role, password FROM users WHERE username = ?;";
    sqlite3_stmt *stmt;

    rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        Logger::dbLogger->error("SQL error during user auth: {}", sqlite3_errmsg(db));
        return nullptr;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int userId = sqlite3_column_int(stmt, 0);
        std::string uname = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        std::string role = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        std::string storedHash = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));

        // Validate password
        if (BCrypt::validatePassword(password, storedHash))
        {
            Logger::userLogger->info("User {} authenticated successfully!", username);
            sqlite3_finalize(stmt);
            return new User(userId, uname, role);
        }
        else
        {
            Logger::userLogger->warn("User {} entered incorrect password.", username);
        }
    }

    Logger::userLogger->warn("User {} does not exist.", username);
    sqlite3_finalize(stmt);
    return nullptr;
}
