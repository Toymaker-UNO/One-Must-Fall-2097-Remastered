#include "hpp/Console.hpp"

using namespace openomf::console;

static void test_console_open_close()
{
    if (!console_init()) {
        throw 1;
    }

    if (console_window_is_open()) {
        throw 2;
    }

    console_window_open();
    if (!console_window_is_open()) {
        throw 3;
    }

    console_output_add("hello ");
    console_output_addline("world");

    console_window_close();
    if (console_window_is_open()) {
        throw 4;
    }

    console_close();
}

int main()
{
    test_console_open_close();
    return 0;
}

