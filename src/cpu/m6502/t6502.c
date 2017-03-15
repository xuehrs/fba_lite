/*****************************************************************************
 *
 *   tbl6502.c
 *   6502 opcode functions and function pointer table
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
 *   2003-05-26  Fixed PHP, PLP, PHA, PLA cycle counts. [SJ]
 *   2004-04-30  Fixed STX (abs) cycle count. [SJ]
 *
 *****************************************************************************/

#undef	OP
#define OP(nn) M6502_INLINE void m6502_##nn(void)

/*****************************************************************************
 *****************************************************************************
 *
 *   plain vanilla 6502 opcodes
 *
 *****************************************************************************
 * op    temp     cycles             rdmem   opc  wrmem   ********************/

OP(00)
{
    BRK;                    /* 7 BRK */
}
OP(20)
{
    JSR;                    /* 6 JSR */
}
OP(40)
{
    RTI;                    /* 6 RTI */
}
OP(60)
{
    RTS;                    /* 6 RTS */
}
OP(80)
{
    int tmp;    /* 2 NOP IMM */
    RD_IMM;
    NOP;
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
    BPL;                                     /* 2-4 BPL REL */
}
OP(30)
{
    BMI;                                     /* 2-4 BMI REL */
}
OP(50)
{
    BVC;                                     /* 2-4 BVC REL */
}
OP(70)
{
    BVS;                                     /* 2-4 BVS REL */
}
OP(90)
{
    BCC;                                     /* 2-4 BCC REL */
}
OP(b0)
{
    BCS;                                     /* 2-4 BCS REL */
}
OP(d0)
{
    BNE;                                     /* 2-4 BNE REL */
}
OP(f0)
{
    BEQ;                                     /* 2-4 BEQ REL */
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
    int tmp;    /* 6 ADC IDX */
    RD_IDX;
    ADC;
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
    int tmp;    /* 6 SBC IDX */
    RD_IDX;
    SBC;
}

OP(11)
{
    int tmp;    /* 5 ORA IDY page penalty */
    RD_IDY_P;
    ORA;
}
OP(31)
{
    int tmp;    /* 5 AND IDY page penalty */
    RD_IDY_P;
    AND;
}
OP(51)
{
    int tmp;    /* 5 EOR IDY page penalty */
    RD_IDY_P;
    EOR;
}
OP(71)
{
    int tmp;    /* 5 ADC IDY page penalty */
    RD_IDY_P;
    ADC;
}
OP(91)
{
    int tmp;    /* 6 STA IDY */
    STA;
    WR_IDY_NP;
}
OP(b1)
{
    int tmp;    /* 5 LDA IDY page penalty */
    RD_IDY_P;
    LDA;
}
OP(d1)
{
    int tmp;    /* 5 CMP IDY page penalty */
    RD_IDY_P;
    CMP;
}
OP(f1)
{
    int tmp;    /* 5 SBC IDY page penalty */
    RD_IDY_P;
    SBC;
}

OP(02)
{
    KIL;                    /* 1 KIL */
}
OP(22)
{
    KIL;                    /* 1 KIL */
}
OP(42)
{
    KIL;                    /* 1 KIL */
}
OP(62)
{
    KIL;                    /* 1 KIL */
}
OP(82)
{
    int tmp;    /* 2 NOP IMM */
    RD_IMM;
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
    int tmp;    /* 2 NOP IMM */
    RD_IMM;
    NOP;
}
OP(e2)
{
    int tmp;    /* 2 NOP IMM */
    RD_IMM;
    NOP;
}

OP(12)
{
    KIL;                                     /* 1 KIL */
}
OP(32)
{
    KIL;                                     /* 1 KIL */
}
OP(52)
{
    KIL;                                     /* 1 KIL */
}
OP(72)
{
    KIL;                                     /* 1 KIL */
}
OP(92)
{
    KIL;                                     /* 1 KIL */
}
OP(b2)
{
    KIL;                                     /* 1 KIL */
}
OP(d2)
{
    KIL;                                     /* 1 KIL */
}
OP(f2)
{
    KIL;                                     /* 1 KIL */
}

