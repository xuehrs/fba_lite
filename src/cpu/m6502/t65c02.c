/*****************************************************************************
 *
 *   tbl65c02.c
 *   65c02 opcode functions and function pointer table
 *
 *   Copyright Juergen Buchmueller, all rights reserved.
 *
 *   - This source code is released as freeware for non-commercial purposes.
 *   - You are free to use and redistribute this code in modified or
 *     unmodified form, provided you list me in the credits.
 *   - If you modify this source code, you must add a notice to each modified
 *     source file that it has been changed.  If you're a nice person, you
 *     will clearly mark each change too.  :)
 *   - If you wish to use this for commercial purposes, please contact me at
 *     pullmoll@t-online.de
 *   - The author of this copywritten work reserves the right to change the
 *     terms of its usage and license at any time, including retroactively
 *   - This entire notice must remain in the source code.
 *
 * Not sure about the timing of all the extra (undocumented) NOP instructions.
 * Core may need to be split up into two 65c02 core. Not all versions supported
 * the bit operation RMB/SMB/etc.
 *
 *****************************************************************************/

#undef	OP
#define OP(nn) M6502_INLINE void m65c02_##nn(void)
#define RD_IMM_DISCARD		RDOPARG()
#define RD_ZPG_DISCARD		EA_ZPG; RDMEM(EAD)
#define RD_ZPX_DISCARD		EA_ZPX; RDMEM(EAD)

/*****************************************************************************
 *****************************************************************************
 *
 *  Implementations for 65C02 opcodes
 *
 *  There are a few slight differences between Rockwell and WDC 65C02 CPUs.
 *  The absolute indexed addressing mode RMW instructions take 6 cycles on
 *  WDC 65C02 CPU but 7 cycles on a regular 6502 and a Rockwell 65C02 CPU.
 *  TODO: Implement STP and WAI for wdc65c02.
 *
 *****************************************************************************
 * op    temp     cycles             rdmem   opc  wrmem   ********************/
OP(00)
{
    BRK_C02;                                      /* 7 BRK */
}
OP(20)
{
    JSR;                                          /* 6 JSR */
}
OP(40)
{
    RTI;                                          /* 6 RTI */
}
OP(60)
{
    RTS;                                          /* 6 RTS */
}
OP(80)
{
    int tmp;    /* 3-4 BRA REL */
    BRA_C02( 1 );
}
OP(a0)
{
    int tmp;    /* 2 LDY IMM */
    RD_IMM;
    LDY;
}
OP(c0)
{
    int tmp;    /* 2 CPY IMM */
    RD_IMM;
    CPY;
}
OP(e0)
{
    int tmp;    /* 2 CPX IMM */
    RD_IMM;
    CPX;
}

OP(10)
{
    int tmp;    /* 2-4 BPL REL */
    BRA_C02( ! ( P & F_N ) );
}
OP(30)
{
    int tmp;    /* 2-4 BMI REL */
    BRA_C02(   ( P & F_N ) );
}
OP(50)
{
    int tmp;    /* 2-4 BVC REL */
    BRA_C02( ! ( P & F_V ) );
}
OP(70)
{
    int tmp;    /* 2-4 BVS REL */
    BRA_C02(   ( P & F_V ) );
}
OP(90)
{
    int tmp;    /* 2-4 BCC REL */
    BRA_C02( ! ( P & F_C ) );
}
OP(b0)
{
    int tmp;    /* 2-4 BCS REL */
    BRA_C02(   ( P & F_C ) );
}
OP(d0)
{
    int tmp;    /* 2-4 BNE REL */
    BRA_C02( ! ( P & F_Z ) );
}
OP(f0)
{
    int tmp;    /* 2-4 BEQ REL */
    BRA_C02(   ( P & F_Z ) );
}

OP(01)
{
    int tmp;    /* 6 ORA IDX */
    RD_IDX;
    ORA;
}
OP(21)
{
    int tmp;    /* 6 AND IDX */
    RD_IDX;
    AND;
}
OP(41)
{
    int tmp;    /* 6 EOR IDX */
    RD_IDX;
    EOR;
}
OP(61)
{
    int tmp;    /* 6/7 ADC IDX */
    RD_IDX;
    ADC_C02;
}
OP(81)
{
    int tmp;    /* 6 STA IDX */
    STA;
    WR_IDX;
}
OP(a1)
{
    int tmp;    /* 6 LDA IDX */
    RD_IDX;
    LDA;
}
OP(c1)
{
    int tmp;    /* 6 CMP IDX */
    RD_IDX;
    CMP;
}
OP(e1)
{
    int tmp;    /* 6/7 SBC IDX */
    RD_IDX;
    SBC_C02;
}

OP(11)
{
    int tmp;    /* 5 ORA IDY page penalty */
    RD_IDY_C02_P;
    ORA;
}
OP(31)
{
    int tmp;    /* 5 AND IDY page penalty */
    RD_IDY_C02_P;
    AND;
}
OP(51)
{
    int tmp;    /* 5 EOR IDY page penalty */
    RD_IDY_C02_P;
    EOR;
}
OP(71)
{
    int tmp;    /* 5/6 ADC IDY page penalty */
    RD_IDY_C02_P;
    ADC_C02;
}
OP(91)
{
    int tmp;    /* 6 STA IDY */
    STA;
    WR_IDY_C02_NP;
}
OP(b1)
{
    int tmp;    /* 5 LDA IDY page penalty */
    RD_IDY_C02_P;
    LDA;
}
OP(d1)
{
    int tmp;    /* 5 CMP IDY page penalty */
    RD_IDY_C02_P;
    CMP;
}
OP(f1)
{
    int tmp;    /* 5/6 SBC IDY page penalty */
    RD_IDY_C02_P;
    SBC_C02;
}

