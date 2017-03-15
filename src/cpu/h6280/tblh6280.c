/*****************************************************************************

    tblh6280.c

    Copyright Bryan McPhail, mish@tendril.co.uk

    This source code is based (with permission!) on the 6502 emulator by
    Juergen Buchmueller.  It is released as part of the Mame emulator project.
    Let me know if you intend to use this code in any other project.

******************************************************************************/

#undef	OP
#define OP(nnn) H6280_INLINE void h6280##nnn(void)

/*****************************************************************************
 *****************************************************************************
 *
 *   Hu6280 opcodes
 *
 *****************************************************************************
 * op     temp     cycles             rdmem   opc   wrmem   ******************/
OP(_000)
{
    H6280_CYCLES(8);		     // 8 BRK
    BRK;
}
OP(_020)
{
    H6280_CYCLES(7);    // 7 JSR  ABS
    EA_ABS;
    JSR;
}
OP(_040)
{
    H6280_CYCLES(7);		     // 7 RTI
    RTI;
}
OP(_060)
{
    H6280_CYCLES(7);		     // 7 RTS
    RTS;
}
OP(_080)
{
    int tmp;  				             // 4 BRA  REL
    BRA(1);
}
OP(_0a0)
{
    int tmp;    // 2 LDY  IMM
    H6280_CYCLES(2);
    RD_IMM;
    LDY;
}
OP(_0c0)
{
    int tmp;    // 2 CPY  IMM
    H6280_CYCLES(2);
    RD_IMM;
    CPY;
}
OP(_0e0)
{
    int tmp;    // 2 CPX  IMM
    H6280_CYCLES(2);
    RD_IMM;
    CPX;
}

OP(_010)
{
    int tmp;							     // 2/4 BPL  REL
    BPL;
}
OP(_030)
{
    int tmp;							     // 2/4 BMI  REL
    BMI;
}
OP(_050)
{
    int tmp;							     // 2/4 BVC  REL
    BVC;
}
OP(_070)
{
    int tmp;							     // 2/4 BVS  REL
    BVS;
}
OP(_090)
{
    int tmp;							     // 2/4 BCC  REL
    BCC;
}
OP(_0b0)
{
    int tmp;							     // 2/4 BCS  REL
    BCS;
}
OP(_0d0)
{
    int tmp;							     // 2/4 BNE  REL
    BNE;
}
OP(_0f0)
{
    int tmp;							     // 2/4 BEQ  REL
    BEQ;
}

OP(_001)
{
    int tmp;    // 7 ORA  IDX
    H6280_CYCLES(7);
    RD_IDX;
    ORA;
}
OP(_021)
{
    int tmp;    // 7 AND  IDX
    H6280_CYCLES(7);
    RD_IDX;
    AND;
}
OP(_041)
{
    int tmp;    // 7 EOR  IDX
    H6280_CYCLES(7);
    RD_IDX;
    EOR;
}
OP(_061)
{
    int tmp;    // 7 ADC  IDX
    H6280_CYCLES(7);
    RD_IDX;
    ADC;
}
OP(_081)
{
    int tmp;    // 7 STA  IDX
    H6280_CYCLES(7);
    STA;
    WR_IDX;
}
OP(_0a1)
{
    int tmp;    // 7 LDA  IDX
    H6280_CYCLES(7);
    RD_IDX;
    LDA;
}
OP(_0c1)
{
    int tmp;    // 7 CMP  IDX
    H6280_CYCLES(7);
    RD_IDX;
    CMP;
}
OP(_0e1)
{
    int tmp;    // 7 SBC  IDX
    H6280_CYCLES(7);
    RD_IDX;
    SBC;
}

OP(_011)
{
    int tmp;    // 7 ORA  IDY
    H6280_CYCLES(7);
    RD_IDY;
    ORA;
}
OP(_031)
{
    int tmp;    // 7 AND  IDY
    H6280_CYCLES(7);
    RD_IDY;
    AND;
}
OP(_051)
{
    int tmp;    // 7 EOR  IDY
    H6280_CYCLES(7);
    RD_IDY;
    EOR;
}
OP(_071)
{
    int tmp;    // 7 ADC  AZP
    H6280_CYCLES(7);
    RD_IDY;
    ADC;
}
OP(_091)
{
    int tmp;    // 7 STA  IDY
    H6280_CYCLES(7);
    STA;
    WR_IDY;
}
OP(_0b1)
{
    int tmp;    // 7 LDA  IDY
    H6280_CYCLES(7);
    RD_IDY;
    LDA;
}
OP(_0d1)
{
    int tmp;    // 7 CMP  IDY
    H6280_CYCLES(7);
    RD_IDY;
    CMP;
}
OP(_0f1)
{
    int tmp;    // 7 SBC  IDY
    H6280_CYCLES(7);
    RD_IDY;
    SBC;
}