OP(03)
{
    int tmp;    /* 7 SLO IDX */
    RD_IDX;
    WB_EA;
    SLO;
    WB_EA;
}
OP(23)
{
    int tmp;    /* 7 RLA IDX */
    RD_IDX;
    WB_EA;
    RLA;
    WB_EA;
}
OP(43)
{
    int tmp;    /* 7 SRE IDX */
    RD_IDX;
    WB_EA;
    SRE;
    WB_EA;
}
OP(63)
{
    int tmp;    /* 7 RRA IDX */
    RD_IDX;
    WB_EA;
    RRA;
    WB_EA;
}
OP(83)
{
    int tmp;                   /* 6 SAX IDX */
    SAX;
    WR_IDX;
}
OP(a3)
{
    int tmp;    /* 6 LAX IDX */
    RD_IDX;
    LAX;
}
OP(c3)
{
    int tmp;    /* 7 DCP IDX */
    RD_IDX;
    WB_EA;
    DCP;
    WB_EA;
}
OP(e3)
{
    int tmp;    /* 7 ISB IDX */
    RD_IDX;
    WB_EA;
    ISB;
    WB_EA;
}

OP(13)
{
    int tmp;    /* 7 SLO IDY */
    RD_IDY_NP;
    WB_EA;
    SLO;
    WB_EA;
}
OP(33)
{
    int tmp;    /* 7 RLA IDY */
    RD_IDY_NP;
    WB_EA;
    RLA;
    WB_EA;
}
OP(53)
{
    int tmp;    /* 7 SRE IDY */
    RD_IDY_NP;
    WB_EA;
    SRE;
    WB_EA;
}
OP(73)
{
    int tmp;    /* 7 RRA IDY */
    RD_IDY_NP;
    WB_EA;
    RRA;
    WB_EA;
}
OP(93)
{
    int tmp;    /* 5 SAH IDY */
    EA_IDY_NP;
    SAH;
    WB_EA;
}
OP(b3)
{
    int tmp;    /* 5 LAX IDY page penalty */
    RD_IDY_P;
    LAX;
}
OP(d3)
{
    int tmp;    /* 7 DCP IDY */
    RD_IDY_NP;
    WB_EA;
    DCP;
    WB_EA;
}
OP(f3)
{
    int tmp;    /* 7 ISB IDY */
    RD_IDY_NP;
    WB_EA;
    ISB;
    WB_EA;
}

OP(04)
{
    int tmp;    /* 3 NOP ZPG */
    RD_ZPG;
    NOP;
}
OP(24)
{
    int tmp;    /* 3 BIT ZPG */
    RD_ZPG;
    BIT;
}
OP(44)
{
    int tmp;    /* 3 NOP ZPG */
    RD_ZPG;
    NOP;
}
OP(64)
{
    int tmp;    /* 3 NOP ZPG */
    RD_ZPG;
    NOP;
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
    int tmp;    /* 4 NOP ZPX */
    RD_ZPX;
    NOP;
}
OP(34)
{
    int tmp;    /* 4 NOP ZPX */
    RD_ZPX;
    NOP;
}
OP(54)
{
    int tmp;    /* 4 NOP ZPX */
    RD_ZPX;
    NOP;
}
OP(74)
{
    int tmp;    /* 4 NOP ZPX */
    RD_ZPX;
    NOP;
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
    int tmp;    /* 4 NOP ZPX */
    RD_ZPX;
    NOP;
}
OP(f4)
{
    int tmp;    /* 4 NOP ZPX */
    RD_ZPX;
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
    int tmp;    /* 3 ADC ZPG */
    RD_ZPG;
    ADC;
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
    int tmp;    /* 3 SBC ZPG */
    RD_ZPG;
    SBC;
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
    int tmp;    /* 4 ADC ZPX */
    RD_ZPX;
    ADC;
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
    int tmp;    /* 4 SBC ZPX */
    RD_ZPX;
    SBC;
}