OP(02)
{
    RD_IMM_DISCARD;    /* 2 NOP not sure for rockwell */
    NOP;
}
OP(22)
{
    RD_IMM_DISCARD;    /* 2 NOP not sure for rockwell */
    NOP;
}
OP(42)
{
    RD_IMM_DISCARD;    /* 2 NOP not sure for rockwell */
    NOP;
}
OP(62)
{
    RD_IMM_DISCARD;    /* 2 NOP not sure for rockwell */
    NOP;
}
OP(82)
{
    RD_IMM_DISCARD;    /* 2 NOP not sure for rockwell */
    NOP;
}
OP(a2)
{
    int tmp;    /* 2 LDX IMM */
    RD_IMM;
    LDX;
}
OP(c2)
{
    RD_IMM_DISCARD;    /* 2 NOP not sure for rockwell */
    NOP;
}
OP(e2)
{
    RD_IMM_DISCARD;    /* 2 NOP not sure for rockwell */
    NOP;
}

OP(12)
{
    int tmp;    /* 5 ORA ZPI */
    RD_ZPI;
    ORA;
}
OP(32)
{
    int tmp;    /* 5 AND ZPI */
    RD_ZPI;
    AND;
}
OP(52)
{
    int tmp;    /* 5 EOR ZPI */
    RD_ZPI;
    EOR;
}
OP(72)
{
    int tmp;    /* 5/6 ADC ZPI */
    RD_ZPI;
    ADC_C02;
}
OP(92)
{
    int tmp;    /* 5 STA ZPI */
    STA;
    WR_ZPI;
}
OP(b2)
{
    int tmp;    /* 5 LDA ZPI */
    RD_ZPI;
    LDA;
}
OP(d2)
{
    int tmp;    /* 5 CMP ZPI */
    RD_ZPI;
    CMP;
}
OP(f2)
{
    int tmp;    /* 5/6 SBC ZPI */
    RD_ZPI;
    SBC_C02;
}

OP(03)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(23)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(43)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(63)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(83)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(a3)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(c3)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(e3)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}

OP(13)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(33)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(53)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(73)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(93)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(b3)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(d3)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(f3)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}

OP(04)
{
    int tmp;    /* 5 TSB ZPG */
    RD_ZPG;
    RD_EA;
    TSB;
    WB_EA;
}
OP(24)
{
    int tmp;    /* 3 BIT ZPG */
    RD_ZPG;
    BIT;
}
OP(44)
{
    RD_ZPG_DISCARD;    /* 3 NOP not sure for rockwell */
    NOP;
}
OP(64)
{
    int tmp;    /* 3 STZ ZPG */
    STZ;
    WR_ZPG;
}
OP(84)
{
    int tmp;    /* 3 STY ZPG */
    STY;
    WR_ZPG;
}
OP(a4)
{
    int tmp;    /* 3 LDY ZPG */
    RD_ZPG;
    LDY;
}
OP(c4)
{
    int tmp;    /* 3 CPY ZPG */
    RD_ZPG;
    CPY;
}
OP(e4)
{
    int tmp;    /* 3 CPX ZPG */
    RD_ZPG;
    CPX;
}

OP(14)
{
    int tmp;    /* 5 TRB ZPG */
    RD_ZPG;
    RD_EA;
    TRB;
    WB_EA;
}
OP(34)
{
    int tmp;    /* 4 BIT ZPX */
    RD_ZPX;
    BIT;
}
OP(54)
{
    RD_ZPX_DISCARD;    /* 4 NOP not sure for rockwell */
    NOP;
}
OP(74)
{
    int tmp;    /* 4 STZ ZPX */
    STZ;
    WR_ZPX;
}
OP(94)
{
    int tmp;    /* 4 STY ZPX */
    STY;
    WR_ZPX;
}
OP(b4)
{
    int tmp;    /* 4 LDY ZPX */
    RD_ZPX;
    LDY;
}
OP(d4)
{
    RD_ZPX_DISCARD;    /* 4 NOP not sure for rockwell */
    NOP;
}
OP(f4)
{
    RD_ZPX_DISCARD;    /* 4 NOP not sure for rockwell */
    NOP;
}

OP(05)
{
    int tmp;    /* 3 ORA ZPG */
    RD_ZPG;
    ORA;
}
OP(25)
{
    int tmp;    /* 3 AND ZPG */
    RD_ZPG;
    AND;
}
OP(45)
{
    int tmp;    /* 3 EOR ZPG */
    RD_ZPG;
    EOR;
}
OP(65)
{
    int tmp;    /* 3/4 ADC ZPG */
    RD_ZPG;
    ADC_C02;
}
OP(85)
{
    int tmp;    /* 3 STA ZPG */
    STA;
    WR_ZPG;
}
OP(a5)
{
    int tmp;    /* 3 LDA ZPG */
    RD_ZPG;
    LDA;
}
OP(c5)
{
    int tmp;    /* 3 CMP ZPG */
    RD_ZPG;
    CMP;
}
OP(e5)
{
    int tmp;    /* 3/4 SBC ZPG */
    RD_ZPG;
    SBC_C02;
}

