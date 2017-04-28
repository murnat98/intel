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

    Program program;
    program.loadProgram(argv[1]);

    program.initData();

    if (!program.check())
    {
        for (auto error : program.getErrors())
        {
            std::cout << error.first << std::endl;

            //return -1;
        }
    }

    program.loadToMemory();

    program.execute();

    program.printRegisters();

    PROGRAM_DUMP(program);

    return 0;
}