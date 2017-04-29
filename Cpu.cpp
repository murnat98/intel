#include "Cpu.h"
#include <stdint.h>

uint8_t getRd(uint32_t command)
{
    return (command >> 24) & 0xf;
}

uint8_t getRs1(uint32_t command)
{
    return (command >> 20) & 0xf;
}

uint8_t getRs2(uint32_t command)
{
    return (command >> 16) & 0xf;
}

uint16_t getImm(uint32_t command)
{
    return command;
}

CPU::CPU() :
    memory_(),
    pc_(INIT_CODE),
    registers_()
{}

CPU::~CPU()
{}

#define DEFCMD(mnemonics) int CPU::mnemonics(uint32_t command)

DEFCMD(cmdAdd)
{
    uint8_t  rd  = getRd (command);
    uint8_t  rs1 = getRs1(command);
    uint8_t  rs2 = getRs2(command);
    uint16_t imm = getImm(command);

    if (!rd)
    {
        return 0;
    }

    registers_[rd] = registers_[rs1] + (registers_[rs2] | imm);

    return 0;
}

DEFCMD(cmdSub)
{
    uint8_t  rd  = getRd (command);
    uint8_t  rs1 = getRs1(command);
    uint8_t  rs2 = getRs2(command);
    uint16_t imm = getImm(command);

    if (!rd)
    {
        return 0;
    }

    registers_[rd] = registers_[rs1] - (registers_[rs2] | imm);

    return 0;
}

DEFCMD(cmdMul)
{
    uint8_t  rd  = getRd (command);
    uint8_t  rs1 = getRs1(command);
    uint8_t  rs2 = getRs2(command);
    uint16_t imm = getImm(command);

    if (!rd)
    {
        return 0;
    }

    registers_[rd] = registers_[rs1] * (registers_[rs2] | imm);

    return 0;
}

DEFCMD(cmdModu)
{
    uint8_t  rd  = getRd (command);
    uint8_t  rs1 = getRs1(command);
    uint8_t  rs2 = getRs2(command);
    uint16_t imm = getImm(command);

    if (!rd)
    {
        return 0;
    }

    registers_[rd] = registers_[rs1] % (registers_[rs2] | imm);

    return 0;
}

DEFCMD(cmdDiv)
{
    uint8_t  rd  = getRd (command);
    uint8_t  rs1 = getRs1(command);
    uint8_t  rs2 = getRs2(command);
    uint16_t imm = getImm(command);

    if (!rd)
    {
        return 0;
    }

    registers_[rd] = registers_[rs1] / (registers_[rs2] | imm);

    return 0;
}

DEFCMD(cmdDivu)
{
    return cmdDiv(command);
}

DEFCMD(cmdOrnot)
{
    uint8_t  rd  = getRd (command);
    uint8_t  rs1 = getRs1(command);
    uint8_t  rs2 = getRs2(command);
    uint16_t imm = getImm(command);

    if (!rd)
    {
        return 0;
    }

    registers_[rd] = registers_[rs1] | ~(registers_[rs2] | imm);

    return 0;
}

DEFCMD(cmdAnd)
{
    uint8_t  rd  = getRd (command);
    uint8_t  rs1 = getRs1(command);
    uint8_t  rs2 = getRs2(command);
    uint16_t imm = getImm(command);

    if (!rd)
    {
        return 0;
    }

    registers_[rd] = rs1 & (rs2 | imm);

    return 0;
}

DEFCMD(cmdLsl)
{
    uint8_t  rd  = getRd (command);
    uint8_t  rs1 = getRs1(command);
    uint8_t  rs2 = getRs2(command);
    uint16_t imm = getImm(command);

    if (!rd)
    {
        return 0;
    }

    registers_[rd] = registers_[rs1] << (registers_[rs2] | imm);

    return 0;
}

DEFCMD(cmdLsr)
{
    uint8_t  rd  = getRd (command);
    uint8_t  rs1 = getRs1(command);
    uint8_t  rs2 = getRs2(command);
    uint16_t imm = getImm(command);

    if (!rd)
    {
        return 0;
    }

    registers_[rd] = registers_[rs1] >> (registers_[rs2] | imm);

    return 0;
}

DEFCMD(cmdAsr)
{
    uint8_t  rd  = getRd (command);
    uint8_t  rs1 = getRs1(command);
    uint8_t  rs2 = getRs2(command);
    uint16_t imm = getImm(command);

    if (!rd)
    {
        return 0;
    }

    registers_[rd] = (signed)registers_[rs1] >> (signed)(registers_[rs2] | imm); // improve here

    return 0;
}

DEFCMD(cmdCmp)
{
    uint8_t  rd  = getRd (command);
    uint8_t  rs1 = getRs1(command);
    uint8_t  rs2 = getRs2(command);
    uint16_t imm = getImm(command);

    if (!rd)
    {
        return 0;
    }

    switch(imm)
    {
        #define DEFCC(code, mnemonics, expr)                                        \
    case code:                                                                      \
        registers_[rd] = (expr) ? 0 : 1;                                            \
        break;

        #include "cmp_code.h"

        #undef DEFCC
    }

    return 0;
}

DEFCMD(cmdBrn)
{
    uint8_t  rd  = getRd (command);
    uint8_t  rs1 = getRs1(command);
    uint8_t  rs2 = getRs2(command);
    uint16_t imm = getImm(command);

    if (!rd)
    {
        return 0;
    }

    registers_[rd] = pc_ + 2;

    if (!registers_[rs1])
    {
        uint16_t dst = registers_[rs2] | imm;
        if (dst & 0x3)
            return -1;
        else
            pc_ = dst;
    }

    return 0;
}

DEFCMD(cmdLd)
{
    uint8_t  rd  = getRd (command);
    uint8_t  rs1 = getRs1(command);
    uint8_t  rs2 = getRs2(command);
    uint16_t imm = getImm(command);

    if (!rd)
    {
        return 0;
    }
    
    uint16_t memory = registers_[rs1] + registers_[rs2] + imm;

    if (checkReadRanges(memory))
    {
        registers_[rd] = memory_[memory];
    }

    return 0;
}

DEFCMD(cmdSt)
{
    uint8_t  rd  = getRd (command);
    uint8_t  rs1 = getRs1(command);
    uint8_t  rs2 = getRs2(command);
    uint16_t imm = getImm(command);

    uint16_t memory = registers_[rs1] + registers_[rs2] + imm;

    if (checkWriteRanges(memory))
    {
        memory_[memory] = registers_[rd];
    }

    return 0;
}

DEFCMD(cmdReserved)
{
    return 1;
}

#undef DEFCMD

void CPU::printRegisters() const
{
    size_t counter = 0;
    for (auto Register : registers_)
    {
        std::cout << "r" << counter << (counter >= 10 ? "" : " ") << " = " << Register << std::endl;

        counter++;
    }
}

void CPU::setReadRanges (const Ranges &ranges)
{
    readRanges_.push_back(ranges);
}

void CPU::setWriteRanges (const Ranges &ranges)
{
    writeRanges_.push_back(ranges);
}

#define CHECK_CODE(RDWR)                                                        \
for (const auto &range : RDWR)                                                  \
    {                                                                           \
        if (memory >= std::get<0>(range) && memory <= std::get<1>(range))       \
        {                                                                       \
            return true;                                                        \
        }                                                                       \
    }

bool CPU::checkReadRanges(uint16_t memory) const
{
    CHECK_CODE(readRanges_)

    return false;
}

bool CPU::checkWriteRanges(uint16_t memory) const
{
    CHECK_CODE(writeRanges_)

    return false;
}

#undef CHECK_CODE