OP(15)
{
    int tmp;    /* 4 ORA ZPX */
    RD_ZPX;
    ORA;
}
OP(35)
{
    int tmp;    /* 4 AND ZPX */
    RD_ZPX;
    AND;
}
OP(55)
{
    int tmp;    /* 4 EOR ZPX */
    RD_ZPX;
    EOR;
}
OP(75)
{
    int tmp;    /* 4/5 ADC ZPX */
    RD_ZPX;
    ADC_C02;
}
OP(95)
{
    int tmp;    /* 4 STA ZPX */
    STA;
    WR_ZPX;
}
OP(b5)
{
    int tmp;    /* 4 LDA ZPX */
    RD_ZPX;
    LDA;
}
OP(d5)
{
    int tmp;    /* 4 CMP ZPX */
    RD_ZPX;
    CMP;
}
OP(f5)
{
    int tmp;    /* 4/5 SBC ZPX */
    RD_ZPX;
    SBC_C02;
}

OP(06)
{
    int tmp;    /* 5 ASL ZPG */
    RD_ZPG, RD_EA;
    ASL;
    WB_EA;
}
OP(26)
{
    int tmp;    /* 5 ROL ZPG */
    RD_ZPG;
    RD_EA;
    ROL;
    WB_EA;
}
OP(46)
{
    int tmp;    /* 5 LSR ZPG */
    RD_ZPG;
    RD_EA;
    LSR;
    WB_EA;
}
OP(66)
{
    int tmp;    /* 5 ROR ZPG */
    RD_ZPG;
    RD_EA;
    ROR;
    WB_EA;
}
OP(86)
{
    int tmp;    /* 3 STX ZPG */
    STX;
    WR_ZPG;
}
OP(a6)
{
    int tmp;    /* 3 LDX ZPG */
    RD_ZPG;
    LDX;
}
OP(c6)
{
    int tmp;    /* 5 DEC ZPG */
    RD_ZPG;
    RD_EA;
    DEC;
    WB_EA;
}
OP(e6)
{
    int tmp;    /* 5 INC ZPG */
    RD_ZPG;
    RD_EA;
    INC;
    WB_EA;
}

OP(16)
{
    int tmp;    /* 6 ASL ZPX */
    RD_ZPX;
    RD_EA;
    ASL;
    WB_EA;
}
OP(36)
{
    int tmp;    /* 6 ROL ZPX */
    RD_ZPX;
    RD_EA;
    ROL;
    WB_EA;
}
OP(56)
{
    int tmp;    /* 6 LSR ZPX */
    RD_ZPX;
    RD_EA;
    LSR;
    WB_EA;
}
OP(76)
{
    int tmp;    /* 6 ROR ZPX */
    RD_ZPX;
    RD_EA;
    ROR;
    WB_EA;
}
OP(96)
{
    int tmp;    /* 4 STX ZPY */
    STX;
    WR_ZPY;
}
OP(b6)
{
    int tmp;    /* 4 LDX ZPY */
    RD_ZPY;
    LDX;
}
OP(d6)
{
    int tmp;    /* 6 DEC ZPX */
    RD_ZPX;
    RD_EA;
    DEC;
    WB_EA;
}
OP(f6)
{
    int tmp;    /* 6 INC ZPX */
    RD_ZPX;
    RD_EA;
    INC;
    WB_EA;
}

OP(07)
{
    int tmp;    /* 5 RMB0 ZPG */
    RD_ZPG;
    RD_EA;
    RMB(0);
    WB_EA;
}
OP(27)
{
    int tmp;    /* 5 RMB2 ZPG */
    RD_ZPG;
    RD_EA;
    RMB(2);
    WB_EA;
}
OP(47)
{
    int tmp;    /* 5 RMB4 ZPG */
    RD_ZPG;
    RD_EA;
    RMB(4);
    WB_EA;
}
OP(67)
{
    int tmp;    /* 5 RMB6 ZPG */
    RD_ZPG;
    RD_EA;
    RMB(6);
    WB_EA;
}
OP(87)
{
    int tmp;    /* 5 SMB0 ZPG */
    RD_ZPG;
    RD_EA;
    SMB(0);
    WB_EA;
}
OP(a7)
{
    int tmp;    /* 5 SMB2 ZPG */
    RD_ZPG;
    RD_EA;
    SMB(2);
    WB_EA;
}
OP(c7)
{
    int tmp;    /* 5 SMB4 ZPG */
    RD_ZPG;
    RD_EA;
    SMB(4);
    WB_EA;
}
OP(e7)
{
    int tmp;    /* 5 SMB6 ZPG */
    RD_ZPG;
    RD_EA;
    SMB(6);
    WB_EA;
}

