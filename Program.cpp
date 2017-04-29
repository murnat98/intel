#include "Program.h"
#include "include.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

uint16_t get2byte(uint8_t byte1, uint8_t byte2)
{
    return (byte2 << 8) | byte1;
}

uint32_t get4byte(uint16_t byte1, uint16_t byte2)
{
    return (byte2 << 16) | byte1;
}

int filesize(const std::string &fileName)
{
    struct stat file;

    if (stat(fileName.c_str(), &file) < 0)
    {
        std::cerr << "some error while reading file size!" << std::endl;

        exit(-1);
    }

    return file.st_size;
}

Program::Program(size_t size) :
    fileSize_(size),
    program_(new uint8_t[size]),
    data_(DATA_COUNT),
    error_()
{}

Program::Program(const std::string &fileName) :
    fileSize_(filesize(fileName)),
    program_(new uint8_t[fileSize_]),
    data_(DATA_COUNT),
    error_()
{
    std::ifstream fin(fileName, std::ifstream::binary);

    fin.read((char *)program_, fileSize_);

    fin.close();
}

Program::~Program()
{
    delete[] program_;
}

void Program::loadProgram(const std::string &fileName)
{
    std::ifstream fin(fileName, std::ifstream::binary);

    fileSize_ = filesize(fileName);

    fin.read((char *)program_, fileSize_);

    fin.close();
}

void Program::initData()
{
    int dataPtr = INIT_CODE;
    for (auto &data : data_)
    {
        data = get2byte(program_[dataPtr], program_[dataPtr + 1]);
        dataPtr += 2;
    }

    #define SET_RDWR_RANGES(RDWR, begin, end)\
    set ## RDWR ## Ranges(std::make_pair(begin, end));

    const int INIT_CODE_ = INIT_CODE;
    const int MEM_END_MEMORY_ = MEM_END_MEMORY;

    SET_RDWR_RANGES(Read, INIT_CODE_, INIT_CODE + data_[CODE_SZ] * 2 - 1)
    if (data_[CDATA])
        SET_RDWR_RANGES(Read, data_[CDATA], data_[CDATA] + data_[CDATA_SZ])
    if (data_[SMC])
        SET_RDWR_RANGES(Read, data_[SMC], data_[SMC] + MEM_END_MEMORY_ - data_[MEM] - data_[DATA_SZ])
    if (data_[DATA])
        SET_RDWR_RANGES(Read, data_[DATA], data_[DATA] + data_[DATA_SZ])
    if (data_[DATA])
        SET_RDWR_RANGES(Read, data_[MEM], MEM_END_MEMORY_)

    if (data_[SMC])
        SET_RDWR_RANGES(Write, data_[SMC], data_[SMC] + MEM_END_MEMORY_ - data_[MEM] - data_[DATA_SZ])
    if (data_[DATA])
        SET_RDWR_RANGES(Write, data_[DATA], data_[DATA] + data_[DATA_SZ])
    if (data_[MEM])
        SET_RDWR_RANGES(Write, data_[MEM], MEM_END_MEMORY_)

    #undef SET_RDWR_RANGES
}

bool Program::check()
{
    #define CHECK_ERROR(expr, error, code)      \
    if (expr)                                   \
    {                                           \
        error_[error] = code;                   \
        return false;                           \
    }

    #include "errors.h"

    #undef CHECK_ERROR

    return true;
}