OP(_002)
{
    int tmp;    // 3 SXY
    H6280_CYCLES(3);
    SXY;
}
OP(_022)
{
    int tmp;    // 3 SAX
    H6280_CYCLES(3);
    SAX;
}
OP(_042)
{
    int tmp;    // 3 SAY
    H6280_CYCLES(3);
    SAY;
}
OP(_062)
{
    H6280_CYCLES(2);		     // 2 CLA
    CLA;
}
OP(_082)
{
    H6280_CYCLES(2);		     // 2 CLX
    CLX;
}
OP(_0a2)
{
    int tmp;    // 2 LDX  IMM
    H6280_CYCLES(2);
    RD_IMM;
    LDX;
}
OP(_0c2)
{
    H6280_CYCLES(2);		     // 2 CLY
    CLY;
}
OP(_0e2)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}

OP(_012)
{
    int tmp;    // 7 ORA  ZPI
    H6280_CYCLES(7);
    RD_ZPI;
    ORA;
}
OP(_032)
{
    int tmp;    // 7 AND  ZPI
    H6280_CYCLES(7);
    RD_ZPI;
    AND;
}
OP(_052)
{
    int tmp;    // 7 EOR  ZPI
    H6280_CYCLES(7);
    RD_ZPI;
    EOR;
}
OP(_072)
{
    int tmp;    // 7 ADC  ZPI
    H6280_CYCLES(7);
    RD_ZPI;
    ADC;
}
OP(_092)
{
    int tmp;    // 7 STA  ZPI
    H6280_CYCLES(7);
    STA;
    WR_ZPI;
}
OP(_0b2)
{
    int tmp;    // 7 LDA  ZPI
    H6280_CYCLES(7);
    RD_ZPI;
    LDA;
}
OP(_0d2)
{
    int tmp;    // 7 CMP  ZPI
    H6280_CYCLES(7);
    RD_ZPI;
    CMP;
}
OP(_0f2)
{
    int tmp;    // 7 SBC  ZPI
    H6280_CYCLES(7);
    RD_ZPI;
    SBC;
}

OP(_003)
{
    int tmp;    // 4 + 1 penalty cycle ST0  IMM
    H6280_CYCLES(5);
    RD_IMM;
    ST0;
}
OP(_023)
{
    int tmp;    // 4 + 1 penalty cycle ST2  IMM
    H6280_CYCLES(5);
    RD_IMM;
    ST2;
}
OP(_043)
{
    int tmp;    // 4 TMA
    H6280_CYCLES(4);
    RD_IMM;
    TMA;
}
OP(_063)
{
    H6280_CYCLES(4);		      // 2 NOP
    NOP;
}
OP(_083)
{
    int tmp, tmp2;    // 7 TST  IMM,ZPG
    H6280_CYCLES(7);
    RD_IMM2;
    RD_ZPG;
    TST;
}
OP(_0a3)
{
    int tmp, tmp2;    // 7 TST  IMM,ZPX
    H6280_CYCLES(7);
    RD_IMM2;
    RD_ZPX;
    TST;
}
OP(_0c3)
{
    int to, from, length;			         // 6*l+17 TDD  XFER
    TDD;
}
OP(_0e3)
{
    int to, from, length, alternate;          // 6*l+17 TIA  XFER
    TIA;
}

OP(_013)
{
    int tmp;    // 4 + 1 penalty cycle ST1
    H6280_CYCLES(5);
    RD_IMM;
    ST1;
}
OP(_033)
{
    H6280_CYCLES(2);		      // 2 NOP
    NOP;
}
OP(_053)
{
    int tmp;    // 5 TAM  IMM
    H6280_CYCLES(5);
    RD_IMM;
    TAM;
}
OP(_073)
{
    int to, from, length;    			     // 6*l+17 TII  XFER
    TII;
}
OP(_093)
{
    int tmp, tmp2;    // 8 TST  IMM,ABS
    H6280_CYCLES(8);
    RD_IMM2;
    RD_ABS;
    TST;
}
OP(_0b3)
{
    int tmp, tmp2;    // 8 TST  IMM,ABX
    H6280_CYCLES(8);
    RD_IMM2;
    RD_ABX;
    TST;
}
OP(_0d3)
{
    int to, from, length;			         // 6*l+17 TIN  XFER
    TIN;
}
OP(_0f3)
{
    int to, from, length, alternate;          // 6*l+17 TAI  XFER
    TAI;
}

OP(_004)
{
    int tmp;    // 6 TSB  ZPG
    H6280_CYCLES(6);
    RD_ZPG;
    TSB;
    WB_EAZ;
}
OP(_024)
{
    int tmp;    // 4 BIT  ZPG
    H6280_CYCLES(4);
    RD_ZPG;
    BIT;
}
OP(_044)
{
    int tmp;							     // 8 BSR  REL
    BSR;
}
OP(_064)
{
    int tmp;    // 4 STZ  ZPG
    H6280_CYCLES(4);
    STZ;
    WR_ZPG;
}
OP(_084)
{
    int tmp;    // 4 STY  ZPG
    H6280_CYCLES(4);
    STY;
    WR_ZPG;
}
OP(_0a4)
{
    int tmp;    // 4 LDY  ZPG
    H6280_CYCLES(4);
    RD_ZPG;
    LDY;
}
OP(_0c4)
{
    int tmp;    // 4 CPY  ZPG
    H6280_CYCLES(4);
    RD_ZPG;
    CPY;
}
OP(_0e4)
{
    int tmp;    // 4 CPX  ZPG
    H6280_CYCLES(4);
    RD_ZPG;
    CPX;
}