OP(17)
{
    int tmp;    /* 5 RMB1 ZPG */
    RD_ZPG;
    RD_EA;
    RMB(1);
    WB_EA;
}
OP(37)
{
    int tmp;    /* 5 RMB3 ZPG */
    RD_ZPG;
    RD_EA;
    RMB(3);
    WB_EA;
}
OP(57)
{
    int tmp;    /* 5 RMB5 ZPG */
    RD_ZPG;
    RD_EA;
    RMB(5);
    WB_EA;
}
OP(77)
{
    int tmp;    /* 5 RMB7 ZPG */
    RD_ZPG;
    RD_EA;
    RMB(7);
    WB_EA;
}
OP(97)
{
    int tmp;    /* 5 SMB1 ZPG */
    RD_ZPG;
    RD_EA;
    SMB(1);
    WB_EA;
}
OP(b7)
{
    int tmp;    /* 5 SMB3 ZPG */
    RD_ZPG;
    RD_EA;
    SMB(3);
    WB_EA;
}
OP(d7)
{
    int tmp;    /* 5 SMB5 ZPG */
    RD_ZPG;
    RD_EA;
    SMB(5);
    WB_EA;
}
OP(f7)
{
    int tmp;    /* 5 SMB7 ZPG */
    RD_ZPG;
    RD_EA;
    SMB(7);
    WB_EA;
}

OP(08)
{
    RD_DUM;    /* 3 PHP */
    PHP;
}
OP(28)
{
    RD_DUM;    /* 4 PLP */
    PLP;
}
OP(48)
{
    RD_DUM;    /* 3 PHA */
    PHA;
}
OP(68)
{
    RD_DUM;    /* 4 PLA */
    PLA;
}
OP(88)
{
    RD_DUM;    /* 2 DEY */
    DEY;
}
OP(a8)
{
    RD_DUM;    /* 2 TAY */
    TAY;
}
OP(c8)
{
    RD_DUM;    /* 2 INY */
    INY;
}
OP(e8)
{
    RD_DUM;    /* 2 INX */
    INX;
}

OP(18)
{
    RD_DUM;    /* 2 CLC */
    CLC;
}
OP(38)
{
    RD_DUM;    /* 2 SEC */
    SEC;
}
OP(58)
{
    RD_DUM;    /* 2 CLI */
    CLI;
}
OP(78)
{
    RD_DUM;    /* 2 SEI */
    SEI;
}
OP(98)
{
    RD_DUM;    /* 2 TYA */
    TYA;
}
OP(b8)
{
    RD_DUM;    /* 2 CLV */
    CLV;
}
OP(d8)
{
    RD_DUM;    /* 2 CLD */
    CLD;
}
OP(f8)
{
    RD_DUM;    /* 2 SED */
    SED;
}

OP(09)
{
    int tmp;    /* 2 ORA IMM */
    RD_IMM;
    ORA;
}
OP(29)
{
    int tmp;    /* 2 AND IMM */
    RD_IMM;
    AND;
}
OP(49)
{
    int tmp;    /* 2 EOR IMM */
    RD_IMM;
    EOR;
}
OP(69)
{
    int tmp;    /* 2/3 ADC IMM */
    RD_IMM;
    ADC_C02;
}
OP(89)
{
    int tmp;    /* 2 BIT IMM */
    RD_IMM;
    BIT_IMM_C02;
}
OP(a9)
{
    int tmp;    /* 2 LDA IMM */
    RD_IMM;
    LDA;
}
OP(c9)
{
    int tmp;    /* 2 CMP IMM */
    RD_IMM;
    CMP;
}
OP(e9)
{
    int tmp;    /* 2/3 SBC IMM */
    RD_IMM;
    SBC_C02;
}

OP(19)
{
    int tmp;    /* 4 ORA ABY page penalty */
    RD_ABY_C02_P;
    ORA;
}
OP(39)
{
    int tmp;    /* 4 AND ABY page penalty */
    RD_ABY_C02_P;
    AND;
}
OP(59)
{
    int tmp;    /* 4 EOR ABY page penalty */
    RD_ABY_C02_P;
    EOR;
}
OP(79)
{
    int tmp;    /* 4/5 ADC ABY page penalty */
    RD_ABY_C02_P;
    ADC_C02;
}
OP(99)
{
    int tmp;    /* 5 STA ABY */
    STA;
    WR_ABY_C02_NP;
}
OP(b9)
{
    int tmp;    /* 4 LDA ABY page penalty */
    RD_ABY_C02_P;
    LDA;
}
OP(d9)
{
    int tmp;    /* 4 CMP ABY page penalty */
    RD_ABY_C02_P;
    CMP;
}
OP(f9)
{
    int tmp;    /* 4/5 SBC ABY page penalty */
    RD_ABY_C02_P;
    SBC_C02;
}

OP(0a)
{
    int tmp;    /* 2 ASL A */
    RD_DUM;
    RD_ACC;
    ASL;
    WB_ACC;
}
OP(2a)
{
    int tmp;    /* 2 ROL A */
    RD_DUM;
    RD_ACC;
    ROL;
    WB_ACC;
}
OP(4a)
{
    int tmp;    /* 2 LSR A */
    RD_DUM;
    RD_ACC;
    LSR;
    WB_ACC;
}
OP(6a)
{
    int tmp;    /* 2 ROR A */
    RD_DUM;
    RD_ACC;
    ROR;
    WB_ACC;
}
OP(8a)
{
    RD_DUM;    /* 2 TXA */
    TXA;
}
OP(aa)
{
    RD_DUM;    /* 2 TAX */
    TAX;
}
OP(ca)
{
    RD_DUM;    /* 2 DEX */
    DEX;
}
OP(ea)
{
    RD_DUM;    /* 2 NOP */
    NOP;
}