OP(06)
{
    int tmp;    /* 5 ASL ZPG */
    RD_ZPG;
    WB_EA;
    ASL;
    WB_EA;
}
OP(26)
{
    int tmp;    /* 5 ROL ZPG */
    RD_ZPG;
    WB_EA;
    ROL;
    WB_EA;
}
OP(46)
{
    int tmp;    /* 5 LSR ZPG */
    RD_ZPG;
    WB_EA;
    LSR;
    WB_EA;
}
OP(66)
{
    int tmp;    /* 5 ROR ZPG */
    RD_ZPG;
    WB_EA;
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
    WB_EA;
    DEC;
    WB_EA;
}
OP(e6)
{
    int tmp;    /* 5 INC ZPG */
    RD_ZPG;
    WB_EA;
    INC;
    WB_EA;
}

OP(16)
{
    int tmp;    /* 6 ASL ZPX */
    RD_ZPX;
    WB_EA;
    ASL;
    WB_EA;
}
OP(36)
{
    int tmp;    /* 6 ROL ZPX */
    RD_ZPX;
    WB_EA;
    ROL;
    WB_EA;
}
OP(56)
{
    int tmp;    /* 6 LSR ZPX */
    RD_ZPX;
    WB_EA;
    LSR;
    WB_EA;
}
OP(76)
{
    int tmp;    /* 6 ROR ZPX */
    RD_ZPX;
    WB_EA;
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
    WB_EA;
    DEC;
    WB_EA;
}
OP(f6)
{
    int tmp;    /* 6 INC ZPX */
    RD_ZPX;
    WB_EA;
    INC;
    WB_EA;
}

OP(07)
{
    int tmp;    /* 5 SLO ZPG */
    RD_ZPG;
    WB_EA;
    SLO;
    WB_EA;
}
OP(27)
{
    int tmp;    /* 5 RLA ZPG */
    RD_ZPG;
    WB_EA;
    RLA;
    WB_EA;
}
OP(47)
{
    int tmp;    /* 5 SRE ZPG */
    RD_ZPG;
    WB_EA;
    SRE;
    WB_EA;
}
OP(67)
{
    int tmp;    /* 5 RRA ZPG */
    RD_ZPG;
    WB_EA;
    RRA;
    WB_EA;
}
OP(87)
{
    int tmp;    /* 3 SAX ZPG */
    SAX;
    WR_ZPG;
}
OP(a7)
{
    int tmp;    /* 3 LAX ZPG */
    RD_ZPG;
    LAX;
}
OP(c7)
{
    int tmp;    /* 5 DCP ZPG */
    RD_ZPG;
    WB_EA;
    DCP;
    WB_EA;
}
OP(e7)
{
    int tmp;    /* 5 ISB ZPG */
    RD_ZPG;
    WB_EA;
    ISB;
    WB_EA;
}

OP(17)
{
    int tmp;    /* 6 SLO ZPX */
    RD_ZPX;
    WB_EA;
    SLO;
    WB_EA;
}
OP(37)
{
    int tmp;    /* 6 RLA ZPX */
    RD_ZPX;
    WB_EA;
    RLA;
    WB_EA;
}
OP(57)
{
    int tmp;    /* 6 SRE ZPX */
    RD_ZPX;
    WB_EA;
    SRE;
    WB_EA;
}
OP(77)
{
    int tmp;    /* 6 RRA ZPX */
    RD_ZPX;
    WB_EA;
    RRA;
    WB_EA;
}
OP(97)
{
    int tmp;    /* 4 SAX ZPY */
    SAX;
    WR_ZPY;
}
OP(b7)
{
    int tmp;    /* 4 LAX ZPY */
    RD_ZPY;
    LAX;
}
OP(d7)
{
    int tmp;    /* 6 DCP ZPX */
    RD_ZPX;
    WB_EA;
    DCP;
    WB_EA;
}
OP(f7)
{
    int tmp;    /* 6 ISB ZPX */
    RD_ZPX;
    WB_EA;
    ISB;
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
    int tmp;    /* 2 ADC IMM */
    RD_IMM;
    ADC;
}
OP(89)
{
    int tmp;    /* 2 NOP IMM */
    RD_IMM;
    NOP;
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
    int tmp;    /* 2 SBC IMM */
    RD_IMM;
    SBC;
}