OP(_014)
{
    int tmp;    // 6 TRB  ZPG
    H6280_CYCLES(6);
    RD_ZPG;
    TRB;
    WB_EAZ;
}
OP(_034)
{
    int tmp;    // 4 BIT  ZPX
    H6280_CYCLES(4);
    RD_ZPX;
    BIT;
}
OP(_054)
{
    H6280_CYCLES(3);			   // 3 CSL
    CSL;
}
OP(_074)
{
    int tmp;    // 4 STZ  ZPX
    H6280_CYCLES(4);
    STZ;
    WR_ZPX;
}
OP(_094)
{
    int tmp;    // 4 STY  ZPX
    H6280_CYCLES(4);
    STY;
    WR_ZPX;
}
OP(_0b4)
{
    int tmp;    // 4 LDY  ZPX
    H6280_CYCLES(4);
    RD_ZPX;
    LDY;
}
OP(_0d4)
{
    H6280_CYCLES(3);			   // 3 CSH
    CSH;
}
OP(_0f4)
{
    H6280_CYCLES(2);		     // 2 SET
    SET;
}

OP(_005)
{
    int tmp;    // 4 ORA  ZPG
    H6280_CYCLES(4);
    RD_ZPG;
    ORA;
}
OP(_025)
{
    int tmp;    // 4 AND  ZPG
    H6280_CYCLES(4);
    RD_ZPG;
    AND;
}
OP(_045)
{
    int tmp;    // 4 EOR  ZPG
    H6280_CYCLES(4);
    RD_ZPG;
    EOR;
}
OP(_065)
{
    int tmp;    // 4 ADC  ZPG
    H6280_CYCLES(4);
    RD_ZPG;
    ADC;
}
OP(_085)
{
    int tmp;    // 4 STA  ZPG
    H6280_CYCLES(4);
    STA;
    WR_ZPG;
}
OP(_0a5)
{
    int tmp;    // 4 LDA  ZPG
    H6280_CYCLES(4);
    RD_ZPG;
    LDA;
}
OP(_0c5)
{
    int tmp;    // 4 CMP  ZPG
    H6280_CYCLES(4);
    RD_ZPG;
    CMP;
}
OP(_0e5)
{
    int tmp;    // 4 SBC  ZPG
    H6280_CYCLES(4);
    RD_ZPG;
    SBC;
}

OP(_015)
{
    int tmp;    // 4 ORA  ZPX
    H6280_CYCLES(4);
    RD_ZPX;
    ORA;
}
OP(_035)
{
    int tmp;    // 4 AND  ZPX
    H6280_CYCLES(4);
    RD_ZPX;
    AND;
}
OP(_055)
{
    int tmp;    // 4 EOR  ZPX
    H6280_CYCLES(4);
    RD_ZPX;
    EOR;
}
OP(_075)
{
    int tmp;    // 4 ADC  ZPX
    H6280_CYCLES(4);
    RD_ZPX;
    ADC;
}
OP(_095)
{
    int tmp;    // 4 STA  ZPX
    H6280_CYCLES(4);
    STA;
    WR_ZPX;
}
OP(_0b5)
{
    int tmp;    // 4 LDA  ZPX
    H6280_CYCLES(4);
    RD_ZPX;
    LDA;
}
OP(_0d5)
{
    int tmp;    // 4 CMP  ZPX
    H6280_CYCLES(4);
    RD_ZPX;
    CMP;
}
OP(_0f5)
{
    int tmp;    // 4 SBC  ZPX
    H6280_CYCLES(4);
    RD_ZPX;
    SBC;
}

OP(_006)
{
    int tmp;    // 6 ASL  ZPG
    H6280_CYCLES(6);
    RD_ZPG;
    ASL;
    WB_EAZ;
}
OP(_026)
{
    int tmp;    // 6 ROL  ZPG
    H6280_CYCLES(6);
    RD_ZPG;
    ROL;
    WB_EAZ;
}
OP(_046)
{
    int tmp;    // 6 LSR  ZPG
    H6280_CYCLES(6);
    RD_ZPG;
    LSR;
    WB_EAZ;
}
OP(_066)
{
    int tmp;    // 6 ROR  ZPG
    H6280_CYCLES(6);
    RD_ZPG;
    ROR;
    WB_EAZ;
}
OP(_086)
{
    int tmp;    // 4 STX  ZPG
    H6280_CYCLES(4);
    STX;
    WR_ZPG;
}
OP(_0a6)
{
    int tmp;    // 4 LDX  ZPG
    H6280_CYCLES(4);
    RD_ZPG;
    LDX;
}
OP(_0c6)
{
    int tmp;    // 6 DEC  ZPG
    H6280_CYCLES(6);
    RD_ZPG;
    DEC;
    WB_EAZ;
}
OP(_0e6)
{
    int tmp;    // 6 INC  ZPG
    H6280_CYCLES(6);
    RD_ZPG;
    INC;
    WB_EAZ;
}

