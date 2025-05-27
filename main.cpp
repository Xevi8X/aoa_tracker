#include "application/application.hpp"

int main(int argc, char const *argv[])
{
    Application app;
    app.init();
    app.run();
    return 0;
}