OP(19)
{
    int tmp;    /* 4 ORA ABY page penalty */
    RD_ABY_P;
    ORA;
}
OP(39)
{
    int tmp;    /* 4 AND ABY page penalty */
    RD_ABY_P;
    AND;
}
OP(59)
{
    int tmp;    /* 4 EOR ABY page penalty */
    RD_ABY_P;
    EOR;
}
OP(79)
{
    int tmp;    /* 4 ADC ABY page penalty */
    RD_ABY_P;
    ADC;
}
OP(99)
{
    int tmp;    /* 5 STA ABY */
    STA;
    WR_ABY_NP;
}
OP(b9)
{
    int tmp;    /* 4 LDA ABY page penalty */
    RD_ABY_P;
    LDA;
}
OP(d9)
{
    int tmp;    /* 4 CMP ABY page penalty */
    RD_ABY_P;
    CMP;
}
OP(f9)
{
    int tmp;    /* 4 SBC ABY page penalty */
    RD_ABY_P;
    SBC;
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
    RD_DUM;    /* 2 NOP */
    NOP;
}
OP(3a)
{
    RD_DUM;    /* 2 NOP */
    NOP;
}
OP(5a)
{
    RD_DUM;    /* 2 NOP */
    NOP;
}
OP(7a)
{
    RD_DUM;    /* 2 NOP */
    NOP;
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
    RD_DUM;    /* 2 NOP */
    NOP;
}
OP(fa)
{
    RD_DUM;    /* 2 NOP */
    NOP;
}

OP(0b)
{
    int tmp;    /* 2 ANC IMM */
    RD_IMM;
    ANC;
}
OP(2b)
{
    int tmp;    /* 2 ANC IMM */
    RD_IMM;
    ANC;
}
OP(4b)
{
    int tmp;    /* 2 ASR IMM */
    RD_IMM;
    ASR;
    WB_ACC;
}
OP(6b)
{
    int tmp;    /* 2 ARR IMM */
    RD_IMM;
    ARR;
    WB_ACC;
}
OP(8b)
{
    int tmp;    /* 2 AXA IMM */
    RD_IMM;
    AXA;
}
OP(ab)
{
    int tmp;    /* 2 OAL IMM */
    RD_IMM;
    OAL;
}
OP(cb)
{
    int tmp;    /* 2 ASX IMM */
    RD_IMM;
    ASX;
}
OP(eb)
{
    int tmp;    /* 2 SBC IMM */
    RD_IMM;
    SBC;
}

OP(1b)
{
    int tmp;    /* 7 SLO ABY */
    RD_ABY_NP;
    WB_EA;
    SLO;
    WB_EA;
}
OP(3b)
{
    int tmp;    /* 7 RLA ABY */
    RD_ABY_NP;
    WB_EA;
    RLA;
    WB_EA;
}
OP(5b)
{
    int tmp;    /* 7 SRE ABY */
    RD_ABY_NP;
    WB_EA;
    SRE;
    WB_EA;
}
OP(7b)
{
    int tmp;    /* 7 RRA ABY */
    RD_ABY_NP;
    WB_EA;
    RRA;
    WB_EA;
}
OP(9b)
{
    int tmp;    /* 5 SSH ABY */
    EA_ABY_NP;
    SSH;
    WB_EA;
}
OP(bb)
{
    int tmp;    /* 4 AST ABY page penalty */
    RD_ABY_P;
    AST;
}
OP(db)
{
    int tmp;    /* 7 DCP ABY */
    RD_ABY_NP;
    WB_EA;
    DCP;
    WB_EA;
}
OP(fb)
{
    int tmp;    /* 7 ISB ABY */
    RD_ABY_NP;
    WB_EA;
    ISB;
    WB_EA;
}