OP(_016)
{
    int tmp;    // 6 ASL  ZPX
    H6280_CYCLES(6);
    RD_ZPX;
    ASL;
    WB_EAZ
}
OP(_036)
{
    int tmp;    // 6 ROL  ZPX
    H6280_CYCLES(6);
    RD_ZPX;
    ROL;
    WB_EAZ
}
OP(_056)
{
    int tmp;    // 6 LSR  ZPX
    H6280_CYCLES(6);
    RD_ZPX;
    LSR;
    WB_EAZ
}
OP(_076)
{
    int tmp;    // 6 ROR  ZPX
    H6280_CYCLES(6);
    RD_ZPX;
    ROR;
    WB_EAZ
}
OP(_096)
{
    int tmp;    // 4 STX  ZPY
    H6280_CYCLES(4);
    STX;
    WR_ZPY;
}
OP(_0b6)
{
    int tmp;    // 4 LDX  ZPY
    H6280_CYCLES(4);
    RD_ZPY;
    LDX;
}
OP(_0d6)
{
    int tmp;    // 6 DEC  ZPX
    H6280_CYCLES(6);
    RD_ZPX;
    DEC;
    WB_EAZ;
}
OP(_0f6)
{
    int tmp;    // 6 INC  ZPX
    H6280_CYCLES(6);
    RD_ZPX;
    INC;
    WB_EAZ;
}

OP(_007)
{
    int tmp;    // 7 RMB0 ZPG
    H6280_CYCLES(7);
    RD_ZPG;
    RMB(0);
    WB_EAZ;
}
OP(_027)
{
    int tmp;    // 7 RMB2 ZPG
    H6280_CYCLES(7);
    RD_ZPG;
    RMB(2);
    WB_EAZ;
}
OP(_047)
{
    int tmp;    // 7 RMB4 ZPG
    H6280_CYCLES(7);
    RD_ZPG;
    RMB(4);
    WB_EAZ;
}
OP(_067)
{
    int tmp;    // 7 RMB6 ZPG
    H6280_CYCLES(7);
    RD_ZPG;
    RMB(6);
    WB_EAZ;
}
OP(_087)
{
    int tmp;    // 7 SMB0 ZPG
    H6280_CYCLES(7);
    RD_ZPG;
    SMB(0);
    WB_EAZ;
}
OP(_0a7)
{
    int tmp;    // 7 SMB2 ZPG
    H6280_CYCLES(7);
    RD_ZPG;
    SMB(2);
    WB_EAZ;
}
OP(_0c7)
{
    int tmp;    // 7 SMB4 ZPG
    H6280_CYCLES(7);
    RD_ZPG;
    SMB(4);
    WB_EAZ;
}
OP(_0e7)
{
    int tmp;    // 7 SMB6 ZPG
    H6280_CYCLES(7);
    RD_ZPG;
    SMB(6);
    WB_EAZ;
}

OP(_017)
{
    int tmp;    // 7 RMB1 ZPG
    H6280_CYCLES(7);
    RD_ZPG;
    RMB(1);
    WB_EAZ;
}
OP(_037)
{
    int tmp;    // 7 RMB3 ZPG
    H6280_CYCLES(7);
    RD_ZPG;
    RMB(3);
    WB_EAZ;
}
OP(_057)
{
    int tmp;    // 7 RMB5 ZPG
    H6280_CYCLES(7);
    RD_ZPG;
    RMB(5);
    WB_EAZ;
}
OP(_077)
{
    int tmp;    // 7 RMB7 ZPG
    H6280_CYCLES(7);
    RD_ZPG;
    RMB(7);
    WB_EAZ;
}
OP(_097)
{
    int tmp;    // 7 SMB1 ZPG
    H6280_CYCLES(7);
    RD_ZPG;
    SMB(1);
    WB_EAZ;
}
OP(_0b7)
{
    int tmp;    // 7 SMB3 ZPG
    H6280_CYCLES(7);
    RD_ZPG;
    SMB(3);
    WB_EAZ;
}
OP(_0d7)
{
    int tmp;    // 7 SMB5 ZPG
    H6280_CYCLES(7);
    RD_ZPG;
    SMB(5);
    WB_EAZ;
}
OP(_0f7)
{
    int tmp;    // 7 SMB7 ZPG
    H6280_CYCLES(7);
    RD_ZPG;
    SMB(7);
    WB_EAZ;
}

OP(_008)
{
    H6280_CYCLES(3);		     // 3 PHP
    PHP;
}
OP(_028)
{
    H6280_CYCLES(4);		     // 4 PLP
    PLP;
}
OP(_048)
{
    H6280_CYCLES(3);		     // 3 PHA
    PHA;
}
OP(_068)
{
    H6280_CYCLES(4);		     // 4 PLA
    PLA;
}
OP(_088)
{
    H6280_CYCLES(2);		     // 2 DEY
    DEY;
}
OP(_0a8)
{
    H6280_CYCLES(2);		     // 2 TAY
    TAY;
}
OP(_0c8)
{
    H6280_CYCLES(2);		     // 2 INY
    INY;
}
OP(_0e8)
{
    H6280_CYCLES(2);		     // 2 INX
    INX;
}