void Program::loadToMemory()
{
    size_t memoryCounter  = 0;
    size_t programCounter = 0;

    #define FILL_MEMORY(begin, end)                                                                     \
    for (memoryCounter = begin; memoryCounter < end; ++memoryCounter)                                   \
    {                                                                                                   \
        memory_[memoryCounter] = get2byte(program_[programCounter], program_[programCounter + 1]);      \
        programCounter += 2;                                                                            \
    }

    for (memoryCounter = 0; memoryCounter < 4; memoryCounter++)
    {
        memory_[memoryCounter] = program_[programCounter++];
    }

    programCounter += INIT_DATA_SIZE - INIT_CODE;
    
    FILL_MEMORY(INIT_CODE, INIT_CODE + data_[CODE_SZ] * 2);

    if (data_[CDATA])
    {
        FILL_MEMORY(data_[CDATA], data_[CDATA] + data_[CDATA_SZ]);
    }

    if (data_[SMC])
    {
        for (memoryCounter = data_[SMC]; memoryCounter < data_[DATA] - data_[SMC]; ++memoryCounter)
        {
            memory_[memoryCounter] = 0; // now it is 0, then it will be changed
        }
    }

    if (data_[DATA])
    {
        FILL_MEMORY(data_[DATA], data_[DATA] + data_[DATA_SZ]);
    }

    if (data_[MEM])
    {
        for (memoryCounter = data_[MEM]; memoryCounter < MEM_END_MEMORY; ++memoryCounter)
        {
            memory_[memoryCounter] = 0;
        }
    }

    #undef FILL_MEMORY
}

uint8_t getOpcode(uint32_t command)
{
    return command >> 28;
}

void Program::execute()
{
    pc_ = INIT_CODE;

    int exitStatus = 0;

    while (true)
    {
        uint32_t command = get4byte (memory_[pc_], memory_[pc_ + 1]);
        uint8_t  opcode  = getOpcode(command);

        exitStatus = 0;

        switch(opcode)
        {
            #define DEFCMD(mnemonics, code)                  \
        case code:                                           \
            exitStatus = mnemonics(command);                 \
            break;

            #include "commands.h"

            #undef DEFCMD
        }

        if (exitStatus)
        {
            break;
        }

        pc_ += 2;
        if (pc_ > INIT_CODE + data_[CODE_SZ] * 2 - 1 || pc_ < INIT_CODE) // TODO: not so
        {
            if (data_[SMC])
            {
                if (pc_ < data_[SMC] || pc_ > data_[SMC] + MEM_END_MEMORY - data_[MEM] - data_[DATA_SZ])
                {
                    break;
                }
            }
            else
                break;
        }
    }
}

void Program::run()
{
    initData();

    if (!check())
    {
        for (auto error : getErrors())
        {
            std::cout << error.first << std::endl;

            //return -1;
        }
    }

    loadToMemory();

    execute();
}

std::map<std::string, int> Program::getErrors() const
{
    return error_;
}

#ifdef DEBUG

bool Program::ok() const
{
    return data_.size() == DATA_COUNT &&
           fileSize_    >= INIT_DATA_SIZE;
}

void Program::dump(const std::string &varName) const
{
    std::ofstream fout("program.dump", std::ofstream::out);

    fout << varName << std::endl;
    fout << "{" << std::endl;

    fout << "\tfileSize_ = " << fileSize_ << std::endl;
    
    fout << "program_ pointer : " << &program_ << std::endl;
    for (size_t i = 0; i < fileSize_; ++i)
    {
        fout << "\tprogram_[" << i << "] = " << (int)program_[i] << std::endl;
    }

    fout << "data_ pointer : " << &data_ << std::endl;
    int counter = 0;
    for (auto data : data_)
    {
        fout << "\tdata_[" << counter++ << "] = " << data << std::endl;
    }

    fout << "memory_ pointer : " << &memory_ << std::endl;
    counter = 0;
    for (auto memory : memory_)
    {
        fout << "\tmemory_[" << counter++ << "] = " << memory << std::endl;
    }

    bool empty = true;
    if (!error_.empty())
    {
        empty = false;

        fout << "=======================ERRORS========================" << std::endl;
    }
    for (auto error : error_)
    {
        fout << "\t" << error.first << " : " << error.second << std::endl;
    }

    if (!empty)
    {
        fout << "=======================ERRORS_END====================" << std::endl;
    }

    fout << "}" << std::endl;

    fout.close();
}

#endif // DEBUG
