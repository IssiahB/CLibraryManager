#include <iostream>

#include "Logger.hpp"
#include "Database.hpp"
#include "User.hpp"

int main(int argc, char *argv[])
{
    Logger::init();
    Database *db = Database::getInstance();
    User *user = db->authenticateUser("mrbnd", "password");

    return 0;
}