OP(_018)
{
    H6280_CYCLES(2);		     // 2 CLC
    CLC;
}
OP(_038)
{
    H6280_CYCLES(2);		     // 2 SEC
    SEC;
}
OP(_058)
{
    H6280_CYCLES(2);		     // 2 CLI
    CLI;
}
OP(_078)
{
    H6280_CYCLES(2);		     // 2 SEI
    SEI;
}
OP(_098)
{
    H6280_CYCLES(2);		     // 2 TYA
    TYA;
}
OP(_0b8)
{
    H6280_CYCLES(2);		     // 2 CLV
    CLV;
}
OP(_0d8)
{
    H6280_CYCLES(2);		     // 2 CLD
    CLD;
}
OP(_0f8)
{
    H6280_CYCLES(2);		     // 2 SED
    SED;
}

OP(_009)
{
    int tmp;    // 2 ORA  IMM
    H6280_CYCLES(2);
    RD_IMM;
    ORA;
}
OP(_029)
{
    int tmp;    // 2 AND  IMM
    H6280_CYCLES(2);
    RD_IMM;
    AND;
}
OP(_049)
{
    int tmp;    // 2 EOR  IMM
    H6280_CYCLES(2);
    RD_IMM;
    EOR;
}
OP(_069)
{
    int tmp;    // 2 ADC  IMM
    H6280_CYCLES(2);
    RD_IMM;
    ADC;
}
OP(_089)
{
    int tmp;    // 2 BIT  IMM
    H6280_CYCLES(2);
    RD_IMM;
    BIT;
}
OP(_0a9)
{
    int tmp;    // 2 LDA  IMM
    H6280_CYCLES(2);
    RD_IMM;
    LDA;
}
OP(_0c9)
{
    int tmp;    // 2 CMP  IMM
    H6280_CYCLES(2);
    RD_IMM;
    CMP;
}
OP(_0e9)
{
    int tmp;    // 2 SBC  IMM
    H6280_CYCLES(2);
    RD_IMM;
    SBC;
}

OP(_019)
{
    int tmp;    // 5 ORA  ABY
    H6280_CYCLES(5);
    RD_ABY;
    ORA;
}
OP(_039)
{
    int tmp;    // 5 AND  ABY
    H6280_CYCLES(5);
    RD_ABY;
    AND;
}
OP(_059)
{
    int tmp;    // 5 EOR  ABY
    H6280_CYCLES(5);
    RD_ABY;
    EOR;
}
OP(_079)
{
    int tmp;    // 5 ADC  ABY
    H6280_CYCLES(5);
    RD_ABY;
    ADC;
}
OP(_099)
{
    int tmp;    // 5 STA  ABY
    H6280_CYCLES(5);
    STA;
    WR_ABY;
}
OP(_0b9)
{
    int tmp;    // 5 LDA  ABY
    H6280_CYCLES(5);
    RD_ABY;
    LDA;
}
OP(_0d9)
{
    int tmp;    // 5 CMP  ABY
    H6280_CYCLES(5);
    RD_ABY;
    CMP;
}
OP(_0f9)
{
    int tmp;    // 5 SBC  ABY
    H6280_CYCLES(5);
    RD_ABY;
    SBC;
}

OP(_00a)
{
    int tmp;    // 2 ASL  A
    H6280_CYCLES(2);
    RD_ACC;
    ASL;
    WB_ACC;
}
OP(_02a)
{
    int tmp;    // 2 ROL  A
    H6280_CYCLES(2);
    RD_ACC;
    ROL;
    WB_ACC;
}
OP(_04a)
{
    int tmp;    // 2 LSR  A
    H6280_CYCLES(2);
    RD_ACC;
    LSR;
    WB_ACC;
}
OP(_06a)
{
    int tmp;    // 2 ROR  A
    H6280_CYCLES(2);
    RD_ACC;
    ROR;
    WB_ACC;
}
OP(_08a)
{
    H6280_CYCLES(2);		     // 2 TXA
    TXA;
}
OP(_0aa)
{
    H6280_CYCLES(2);		     // 2 TAX
    TAX;
}
OP(_0ca)
{
    H6280_CYCLES(2);		     // 2 DEX
    DEX;
}
OP(_0ea)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}

OP(_01a)
{
    H6280_CYCLES(2);		     // 2 INC  A
    INA;
}
OP(_03a)
{
    H6280_CYCLES(2);		     // 2 DEC  A
    DEA;
}
OP(_05a)
{
    H6280_CYCLES(3);		     // 3 PHY
    PHY;
}
OP(_07a)
{
    H6280_CYCLES(4);		     // 4 PLY
    PLY;
}
OP(_09a)
{
    H6280_CYCLES(2);		     // 2 TXS
    TXS;
}
OP(_0ba)
{
    H6280_CYCLES(2);		     // 2 TSX
    TSX;
}
OP(_0da)
{
    H6280_CYCLES(3);		     // 3 PHX
    PHX;
}
OP(_0fa)
{
    H6280_CYCLES(4);		     // 4 PLX
    PLX;
}