OP(1a)
{
    RD_DUM;    /* 2 INA */
    INA;
}
OP(3a)
{
    RD_DUM;    /* 2 DEA */
    DEA;
}
OP(5a)
{
    RD_DUM;    /* 3 PHY */
    PHY;
}
OP(7a)
{
    RD_DUM;    /* 4 PLY */
    PLY;
}
OP(9a)
{
    RD_DUM;    /* 2 TXS */
    TXS;
}
OP(ba)
{
    RD_DUM;    /* 2 TSX */
    TSX;
}
OP(da)
{
    RD_DUM;    /* 3 PHX */
    PHX;
}
OP(fa)
{
    RD_DUM;    /* 4 PLX */
    PLX;
}

OP(0b)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(2b)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(4b)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(6b)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(8b)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(ab)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(cb)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(eb)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}

OP(1b)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(3b)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(5b)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(7b)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(9b)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(bb)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(db)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}
OP(fb)
{
    NOP;                                          /* 1 NOP not sure for rockwell */
}

OP(0c)
{
    int tmp;    /* 6 TSB ABS */
    RD_ABS;
    RD_EA;
    TSB;
    WB_EA;
}
OP(2c)
{
    int tmp;    /* 4 BIT ABS */
    RD_ABS;
    BIT;
}
OP(4c)
{
    EA_ABS;    /* 3 JMP ABS */
    JMP;
}
OP(6c)
{
    int tmp;    /* 6 JMP IND */
    EA_IND_C02;
    JMP;
}
OP(8c)
{
    int tmp;    /* 4 STY ABS */
    STY;
    WR_ABS;
}
OP(ac)
{
    int tmp;    /* 4 LDY ABS */
    RD_ABS;
    LDY;
}
OP(cc)
{
    int tmp;    /* 4 CPY ABS */
    RD_ABS;
    CPY;
}
OP(ec)
{
    int tmp;    /* 4 CPX ABS */
    RD_ABS;
    CPX;
}

OP(1c)
{
    int tmp;    /* 6 TRB ABS */
    RD_ABS;
    RD_EA;
    TRB;
    WB_EA;
}
OP(3c)
{
    int tmp;    /* 4 BIT ABX page penalty */
    RD_ABX_C02_P;
    BIT;
}
OP(5c)
{
    RD_ABX_C02_NP_DISCARD;    /* 8 NOP ABX not sure for rockwell. Page penalty not sure */
    RD_DUM;
    RD_DUM;
    RD_DUM;
    RD_DUM;
}
OP(7c)
{
    int tmp;    /* 6 JMP IAX page penalty */
    EA_IAX;
    JMP;
}
OP(9c)
{
    int tmp;    /* 4 STZ ABS */
    STZ;
    WR_ABS;
}
OP(bc)
{
    int tmp;    /* 4 LDY ABX page penalty */
    RD_ABX_C02_P;
    LDY;
}
OP(dc)
{
    RD_ABX_C02_NP_DISCARD;    /* 4 NOP ABX not sure for rockwell. Page penalty not sure  */
    NOP;
}
OP(fc)
{
    RD_ABX_C02_NP_DISCARD;    /* 4 NOP ABX not sure for rockwell. Page penalty not sure  */
    NOP;
}

OP(0d)
{
    int tmp;    /* 4 ORA ABS */
    RD_ABS;
    ORA;
}
OP(2d)
{
    int tmp;    /* 4 AND ABS */
    RD_ABS;
    AND;
}
OP(4d)
{
    int tmp;    /* 4 EOR ABS */
    RD_ABS;
    EOR;
}
OP(6d)
{
    int tmp;    /* 4/5 ADC ABS */
    RD_ABS;
    ADC_C02;
}
OP(8d)
{
    int tmp;    /* 4 STA ABS */
    STA;
    WR_ABS;
}
OP(ad)
{
    int tmp;    /* 4 LDA ABS */
    RD_ABS;
    LDA;
}
OP(cd)
{
    int tmp;    /* 4 CMP ABS */
    RD_ABS;
    CMP;
}
OP(ed)
{
    int tmp;    /* 4/5 SBC ABS */
    RD_ABS;
    SBC_C02;
}

OP(1d)
{
    int tmp;    /* 4 ORA ABX page penalty */
    RD_ABX_C02_P;
    ORA;
}
OP(3d)
{
    int tmp;    /* 4 AND ABX page penalty */
    RD_ABX_C02_P;
    AND;
}
OP(5d)
{
    int tmp;    /* 4 EOR ABX page penalty */
    RD_ABX_C02_P;
    EOR;
}
OP(7d)
{
    int tmp;    /* 4/5 ADC ABX page penalty */
    RD_ABX_C02_P;
    ADC_C02;
}
OP(9d)
{
    int tmp;    /* 5 STA ABX */
    STA;
    WR_ABX_C02_NP;
}
OP(bd)
{
    int tmp;    /* 4 LDA ABX page penalty */
    RD_ABX_C02_P;
    LDA;
}
OP(dd)
{
    int tmp;    /* 4 CMP ABX page penalty */
    RD_ABX_C02_P;
    CMP;
}
OP(fd)
{
    int tmp;    /* 4/5 SBC ABX page penalty */
    RD_ABX_C02_P;
    SBC_C02;
}

