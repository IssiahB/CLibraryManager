#include <iostream>

#include "Logger.hpp"
#include "Database.hpp"
#include "User.hpp"
#include <CLI/CLI.hpp>

typedef struct
{

} AppContext;

int main(int argc, char *argv[])
{
    Logger::init();
    Database *db = Database::getInstance();
    CLI::App app{"A basic library manager that can be used to store a "
                 "collection of user \"created\" books.",
                 "CLibraryManager"};
    bool gui_mode = false;
    app.add_flag("-g,--gui_mode", gui_mode, "Runs the application using the Graphical User Interface");

    CLI11_PARSE(app, argc, argv);
    std::cout << gui_mode << std::endl;
    // User *user = db->authenticateUser("mrbnd", "password");

    return 0;
}