OP(_00b)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}
OP(_02b)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}
OP(_04b)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}
OP(_06b)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}
OP(_08b)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}
OP(_0ab)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}
OP(_0cb)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}
OP(_0eb)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}

OP(_01b)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}
OP(_03b)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}
OP(_05b)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}
OP(_07b)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}
OP(_09b)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}
OP(_0bb)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}
OP(_0db)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}
OP(_0fb)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}

OP(_00c)
{
    int tmp;    // 7 TSB  ABS
    H6280_CYCLES(7);
    RD_ABS;
    TSB;
    WB_EA;
}
OP(_02c)
{
    int tmp;    // 5 BIT  ABS
    H6280_CYCLES(5);
    RD_ABS;
    BIT;
}
OP(_04c)
{
    H6280_CYCLES(4);    // 4 JMP  ABS
    EA_ABS;
    JMP;
}
OP(_06c)
{
    int tmp;    // 7 JMP  IND
    H6280_CYCLES(7);
    EA_IND;
    JMP;
}
OP(_08c)
{
    int tmp;    // 5 STY  ABS
    H6280_CYCLES(5);
    STY;
    WR_ABS;
}
OP(_0ac)
{
    int tmp;    // 5 LDY  ABS
    H6280_CYCLES(5);
    RD_ABS;
    LDY;
}
OP(_0cc)
{
    int tmp;    // 5 CPY  ABS
    H6280_CYCLES(5);
    RD_ABS;
    CPY;
}
OP(_0ec)
{
    int tmp;    // 5 CPX  ABS
    H6280_CYCLES(5);
    RD_ABS;
    CPX;
}

OP(_01c)
{
    int tmp;    // 7 TRB  ABS
    H6280_CYCLES(7);
    RD_ABS;
    TRB;
    WB_EA;
}
OP(_03c)
{
    int tmp;    // 5 BIT  ABX
    H6280_CYCLES(5);
    RD_ABX;
    BIT;
}
OP(_05c)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}
OP(_07c)
{
    int tmp;    // 7 JMP  IAX
    H6280_CYCLES(7);
    EA_IAX;
    JMP;
}
OP(_09c)
{
    int tmp;    // 5 STZ  ABS
    H6280_CYCLES(5);
    STZ;
    WR_ABS;
}
OP(_0bc)
{
    int tmp;    // 5 LDY  ABX
    H6280_CYCLES(5);
    RD_ABX;
    LDY;
}
OP(_0dc)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}
OP(_0fc)
{
    H6280_CYCLES(2);		     // 2 NOP
    NOP;
}

OP(_00d)
{
    int tmp;    // 5 ORA  ABS
    H6280_CYCLES(5);
    RD_ABS;
    ORA;
}
OP(_02d)
{
    int tmp;    // 5 AND  ABS
    H6280_CYCLES(5);
    RD_ABS;
    AND;
}
OP(_04d)
{
    int tmp;    // 5 EOR  ABS
    H6280_CYCLES(5);
    RD_ABS;
    EOR;
}
OP(_06d)
{
    int tmp;    // 5 ADC  ABS
    H6280_CYCLES(5);
    RD_ABS;
    ADC;
}
OP(_08d)
{
    int tmp;    // 5 STA  ABS
    H6280_CYCLES(5);
    STA;
    WR_ABS;
}
OP(_0ad)
{
    int tmp;    // 5 LDA  ABS
    H6280_CYCLES(5);
    RD_ABS;
    LDA;
}
OP(_0cd)
{
    int tmp;    // 5 CMP  ABS
    H6280_CYCLES(5);
    RD_ABS;
    CMP;
}
OP(_0ed)
{
    int tmp;    // 5 SBC  ABS
    H6280_CYCLES(5);
    RD_ABS;
    SBC;
}

OP(_01d)
{
    int tmp;    // 5 ORA  ABX
    H6280_CYCLES(5);
    RD_ABX;
    ORA;
}
OP(_03d)
{
    int tmp;    // 5 AND  ABX
    H6280_CYCLES(5);
    RD_ABX;
    AND;
}
OP(_05d)
{
    int tmp;    // 5 EOR  ABX
    H6280_CYCLES(5);
    RD_ABX;
    EOR;
}
OP(_07d)
{
    int tmp;    // 5 ADC  ABX
    H6280_CYCLES(5);
    RD_ABX;
    ADC;
}
OP(_09d)
{
    int tmp;    // 5 STA  ABX
    H6280_CYCLES(5);
    STA;
    WR_ABX;
}
OP(_0bd)
{
    int tmp;    // 5 LDA  ABX
    H6280_CYCLES(5);
    RD_ABX;
    LDA;
}
OP(_0dd)
{
    int tmp;    // 5 CMP  ABX
    H6280_CYCLES(5);
    RD_ABX;
    CMP;
}
OP(_0fd)
{
    int tmp;    // 5 SBC  ABX
    H6280_CYCLES(5);
    RD_ABX;
    SBC;
}