OP(0c)
{
    int tmp;    /* 4 NOP ABS */
    RD_ABS;
    NOP;
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
    int tmp;    /* 5 JMP IND */
    EA_IND;
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
    int tmp;    /* 4 NOP ABX page penalty */
    RD_ABX_P;
    NOP;
}
OP(3c)
{
    int tmp;    /* 4 NOP ABX page penalty */
    RD_ABX_P;
    NOP;
}
OP(5c)
{
    int tmp;    /* 4 NOP ABX page penalty */
    RD_ABX_P;
    NOP;
}
OP(7c)
{
    int tmp;    /* 4 NOP ABX page penalty */
    RD_ABX_P;
    NOP;
}
OP(9c)
{
    int tmp;    /* 5 SYH ABX */
    EA_ABX_NP;
    SYH;
    WB_EA;
}
OP(bc)
{
    int tmp;    /* 4 LDY ABX page penalty */
    RD_ABX_P;
    LDY;
}
OP(dc)
{
    int tmp;    /* 4 NOP ABX page penalty */
    RD_ABX_P;
    NOP;
}
OP(fc)
{
    int tmp;    /* 4 NOP ABX page penalty */
    RD_ABX_P;
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
    int tmp;    /* 4 ADC ABS */
    RD_ABS;
    ADC;
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
    int tmp;    /* 4 SBC ABS */
    RD_ABS;
    SBC;
}

OP(1d)
{
    int tmp;    /* 4 ORA ABX page penalty */
    RD_ABX_P;
    ORA;
}
OP(3d)
{
    int tmp;    /* 4 AND ABX page penalty */
    RD_ABX_P;
    AND;
}
OP(5d)
{
    int tmp;    /* 4 EOR ABX page penalty */
    RD_ABX_P;
    EOR;
}
OP(7d)
{
    int tmp;    /* 4 ADC ABX page penalty */
    RD_ABX_P;
    ADC;
}
OP(9d)
{
    int tmp;    /* 5 STA ABX */
    STA;
    WR_ABX_NP;
}
OP(bd)
{
    int tmp;    /* 4 LDA ABX page penalty */
    RD_ABX_P;
    LDA;
}
OP(dd)
{
    int tmp;    /* 4 CMP ABX page penalty */
    RD_ABX_P;
    CMP;
}
OP(fd)
{
    int tmp;    /* 4 SBC ABX page penalty */
    RD_ABX_P;
    SBC;
}

OP(0e)
{
    int tmp;    /* 6 ASL ABS */
    RD_ABS;
    WB_EA;
    ASL;
    WB_EA;
}
OP(2e)
{
    int tmp;    /* 6 ROL ABS */
    RD_ABS;
    WB_EA;
    ROL;
    WB_EA;
}
OP(4e)
{
    int tmp;    /* 6 LSR ABS */
    RD_ABS;
    WB_EA;
    LSR;
    WB_EA;
}
OP(6e)
{
    int tmp;    /* 6 ROR ABS */
    RD_ABS;
    WB_EA;
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
    WB_EA;
    DEC;
    WB_EA;
}
OP(ee)
{
    int tmp;    /* 6 INC ABS */
    RD_ABS;
    WB_EA;
    INC;
    WB_EA;
}

