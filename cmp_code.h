DEFCC(0x0, EQ, registers_[rs1] == registers_[rs2])
DEFCC(0x1, NE, registers_[rs1] != registers_[rs2])
DEFCC(0x2, BN, (registers_[rs1] & registers_[rs2]) == 0)
DEFCC(0x3, BS, (registers_[rs1] & registers_[rs2]) != 0)
DEFCC(0x4, LS, registers_[rs1] < registers_[rs2])
DEFCC(0x5, GT, registers_[rs1] > registers_[rs2])
DEFCC(0x6, GE, registers_[rs1] >= registers_[rs2])
DEFCC(0x7, LE, registers_[rs1] <= registers_[rs2])
DEFCC(0x8, BL, registers_[rs1] < registers_[rs2])
DEFCC(0x9, AB, registers_[rs1] > registers_[rs2])
DEFCC(0xa, BE, registers_[rs1] <= registers_[rs2])
DEFCC(0xb, AE, registers_[rs1] >= registers_[rs2])
