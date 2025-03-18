#ifndef USER_HPP
#define USER_HPP
#include <string>

class User
{
private:
    int id;
    std::string username;
    std::string role;

public:
    User(int userId, std::string uname, std::string userRole)
        : id(userId), username(uname), role(userRole) {}

    bool isAdmin() const
    {
        return role == "admin";
    }
};

#endif