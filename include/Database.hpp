#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <sqlite3.h>
#include <string>

#include "User.hpp"

// This class manages any queries and interactions with the library database.
// Implements the Singleton pattern making only one instance of this class available
class Database
{
private:
    static Database *instance;
    sqlite3 *db;
    char *errMsg = 0;
    int rc;

    Database();
    int createTables();
    static void initLogger();
    std::string hashPassword(const std::string &password);

public:
    ~Database();
    static Database *getInstance();
    bool addUser(const std::string &username, const std::string &email, const std::string &password);
    User *authenticateUser(const std::string &username, const std::string &password);
};

#endif