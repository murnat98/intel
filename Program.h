#ifndef PROGRAM_H_INCLUDED
#define PROGRAM_H_INCLUDED

#include "include.h"
#include "Cpu.h"
#include <map>
#include <vector>
#include <array>


#ifdef DEBUG

#define PROGRAM_DUMP(program) program.dump(#program);
#define ASSERT(program) if (program.ok()) { PROGRAM_DEBUG(program); assert(!"Program is not ok!");}

#else

#define PROGRAM_DUMP(program)
#define ASSERT(program)

#endif // DEBUG

class Program : public CPU
{
public:
    static const size_t INIT_SIZE      = 1000;
    static const int    INIT_DATA_SIZE = 20; // = 4 + 2 * 8
    static const int    DATA_COUNT     = 8;
    static const int    INIT_CODE      = 0x4;
    static const int    MEM_END_MEMORY = 0xefff;

    enum errors
    {
        NO_DATA,
        INCORRECT_VERSION,
        CODE_SZ_ZERO,
        MEMORY_INCORRECT,
        CDATA_FIELD_ERROR,
        DATA_FIELD_ERROR
    };

    enum datas
    {
        CODE_SZ = 0,
        CDATA,
        CDATA_SZ,
        SMC,
        DATA,
        DATA_SZ,
        MEM,
        DBG_SZ
    };

    explicit Program(size_t size = INIT_SIZE);
    explicit Program(const std::string &fileName);
    ~Program();

    void loadProgram(const std::string &fileName);

    void initData();
    bool check(); // checks file format and writes error in error_ if an error occurs
    void loadToMemory();
    void execute();

    std::map<std::string, int> getErrors() const;

    #ifdef DEBUG

    bool ok  ()                           const;
    void dump(const std::string &varName) const;

    #endif // DEBUG

private:
    size_t fileSize_;
    uint8_t *program_;
    std::vector<uint16_t> data_;
    std::map<std::string, int> error_;
};

#endif // PROGRAM_H_INCLUDED