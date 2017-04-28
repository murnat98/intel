#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include "include.h"
#include <array>
#include <vector>
#include <utility>
#include <stdint.h>

using Ranges = std::pair<uint16_t, uint16_t>;

class CPU
{
public:
    static const int MEMORY_SIZE     = 0xffff;
    static const int REGISTERS_COUNT = 16;
    static const int INIT_CODE       = 0x4;

    CPU();
    ~CPU();

    #define DEFCMD(mnemonics, code)     \
    int mnemonics(uint32_t command);

    #include "commands.h"

    #undef DEFCMD

    void printRegisters() const;

    void setReadRanges (const Ranges &ranges);
    void setWriteRanges(const Ranges &ranges);

    bool checkReadRanges (uint16_t memory) const;
    bool checkWriteRanges(uint16_t memory) const;

protected:
    std::array<uint16_t, MEMORY_SIZE> memory_;
    uint16_t pc_;

private:
    std::array<uint16_t, REGISTERS_COUNT> registers_;
    std::vector<Ranges> readRanges_;
    std::vector<Ranges> writeRanges_;
};

#endif // CPU_H_INCLUDED