OP(0e)
{
    int tmp;    /* 6 ASL ABS */
    RD_ABS;
    RD_EA;
    ASL;
    WB_EA;
}
OP(2e)
{
    int tmp;    /* 6 ROL ABS */
    RD_ABS;
    RD_EA;
    ROL;
    WB_EA;
}
OP(4e)
{
    int tmp;    /* 6 LSR ABS */
    RD_ABS;
    RD_EA;
    LSR;
    WB_EA;
}
OP(6e)
{
    int tmp;    /* 6 ROR ABS */
    RD_ABS;
    RD_EA;
    ROR;
    WB_EA;
}
OP(8e)
{
    int tmp;    /* 4 STX ABS */
    STX;
    WR_ABS;
}
OP(ae)
{
    int tmp;    /* 4 LDX ABS */
    RD_ABS;
    LDX;
}
OP(ce)
{
    int tmp;    /* 6 DEC ABS */
    RD_ABS;
    RD_EA;
    DEC;
    WB_EA;
}
OP(ee)
{
    int tmp;    /* 6 INC ABS */
    RD_ABS;
    RD_EA;
    INC;
    WB_EA;
}

OP(1e)
{
    int tmp;    /* 7 ASL ABX */
    RD_ABX_C02_NP;
    RD_EA;
    ASL;
    WB_EA;
}
OP(3e)
{
    int tmp;    /* 7 ROL ABX */
    RD_ABX_C02_NP;
    RD_EA;
    ROL;
    WB_EA;
}
OP(5e)
{
    int tmp;    /* 7 LSR ABX */
    RD_ABX_C02_NP;
    RD_EA;
    LSR;
    WB_EA;
}
OP(7e)
{
    int tmp;    /* 7 ROR ABX */
    RD_ABX_C02_NP;
    RD_EA;
    ROR;
    WB_EA;
}
OP(9e)
{
    int tmp;    /* 5 STZ ABX */
    STZ;
    WR_ABX_C02_NP;
}
OP(be)
{
    int tmp;    /* 4 LDX ABY page penalty */
    RD_ABY_C02_P;
    LDX;
}
OP(de)
{
    int tmp;    /* 7 DEC ABX */
    RD_ABX_C02_NP;
    RD_EA;
    DEC;
    WB_EA;
}
OP(fe)
{
    int tmp;    /* 7 INC ABX */
    RD_ABX_C02_NP;
    RD_EA;
    INC;
    WB_EA;
}

OP(0f)
{
    int tmp;    /* 5-7 BBR0 ZPG */
    RD_ZPG;
    BBR(0);
}
OP(2f)
{
    int tmp;    /* 5-7 BBR2 ZPG */
    RD_ZPG;
    BBR(2);
}
OP(4f)
{
    int tmp;    /* 5-7 BBR4 ZPG */
    RD_ZPG;
    BBR(4);
}
OP(6f)
{
    int tmp;    /* 5-7 BBR6 ZPG */
    RD_ZPG;
    BBR(6);
}
OP(8f)
{
    int tmp;    /* 5-7 BBS0 ZPG */
    RD_ZPG;
    BBS(0);
}
OP(af)
{
    int tmp;    /* 5-7 BBS2 ZPG */
    RD_ZPG;
    BBS(2);
}
OP(cf)
{
    int tmp;    /* 5-7 BBS4 ZPG */
    RD_ZPG;
    BBS(4);
}
OP(ef)
{
    int tmp;    /* 5-7 BBS6 ZPG */
    RD_ZPG;
    BBS(6);
}

OP(1f)
{
    int tmp;    /* 5-7 BBR1 ZPG */
    RD_ZPG;
    BBR(1);
}
OP(3f)
{
    int tmp;    /* 5-7 BBR3 ZPG */
    RD_ZPG;
    BBR(3);
}
OP(5f)
{
    int tmp;    /* 5-7 BBR5 ZPG */
    RD_ZPG;
    BBR(5);
}
OP(7f)
{
    int tmp;    /* 5-7 BBR7 ZPG */
    RD_ZPG;
    BBR(7);
}
OP(9f)
{
    int tmp;    /* 5-7 BBS1 ZPG */
    RD_ZPG;
    BBS(1);
}
OP(bf)
{
    int tmp;    /* 5-7 BBS3 ZPG */
    RD_ZPG;
    BBS(3);
}
OP(df)
{
    int tmp;    /* 5-7 BBS5 ZPG */
    RD_ZPG;
    BBS(5);
}
OP(ff)
{
    int tmp;    /* 5-7 BBS7 ZPG */
    RD_ZPG;
    BBS(7);
}

