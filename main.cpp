#include "include.h"
#include "Program.h"
#include <string>

#include "Memory.h"

int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        std::cerr << "Program gets 1 argument(file name)" << std::endl;

        return -1;
    }

    std::string fileName = argv[1];

    Program program(fileName);

    program.run();

    program.printRegisters();

    return 0;
}