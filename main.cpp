#include "library_app.h"
#include <chrono>
#include <iostream>


int main() {

    std::chrono::seconds day_duration(10); // 10 seconds represent a day
    LibraryConsole<std::chrono::seconds> console(day_duration);
    console.run();
    return 0;
}