OP(1e)
{
    int tmp;    /* 7 ASL ABX */
    RD_ABX_NP;
    WB_EA;
    ASL;
    WB_EA;
}
OP(3e)
{
    int tmp;    /* 7 ROL ABX */
    RD_ABX_NP;
    WB_EA;
    ROL;
    WB_EA;
}
OP(5e)
{
    int tmp;    /* 7 LSR ABX */
    RD_ABX_NP;
    WB_EA;
    LSR;
    WB_EA;
}
OP(7e)
{
    int tmp;    /* 7 ROR ABX */
    RD_ABX_NP;
    WB_EA;
    ROR;
    WB_EA;
}
OP(9e)
{
    int tmp;    /* 5 SXH ABY */
    EA_ABY_NP;
    SXH;
    WB_EA;
}
OP(be)
{
    int tmp;    /* 4 LDX ABY page penalty */
    RD_ABY_P;
    LDX;
}
OP(de)
{
    int tmp;    /* 7 DEC ABX */
    RD_ABX_NP;
    WB_EA;
    DEC;
    WB_EA;
}
OP(fe)
{
    int tmp;    /* 7 INC ABX */
    RD_ABX_NP;
    WB_EA;
    INC;
    WB_EA;
}

OP(0f)
{
    int tmp;    /* 6 SLO ABS */
    RD_ABS;
    WB_EA;
    SLO;
    WB_EA;
}
OP(2f)
{
    int tmp;    /* 6 RLA ABS */
    RD_ABS;
    WB_EA;
    RLA;
    WB_EA;
}
OP(4f)
{
    int tmp;    /* 6 SRE ABS */
    RD_ABS;
    WB_EA;
    SRE;
    WB_EA;
}
OP(6f)
{
    int tmp;    /* 6 RRA ABS */
    RD_ABS;
    WB_EA;
    RRA;
    WB_EA;
}
OP(8f)
{
    int tmp;    /* 4 SAX ABS */
    SAX;
    WR_ABS;
}
OP(af)
{
    int tmp;    /* 4 LAX ABS */
    RD_ABS;
    LAX;
}
OP(cf)
{
    int tmp;    /* 6 DCP ABS */
    RD_ABS;
    WB_EA;
    DCP;
    WB_EA;
}
OP(ef)
{
    int tmp;    /* 6 ISB ABS */
    RD_ABS;
    WB_EA;
    ISB;
    WB_EA;
}

OP(1f)
{
    int tmp;    /* 7 SLO ABX */
    RD_ABX_NP;
    WB_EA;
    SLO;
    WB_EA;
}
OP(3f)
{
    int tmp;    /* 7 RLA ABX */
    RD_ABX_NP;
    WB_EA;
    RLA;
    WB_EA;
}
OP(5f)
{
    int tmp;    /* 7 SRE ABX */
    RD_ABX_NP;
    WB_EA;
    SRE;
    WB_EA;
}
OP(7f)
{
    int tmp;    /* 7 RRA ABX */
    RD_ABX_NP;
    WB_EA;
    RRA;
    WB_EA;
}
OP(9f)
{
    int tmp;    /* 5 SAH ABY */
    EA_ABY_NP;
    SAH;
    WB_EA;
}
OP(bf)
{
    int tmp;    /* 4 LAX ABY page penalty */
    RD_ABY_P;
    LAX;
}
OP(df)
{
    int tmp;    /* 7 DCP ABX */
    RD_ABX_NP;
    WB_EA;
    DCP;
    WB_EA;
}
OP(ff)
{
    int tmp;    /* 7 ISB ABX */
    RD_ABX_NP;
    WB_EA;
    ISB;
    WB_EA;
}

/* and here's the array of function pointers */