OP(_00e)
{
    int tmp;    // 7 ASL  ABS
    H6280_CYCLES(7);
    RD_ABS;
    ASL;
    WB_EA;
}
OP(_02e)
{
    int tmp;    // 7 ROL  ABS
    H6280_CYCLES(7);
    RD_ABS;
    ROL;
    WB_EA;
}
OP(_04e)
{
    int tmp;    // 7 LSR  ABS
    H6280_CYCLES(7);
    RD_ABS;
    LSR;
    WB_EA;
}
OP(_06e)
{
    int tmp;    // 7 ROR  ABS
    H6280_CYCLES(7);
    RD_ABS;
    ROR;
    WB_EA;
}
OP(_08e)
{
    int tmp;    // 5 STX  ABS
    H6280_CYCLES(5);
    STX;
    WR_ABS;
}
OP(_0ae)
{
    int tmp;    // 5 LDX  ABS
    H6280_CYCLES(5);
    RD_ABS;
    LDX;
}
OP(_0ce)
{
    int tmp;    // 7 DEC  ABS
    H6280_CYCLES(7);
    RD_ABS;
    DEC;
    WB_EA;
}
OP(_0ee)
{
    int tmp;    // 7 INC  ABS
    H6280_CYCLES(7);
    RD_ABS;
    INC;
    WB_EA;
}

OP(_01e)
{
    int tmp;    // 7 ASL  ABX
    H6280_CYCLES(7);
    RD_ABX;
    ASL;
    WB_EA;
}
OP(_03e)
{
    int tmp;    // 7 ROL  ABX
    H6280_CYCLES(7);
    RD_ABX;
    ROL;
    WB_EA;
}
OP(_05e)
{
    int tmp;    // 7 LSR  ABX
    H6280_CYCLES(7);
    RD_ABX;
    LSR;
    WB_EA;
}
OP(_07e)
{
    int tmp;    // 7 ROR  ABX
    H6280_CYCLES(7);
    RD_ABX;
    ROR;
    WB_EA;
}
OP(_09e)
{
    int tmp;    // 5 STZ  ABX
    H6280_CYCLES(5);
    STZ;
    WR_ABX;
}
OP(_0be)
{
    int tmp;    // 5 LDX  ABY
    H6280_CYCLES(5);
    RD_ABY;
    LDX;
}
OP(_0de)
{
    int tmp;    // 7 DEC  ABX
    H6280_CYCLES(7);
    RD_ABX;
    DEC;
    WB_EA;
}
OP(_0fe)
{
    int tmp;    // 7 INC  ABX
    H6280_CYCLES(7);
    RD_ABX;
    INC;
    WB_EA;
}

OP(_00f)
{
    int tmp;    // 6/8 BBR0 ZPG,REL
    H6280_CYCLES(4);
    RD_ZPG;
    BBR(0);
}
OP(_02f)
{
    int tmp;    // 6/8 BBR2 ZPG,REL
    H6280_CYCLES(4);
    RD_ZPG;
    BBR(2);
}
OP(_04f)
{
    int tmp;    // 6/8 BBR4 ZPG,REL
    H6280_CYCLES(4);
    RD_ZPG;
    BBR(4);
}
OP(_06f)
{
    int tmp;    // 6/8 BBR6 ZPG,REL
    H6280_CYCLES(4);
    RD_ZPG;
    BBR(6);
}
OP(_08f)
{
    int tmp;    // 6/8 BBS0 ZPG,REL
    H6280_CYCLES(4);
    RD_ZPG;
    BBS(0);
}
OP(_0af)
{
    int tmp;    // 6/8 BBS2 ZPG,REL
    H6280_CYCLES(4);
    RD_ZPG;
    BBS(2);
}
OP(_0cf)
{
    int tmp;    // 6/8 BBS4 ZPG,REL
    H6280_CYCLES(4);
    RD_ZPG;
    BBS(4);
}
OP(_0ef)
{
    int tmp;    // 6/8 BBS6 ZPG,REL
    H6280_CYCLES(4);
    RD_ZPG;
    BBS(6);
}

OP(_01f)
{
    int tmp;    // 6/8 BBR1 ZPG,REL
    H6280_CYCLES(4);
    RD_ZPG;
    BBR(1);
}
OP(_03f)
{
    int tmp;    // 6/8 BBR3 ZPG,REL
    H6280_CYCLES(4);
    RD_ZPG;
    BBR(3);
}
OP(_05f)
{
    int tmp;    // 6/8 BBR5 ZPG,REL
    H6280_CYCLES(4);
    RD_ZPG;
    BBR(5);
}
OP(_07f)
{
    int tmp;    // 6/8 BBR7 ZPG,REL
    H6280_CYCLES(4);
    RD_ZPG;
    BBR(7);
}
OP(_09f)
{
    int tmp;    // 6/8 BBS1 ZPG,REL
    H6280_CYCLES(4);
    RD_ZPG;
    BBS(1);
}
OP(_0bf)
{
    int tmp;    // 6/8 BBS3 ZPG,REL
    H6280_CYCLES(4);
    RD_ZPG;
    BBS(3);
}
OP(_0df)
{
    int tmp;    // 6/8 BBS5 ZPG,REL
    H6280_CYCLES(4);
    RD_ZPG;
    BBS(5);
}
OP(_0ff)
{
    int tmp;    // 6/8 BBS7 ZPG,REL
    H6280_CYCLES(4);
    RD_ZPG;
    BBS(7);
}