static void (*const insn65c02[0x100])(void) =
{
    m65c02_00, m65c02_01, m65c02_02, m65c02_03, m65c02_04, m65c02_05, m65c02_06, m65c02_07,
    m65c02_08, m65c02_09, m65c02_0a, m65c02_0b, m65c02_0c, m65c02_0d, m65c02_0e, m65c02_0f,
    m65c02_10, m65c02_11, m65c02_12, m65c02_13, m65c02_14, m65c02_15, m65c02_16, m65c02_17,
    m65c02_18, m65c02_19, m65c02_1a, m65c02_1b, m65c02_1c, m65c02_1d, m65c02_1e, m65c02_1f,
    m65c02_20, m65c02_21, m65c02_22, m65c02_23, m65c02_24, m65c02_25, m65c02_26, m65c02_27,
    m65c02_28, m65c02_29, m65c02_2a, m65c02_2b, m65c02_2c, m65c02_2d, m65c02_2e, m65c02_2f,
    m65c02_30, m65c02_31, m65c02_32, m65c02_33, m65c02_34, m65c02_35, m65c02_36, m65c02_37,
    m65c02_38, m65c02_39, m65c02_3a, m65c02_3b, m65c02_3c, m65c02_3d, m65c02_3e, m65c02_3f,
    m65c02_40, m65c02_41, m65c02_42, m65c02_43, m65c02_44, m65c02_45, m65c02_46, m65c02_47,
    m65c02_48, m65c02_49, m65c02_4a, m65c02_4b, m65c02_4c, m65c02_4d, m65c02_4e, m65c02_4f,
    m65c02_50, m65c02_51, m65c02_52, m65c02_53, m65c02_54, m65c02_55, m65c02_56, m65c02_57,
    m65c02_58, m65c02_59, m65c02_5a, m65c02_5b, m65c02_5c, m65c02_5d, m65c02_5e, m65c02_5f,
    m65c02_60, m65c02_61, m65c02_62, m65c02_63, m65c02_64, m65c02_65, m65c02_66, m65c02_67,
    m65c02_68, m65c02_69, m65c02_6a, m65c02_6b, m65c02_6c, m65c02_6d, m65c02_6e, m65c02_6f,
    m65c02_70, m65c02_71, m65c02_72, m65c02_73, m65c02_74, m65c02_75, m65c02_76, m65c02_77,
    m65c02_78, m65c02_79, m65c02_7a, m65c02_7b, m65c02_7c, m65c02_7d, m65c02_7e, m65c02_7f,
    m65c02_80, m65c02_81, m65c02_82, m65c02_83, m65c02_84, m65c02_85, m65c02_86, m65c02_87,
    m65c02_88, m65c02_89, m65c02_8a, m65c02_8b, m65c02_8c, m65c02_8d, m65c02_8e, m65c02_8f,
    m65c02_90, m65c02_91, m65c02_92, m65c02_93, m65c02_94, m65c02_95, m65c02_96, m65c02_97,
    m65c02_98, m65c02_99, m65c02_9a, m65c02_9b, m65c02_9c, m65c02_9d, m65c02_9e, m65c02_9f,
    m65c02_a0, m65c02_a1, m65c02_a2, m65c02_a3, m65c02_a4, m65c02_a5, m65c02_a6, m65c02_a7,
    m65c02_a8, m65c02_a9, m65c02_aa, m65c02_ab, m65c02_ac, m65c02_ad, m65c02_ae, m65c02_af,
    m65c02_b0, m65c02_b1, m65c02_b2, m65c02_b3, m65c02_b4, m65c02_b5, m65c02_b6, m65c02_b7,
    m65c02_b8, m65c02_b9, m65c02_ba, m65c02_bb, m65c02_bc, m65c02_bd, m65c02_be, m65c02_bf,
    m65c02_c0, m65c02_c1, m65c02_c2, m65c02_c3, m65c02_c4, m65c02_c5, m65c02_c6, m65c02_c7,
    m65c02_c8, m65c02_c9, m65c02_ca, m65c02_cb, m65c02_cc, m65c02_cd, m65c02_ce, m65c02_cf,
    m65c02_d0, m65c02_d1, m65c02_d2, m65c02_d3, m65c02_d4, m65c02_d5, m65c02_d6, m65c02_d7,
    m65c02_d8, m65c02_d9, m65c02_da, m65c02_db, m65c02_dc, m65c02_dd, m65c02_de, m65c02_df,
    m65c02_e0, m65c02_e1, m65c02_e2, m65c02_e3, m65c02_e4, m65c02_e5, m65c02_e6, m65c02_e7,
    m65c02_e8, m65c02_e9, m65c02_ea, m65c02_eb, m65c02_ec, m65c02_ed, m65c02_ee, m65c02_ef,
    m65c02_f0, m65c02_f1, m65c02_f2, m65c02_f3, m65c02_f4, m65c02_f5, m65c02_f6, m65c02_f7,
    m65c02_f8, m65c02_f9, m65c02_fa, m65c02_fb, m65c02_fc, m65c02_fd, m65c02_fe, m65c02_ff
};

#ifdef WDC65C02
OP(cb_wdc)
{
    RD_DUM;    /* 3 WAI, TODO: Implement HALT mode */
    RD_DUM;
}
OP(db_wdc)
{
    RD_DUM;    /* 3 STP, TODO: Implement STP mode */
    RD_DUM;
}
OP(1e_wdc)
{
    int tmp;    /* 6 ASL ABX page penalty */
    RD_ABX_P;
    RD_EA;
    ASL;
    WB_EA;
}
OP(3e_wdc)
{
    int tmp;    /* 6 ROL ABX page penalty */
    RD_ABX_P;
    RD_EA;
    ROL;
    WB_EA;
}
OP(5e_wdc)
{
    int tmp;    /* 6 LSR ABX page penalty */
    RD_ABX_P;
    RD_EA;
    LSR;
    WB_EA;
}
OP(7e_wdc)
{
    int tmp;    /* 6 ROR ABX page penalty */
    RD_ABX_P;
    RD_EA;
    ROR;
    WB_EA;
}
OP(de_wdc)
{
    int tmp;    /* 6 DEC ABX page penalty */
    RD_ABX_P;
    RD_EA;
    DEC;
    WB_EA;
}
OP(fe_wdc)
{
    int tmp;    /* 6 INC ABX page penalty */
    RD_ABX_P;
    RD_EA;
    INC;
    WB_EA;
}