static void (*const insn6502[0x100])(void) =
{
    m6502_00, m6502_01, m6502_02, m6502_03, m6502_04, m6502_05, m6502_06, m6502_07,
    m6502_08, m6502_09, m6502_0a, m6502_0b, m6502_0c, m6502_0d, m6502_0e, m6502_0f,
    m6502_10, m6502_11, m6502_12, m6502_13, m6502_14, m6502_15, m6502_16, m6502_17,
    m6502_18, m6502_19, m6502_1a, m6502_1b, m6502_1c, m6502_1d, m6502_1e, m6502_1f,
    m6502_20, m6502_21, m6502_22, m6502_23, m6502_24, m6502_25, m6502_26, m6502_27,
    m6502_28, m6502_29, m6502_2a, m6502_2b, m6502_2c, m6502_2d, m6502_2e, m6502_2f,
    m6502_30, m6502_31, m6502_32, m6502_33, m6502_34, m6502_35, m6502_36, m6502_37,
    m6502_38, m6502_39, m6502_3a, m6502_3b, m6502_3c, m6502_3d, m6502_3e, m6502_3f,
    m6502_40, m6502_41, m6502_42, m6502_43, m6502_44, m6502_45, m6502_46, m6502_47,
    m6502_48, m6502_49, m6502_4a, m6502_4b, m6502_4c, m6502_4d, m6502_4e, m6502_4f,
    m6502_50, m6502_51, m6502_52, m6502_53, m6502_54, m6502_55, m6502_56, m6502_57,
    m6502_58, m6502_59, m6502_5a, m6502_5b, m6502_5c, m6502_5d, m6502_5e, m6502_5f,
    m6502_60, m6502_61, m6502_62, m6502_63, m6502_64, m6502_65, m6502_66, m6502_67,
    m6502_68, m6502_69, m6502_6a, m6502_6b, m6502_6c, m6502_6d, m6502_6e, m6502_6f,
    m6502_70, m6502_71, m6502_72, m6502_73, m6502_74, m6502_75, m6502_76, m6502_77,
    m6502_78, m6502_79, m6502_7a, m6502_7b, m6502_7c, m6502_7d, m6502_7e, m6502_7f,
    m6502_80, m6502_81, m6502_82, m6502_83, m6502_84, m6502_85, m6502_86, m6502_87,
    m6502_88, m6502_89, m6502_8a, m6502_8b, m6502_8c, m6502_8d, m6502_8e, m6502_8f,
    m6502_90, m6502_91, m6502_92, m6502_93, m6502_94, m6502_95, m6502_96, m6502_97,
    m6502_98, m6502_99, m6502_9a, m6502_9b, m6502_9c, m6502_9d, m6502_9e, m6502_9f,
    m6502_a0, m6502_a1, m6502_a2, m6502_a3, m6502_a4, m6502_a5, m6502_a6, m6502_a7,
    m6502_a8, m6502_a9, m6502_aa, m6502_ab, m6502_ac, m6502_ad, m6502_ae, m6502_af,
    m6502_b0, m6502_b1, m6502_b2, m6502_b3, m6502_b4, m6502_b5, m6502_b6, m6502_b7,
    m6502_b8, m6502_b9, m6502_ba, m6502_bb, m6502_bc, m6502_bd, m6502_be, m6502_bf,
    m6502_c0, m6502_c1, m6502_c2, m6502_c3, m6502_c4, m6502_c5, m6502_c6, m6502_c7,
    m6502_c8, m6502_c9, m6502_ca, m6502_cb, m6502_cc, m6502_cd, m6502_ce, m6502_cf,
    m6502_d0, m6502_d1, m6502_d2, m6502_d3, m6502_d4, m6502_d5, m6502_d6, m6502_d7,
    m6502_d8, m6502_d9, m6502_da, m6502_db, m6502_dc, m6502_dd, m6502_de, m6502_df,
    m6502_e0, m6502_e1, m6502_e2, m6502_e3, m6502_e4, m6502_e5, m6502_e6, m6502_e7,
    m6502_e8, m6502_e9, m6502_ea, m6502_eb, m6502_ec, m6502_ed, m6502_ee, m6502_ef,
    m6502_f0, m6502_f1, m6502_f2, m6502_f3, m6502_f4, m6502_f5, m6502_f6, m6502_f7,
    m6502_f8, m6502_f9, m6502_fa, m6502_fb, m6502_fc, m6502_fd, m6502_fe, m6502_ff
};