static void (*const insnh6280[0x100])(void) =
{
    h6280_000, h6280_001, h6280_002, h6280_003, h6280_004, h6280_005, h6280_006, h6280_007,
    h6280_008, h6280_009, h6280_00a, h6280_00b, h6280_00c, h6280_00d, h6280_00e, h6280_00f,
    h6280_010, h6280_011, h6280_012, h6280_013, h6280_014, h6280_015, h6280_016, h6280_017,
    h6280_018, h6280_019, h6280_01a, h6280_01b, h6280_01c, h6280_01d, h6280_01e, h6280_01f,
    h6280_020, h6280_021, h6280_022, h6280_023, h6280_024, h6280_025, h6280_026, h6280_027,
    h6280_028, h6280_029, h6280_02a, h6280_02b, h6280_02c, h6280_02d, h6280_02e, h6280_02f,
    h6280_030, h6280_031, h6280_032, h6280_033, h6280_034, h6280_035, h6280_036, h6280_037,
    h6280_038, h6280_039, h6280_03a, h6280_03b, h6280_03c, h6280_03d, h6280_03e, h6280_03f,
    h6280_040, h6280_041, h6280_042, h6280_043, h6280_044, h6280_045, h6280_046, h6280_047,
    h6280_048, h6280_049, h6280_04a, h6280_04b, h6280_04c, h6280_04d, h6280_04e, h6280_04f,
    h6280_050, h6280_051, h6280_052, h6280_053, h6280_054, h6280_055, h6280_056, h6280_057,
    h6280_058, h6280_059, h6280_05a, h6280_05b, h6280_05c, h6280_05d, h6280_05e, h6280_05f,
    h6280_060, h6280_061, h6280_062, h6280_063, h6280_064, h6280_065, h6280_066, h6280_067,
    h6280_068, h6280_069, h6280_06a, h6280_06b, h6280_06c, h6280_06d, h6280_06e, h6280_06f,
    h6280_070, h6280_071, h6280_072, h6280_073, h6280_074, h6280_075, h6280_076, h6280_077,
    h6280_078, h6280_079, h6280_07a, h6280_07b, h6280_07c, h6280_07d, h6280_07e, h6280_07f,
    h6280_080, h6280_081, h6280_082, h6280_083, h6280_084, h6280_085, h6280_086, h6280_087,
    h6280_088, h6280_089, h6280_08a, h6280_08b, h6280_08c, h6280_08d, h6280_08e, h6280_08f,
    h6280_090, h6280_091, h6280_092, h6280_093, h6280_094, h6280_095, h6280_096, h6280_097,
    h6280_098, h6280_099, h6280_09a, h6280_09b, h6280_09c, h6280_09d, h6280_09e, h6280_09f,
    h6280_0a0, h6280_0a1, h6280_0a2, h6280_0a3, h6280_0a4, h6280_0a5, h6280_0a6, h6280_0a7,
    h6280_0a8, h6280_0a9, h6280_0aa, h6280_0ab, h6280_0ac, h6280_0ad, h6280_0ae, h6280_0af,
    h6280_0b0, h6280_0b1, h6280_0b2, h6280_0b3, h6280_0b4, h6280_0b5, h6280_0b6, h6280_0b7,
    h6280_0b8, h6280_0b9, h6280_0ba, h6280_0bb, h6280_0bc, h6280_0bd, h6280_0be, h6280_0bf,
    h6280_0c0, h6280_0c1, h6280_0c2, h6280_0c3, h6280_0c4, h6280_0c5, h6280_0c6, h6280_0c7,
    h6280_0c8, h6280_0c9, h6280_0ca, h6280_0cb, h6280_0cc, h6280_0cd, h6280_0ce, h6280_0cf,
    h6280_0d0, h6280_0d1, h6280_0d2, h6280_0d3, h6280_0d4, h6280_0d5, h6280_0d6, h6280_0d7,
    h6280_0d8, h6280_0d9, h6280_0da, h6280_0db, h6280_0dc, h6280_0dd, h6280_0de, h6280_0df,
    h6280_0e0, h6280_0e1, h6280_0e2, h6280_0e3, h6280_0e4, h6280_0e5, h6280_0e6, h6280_0e7,
    h6280_0e8, h6280_0e9, h6280_0ea, h6280_0eb, h6280_0ec, h6280_0ed, h6280_0ee, h6280_0ef,
    h6280_0f0, h6280_0f1, h6280_0f2, h6280_0f3, h6280_0f4, h6280_0f5, h6280_0f6, h6280_0f7,
    h6280_0f8, h6280_0f9, h6280_0fa, h6280_0fb, h6280_0fc, h6280_0fd, h6280_0fe, h6280_0ff
};