static void (*const insnwdc65c02[0x100])(m6502_Regs *cpustate) =
{
    m65c02_00, m65c02_01, m65c02_02, m65c02_03, m65c02_04, m65c02_05, m65c02_06, m65c02_07,
    m65c02_08, m65c02_09, m65c02_0a, m65c02_0b, m65c02_0c, m65c02_0d, m65c02_0e, m65c02_0f,
    m65c02_10, m65c02_11, m65c02_12, m65c02_13, m65c02_14, m65c02_15, m65c02_16, m65c02_17,
    m65c02_18, m65c02_19, m65c02_1a, m65c02_1b, m65c02_1c, m65c02_1d, m65c02_1e_wdc, m65c02_1f,
    m65c02_20, m65c02_21, m65c02_22, m65c02_23, m65c02_24, m65c02_25, m65c02_26, m65c02_27,
    m65c02_28, m65c02_29, m65c02_2a, m65c02_2b, m65c02_2c, m65c02_2d, m65c02_2e, m65c02_2f,
    m65c02_30, m65c02_31, m65c02_32, m65c02_33, m65c02_34, m65c02_35, m65c02_36, m65c02_37,
    m65c02_38, m65c02_39, m65c02_3a, m65c02_3b, m65c02_3c, m65c02_3d, m65c02_3e_wdc, m65c02_3f,
    m65c02_40, m65c02_41, m65c02_42, m65c02_43, m65c02_44, m65c02_45, m65c02_46, m65c02_47,
    m65c02_48, m65c02_49, m65c02_4a, m65c02_4b, m65c02_4c, m65c02_4d, m65c02_4e, m65c02_4f,
    m65c02_50, m65c02_51, m65c02_52, m65c02_53, m65c02_54, m65c02_55, m65c02_56, m65c02_57,
    m65c02_58, m65c02_59, m65c02_5a, m65c02_5b, m65c02_5c, m65c02_5d, m65c02_5e_wdc, m65c02_5f,
    m65c02_60, m65c02_61, m65c02_62, m65c02_63, m65c02_64, m65c02_65, m65c02_66, m65c02_67,
    m65c02_68, m65c02_69, m65c02_6a, m65c02_6b, m65c02_6c, m65c02_6d, m65c02_6e, m65c02_6f,
    m65c02_70, m65c02_71, m65c02_72, m65c02_73, m65c02_74, m65c02_75, m65c02_76, m65c02_77,
    m65c02_78, m65c02_79, m65c02_7a, m65c02_7b, m65c02_7c, m65c02_7d, m65c02_7e_wdc, m65c02_7f,
    m65c02_80, m65c02_81, m65c02_82, m65c02_83, m65c02_84, m65c02_85, m65c02_86, m65c02_87,
    m65c02_88, m65c02_89, m65c02_8a, m65c02_8b, m65c02_8c, m65c02_8d, m65c02_8e, m65c02_8f,
    m65c02_90, m65c02_91, m65c02_92, m65c02_93, m65c02_94, m65c02_95, m65c02_96, m65c02_97,
    m65c02_98, m65c02_99, m65c02_9a, m65c02_9b, m65c02_9c, m65c02_9d, m65c02_9e, m65c02_9f,
    m65c02_a0, m65c02_a1, m65c02_a2, m65c02_a3, m65c02_a4, m65c02_a5, m65c02_a6, m65c02_a7,
    m65c02_a8, m65c02_a9, m65c02_aa, m65c02_ab, m65c02_ac, m65c02_ad, m65c02_ae, m65c02_af,
    m65c02_b0, m65c02_b1, m65c02_b2, m65c02_b3, m65c02_b4, m65c02_b5, m65c02_b6, m65c02_b7,
    m65c02_b8, m65c02_b9, m65c02_ba, m65c02_bb, m65c02_bc, m65c02_bd, m65c02_be, m65c02_bf,
    m65c02_c0, m65c02_c1, m65c02_c2, m65c02_c3, m65c02_c4, m65c02_c5, m65c02_c6, m65c02_c7,
    m65c02_c8, m65c02_c9, m65c02_ca, m65c02_cb_wdc, m65c02_cc, m65c02_cd, m65c02_ce, m65c02_cf,
    m65c02_d0, m65c02_d1, m65c02_d2, m65c02_d3, m65c02_d4, m65c02_d5, m65c02_d6, m65c02_d7,
    m65c02_d8, m65c02_d9, m65c02_da, m65c02_db_wdc, m65c02_dc, m65c02_dd, m65c02_de_wdc, m65c02_df,
    m65c02_e0, m65c02_e1, m65c02_e2, m65c02_e3, m65c02_e4, m65c02_e5, m65c02_e6, m65c02_e7,
    m65c02_e8, m65c02_e9, m65c02_ea, m65c02_eb, m65c02_ec, m65c02_ed, m65c02_ee, m65c02_ef,
    m65c02_f0, m65c02_f1, m65c02_f2, m65c02_f3, m65c02_f4, m65c02_f5, m65c02_f6, m65c02_f7,
    m65c02_f8, m65c02_f9, m65c02_fa, m65c02_fb, m65c02_fc, m65c02_fd, m65c02_fe_wdc, m65c02_ff
};
#endif


