#pragma once

#include <biscuit/code_buffer.hpp>
#include <biscuit/label.hpp>
#include <biscuit/registers.hpp>
#include <cstddef>
#include <cstdint>

namespace biscuit {

/**
 * Base class that all assembler instances will inherit from
 * to generate RISC-V code.
 */
class Assembler {
public:
    // Control and Status Register
    enum class CSR : uint32_t {
        FFlags = 0x001,   // Floating-point Accrued Exceptions
        FRM = 0x002,      // Floating-point Dynamic Rounding Mode
        FCSR = 0x003,     // Floating-point Control and Status Register (frm + fflags)

        Cycle = 0xC00,    // Cycle counter for RDCYCLE instruction.
        Time = 0xC01,     // Timer for RDTIME instruction.
        InstRet = 0xC02,  // Instructions retired counter for RDINSTRET instruction.
        CycleH = 0xC80,   // Upper 32 bits of cycle, RV32I only.
        TimeH = 0xC81,    // Upper 32 bits of time, RV32I only.
        InstRetH = 0xC82, // Upper 32 bits of instret, RV32I only.
    };

    enum class FenceOrder : uint32_t {
        W = 1, // Write
        R = 2, // Read
        O = 4, // Device Output
        I = 8, // Device Input

        RW = R | W,

        IO = I | O,
        IR = I | R,
        IW = I | W,
        IRW = I | R | W,

        OI = O | I,
        OR = O | R,
        OW = O | W,
        ORW = O | R | W,

        IORW = I | O | R | W,
    };

    // Atomic ordering
    enum class Ordering : uint32_t {
        None = 0,       // None
        RL = 1,         // Release
        AQ = 2,         // Acquire
        AQRL = AQ | RL, // Acquire-Release
    };

    // Floating-point Rounding Mode
    enum class RMode : uint32_t {
        RNE = 0b000, // Round to Nearest, ties to Even
        RTZ = 0b001, // Round towards Zero
        RDN = 0b010, // Round Down (towards negative infinity)
        RUP = 0b011, // Round Up (towards positive infinity)
        RMM = 0b100, // Round to Nearest, ties to Max Magnitude
        DYN = 0b111, // Dynamic Rounding Mode
    };

    /**
     * Constructor
     *
     * Initializes the underlying code buffer to be able to hold `capacity` bytes.
     *
     * @param capacity The capacity for the underlying code buffer in bytes.
     *                 If no capacity is specified, then the underlying buffer
     *                 will be 4KB in size.
     */
    [[nodiscard]] explicit Assembler(size_t capacity = CodeBuffer::default_capacity)
        : m_buffer(capacity) {}

   /**
     * Constructor
     *
     * @param buffer   A non-null pointer to an allocated buffer of size `capacity`.
     * @param capacity The capacity of the memory pointed to by `buffer`.
     *
     * @pre The given memory buffer must not be null.
     * @pre The given memory buffer must be at minimum `capacity` bytes in size.
     *
     * @note The caller is responsible for managing the lifetime of the given memory.
     *       CodeBuffer will *not* free the memory once it goes out of scope.
     */
    [[nodiscard]] explicit Assembler(uint8_t* buffer, size_t capacity)
        : m_buffer(buffer, capacity) {}

    // Copy constructor and assignment.
    Assembler(const Assembler&) = delete;
    Assembler& operator=(const Assembler&) = delete;

    // Move constructor and assignment.
    Assembler(Assembler&&) = default;
    Assembler& operator=(Assembler&&) = default;

    // Destructor
    virtual ~Assembler() = default;

    /**
     * Allows rewinding of the code buffer cursor.
     *
     * @param offset The offset to rewind the cursor by.
     *
     * @note If no offset is provided, then this function rewinds the
     *       cursor to the beginning of the buffer.
     *
     * @note The offset may not be larger than the current cursor offset
     *       and may not be less than the current buffer starting address.
     */
    void RewindBuffer(ptrdiff_t offset = 0) {
        m_buffer.RewindCursor(offset);
    }

    /**
     * Binds a label to the current offset within the code buffer
     *
     * @param label A non-null valid label to bind.
     */
    void Bind(Label* label);

    // RV32I Instructions

    void ADD(GPR rd, GPR lhs, GPR rhs) noexcept;
    void ADDI(GPR rd, GPR rs, int32_t imm) noexcept;
    void AND(GPR rd, GPR lhs, GPR rhs) noexcept;
    void ANDI(GPR rd, GPR rs, uint32_t imm) noexcept;

    void AUIPC(GPR rd, uint32_t imm) noexcept;

    void BEQ(GPR rs1, GPR rs2, Label* label) noexcept;
    void BEQZ(GPR rs, Label* label) noexcept;
    void BGE(GPR rs1, GPR rs2, Label* label) noexcept;
    void BGEU(GPR rs1, GPR rs2, Label* label) noexcept;
    void BGEZ(GPR rs, Label* label) noexcept;
    void BGT(GPR rs, GPR rt, Label* label) noexcept;
    void BGTU(GPR rs, GPR rt, Label* label) noexcept;
    void BGTZ(GPR rs, Label* label) noexcept;
    void BLE(GPR rs, GPR rt, Label* label) noexcept;
    void BLEU(GPR rs, GPR rt, Label* label) noexcept;
    void BLEZ(GPR rs, Label* label) noexcept;
    void BLT(GPR rs1, GPR rs2, Label* label) noexcept;
    void BLTU(GPR rs1, GPR rs2, Label* label) noexcept;
    void BLTZ(GPR rs, Label* label) noexcept;
    void BNE(GPR rs1, GPR rs2, Label* label) noexcept;
    void BNEZ(GPR rs, Label* label) noexcept;

    void BEQ(GPR rs1, GPR rs2, int32_t imm) noexcept;
    void BEQZ(GPR rs, int32_t imm) noexcept;
    void BGE(GPR rs1, GPR rs2, int32_t imm) noexcept;
    void BGEU(GPR rs1, GPR rs2, int32_t imm) noexcept;
    void BGEZ(GPR rs, int32_t imm) noexcept;
    void BGT(GPR rs, GPR rt, int32_t imm) noexcept;
    void BGTU(GPR rs, GPR rt, int32_t imm) noexcept;
    void BGTZ(GPR rs, int32_t imm) noexcept;
    void BLE(GPR rs, GPR rt, int32_t imm) noexcept;
    void BLEU(GPR rs, GPR rt, int32_t imm) noexcept;
    void BLEZ(GPR rs, int32_t imm) noexcept;
    void BLT(GPR rs1, GPR rs2, int32_t imm) noexcept;
    void BLTU(GPR rs1, GPR rs2, int32_t imm) noexcept;
    void BLTZ(GPR rs, int32_t imm) noexcept;
    void BNE(GPR rs1, GPR rs2, int32_t imm) noexcept;
    void BNEZ(GPR rs, int32_t imm) noexcept;

    void EBREAK() noexcept;
    void ECALL() noexcept;

    void FENCE() noexcept;
    void FENCE(FenceOrder pred, FenceOrder succ) noexcept;
    void FENCEI(GPR rd = x0, GPR rs = x0, uint32_t imm = 0) noexcept;
    void FENCETSO() noexcept;

    void J(Label* label) noexcept;
    void JAL(Label* label) noexcept;
    void JAL(GPR rd, Label* label) noexcept;

    void J(int32_t imm) noexcept;
    void JAL(int32_t imm) noexcept;
    void JAL(GPR rd, int32_t imm) noexcept;
    void JALR(GPR rs) noexcept;
    void JALR(GPR rd, int32_t imm, GPR rs1) noexcept;
    void JR(GPR rs) noexcept;

    void LB(GPR rd, uint32_t imm, GPR rs) noexcept;
    void LBU(GPR rd, uint32_t imm, GPR rs) noexcept;
    void LH(GPR rd, uint32_t imm, GPR rs) noexcept;
    void LHU(GPR rd, uint32_t imm, GPR rs) noexcept;
    void LUI(GPR rd, uint32_t imm) noexcept;
    void LW(GPR rd, uint32_t imm, GPR rs) noexcept;

    void MV(GPR rd, GPR rs) noexcept;
    void NEG(GPR rd, GPR rs) noexcept;

    void NOP() noexcept;

    void NOT(GPR rd, GPR rs) noexcept;
    void OR(GPR rd, GPR lhs, GPR rhs) noexcept;
    void ORI(GPR rd, GPR rs, uint32_t imm) noexcept;

    void PAUSE() noexcept;
    void RET() noexcept;

    void SB(GPR rs2, uint32_t imm, GPR rs1) noexcept;
    void SH(GPR rs2, uint32_t imm, GPR rs1) noexcept;
    void SW(GPR rs2, uint32_t imm, GPR rs1) noexcept;

    void SEQZ(GPR rd, GPR rs) noexcept;
    void SGTZ(GPR rd, GPR rs) noexcept;

    void SLL(GPR rd, GPR lhs, GPR rhs) noexcept;
    void SLLI(GPR rd, GPR rs, uint32_t shift) noexcept;

    void SLT(GPR rd, GPR lhs, GPR rhs) noexcept;
    void SLTI(GPR rd, GPR rs, uint32_t imm) noexcept;
    void SLTIU(GPR rd, GPR rs, uint32_t imm) noexcept;
    void SLTU(GPR rd, GPR lhs, GPR rhs) noexcept;
    void SLTZ(GPR rd, GPR rs) noexcept;

    void SNEZ(GPR rd, GPR rs) noexcept;

    void SRA(GPR rd, GPR lhs, GPR rhs) noexcept;
    void SRAI(GPR rd, GPR rs, uint32_t shift) noexcept;

    void SRL(GPR rd, GPR lhs, GPR rhs) noexcept;
    void SRLI(GPR rd, GPR rs, uint32_t shift) noexcept;

    void SUB(GPR rd, GPR lhs, GPR rhs) noexcept;

    void XOR(GPR rd, GPR lhs, GPR rhs) noexcept;
    void XORI(GPR rd, GPR rs, uint32_t imm) noexcept;

    // RV64I Base Instruction Set

    void ADDIW(GPR rd, GPR rs, int32_t imm) noexcept;
    void ADDW(GPR rd, GPR lhs, GPR rhs) noexcept;
    void LD(GPR rd, uint32_t imm, GPR rs) noexcept;
    void LWU(GPR rd, GPR rs, uint32_t imm) noexcept;
    void SD(GPR rs2, uint32_t imm, GPR rs1) noexcept;

    // NOTE: Perhaps we should coalesce this into the 32-bit variant?
    //       Keeping them separated would allow asserts for catching
    //       out of range shifts in the future.
    void SRAI64(GPR rd, GPR rs, uint32_t shift) noexcept;
    void SLLI64(GPR rd, GPR rs, uint32_t shift) noexcept;
    void SRLI64(GPR rd, GPR rs, uint32_t shift) noexcept;

    void SLLIW(GPR rd, GPR rs, uint32_t shift) noexcept;
    void SRAIW(GPR rd, GPR rs, uint32_t shift) noexcept;
    void SRLIW(GPR rd, GPR rs, uint32_t shift) noexcept;

    void SLLW(GPR rd, GPR lhs, GPR rhs) noexcept;
    void SRAW(GPR rd, GPR lhs, GPR rhs) noexcept;
    void SRLW(GPR rd, GPR lhs, GPR rhs) noexcept;
    void SUBW(GPR rd, GPR lhs, GPR rhs) noexcept;

    // Zicsr Extension Instructions

    void CSRRC(GPR rd, CSR csr, GPR rs) noexcept;
    void CSRRCI(GPR rd, CSR csr, uint32_t imm) noexcept;
    void CSRRS(GPR rd, CSR csr, GPR rs) noexcept;
    void CSRRSI(GPR rd, CSR csr, uint32_t imm) noexcept;
    void CSRRW(GPR rd, CSR csr, GPR rs) noexcept;
    void CSRRWI(GPR rd, CSR csr, uint32_t imm) noexcept;

    void CSRR(GPR rd, CSR csr) noexcept;
    void CSWR(CSR csr, GPR rs) noexcept;

    void CSRS(CSR csr, GPR rs) noexcept;
    void CSRC(CSR csr, GPR rs) noexcept;

    void CSRCI(CSR csr, uint32_t imm) noexcept;
    void CSRSI(CSR csr, uint32_t imm) noexcept;
    void CSRWI(CSR csr, uint32_t imm) noexcept;

    void FRCSR(GPR rd) noexcept;
    void FSCSR(GPR rd, GPR rs) noexcept;
    void FSCSR(GPR rs) noexcept;

    void FRRM(GPR rd) noexcept;
    void FSRM(GPR rd, GPR rs) noexcept;
    void FSRM(GPR rs) noexcept;

    void FSRMI(GPR rd, uint32_t imm) noexcept;
    void FSRMI(uint32_t imm) noexcept;

    void FRFLAGS(GPR rd) noexcept;
    void FSFLAGS(GPR rd, GPR rs) noexcept;
    void FSFLAGS(GPR rs) noexcept;

    void FSFLAGSI(GPR rd, uint32_t imm) noexcept;
    void FSFLAGSI(uint32_t imm) noexcept;

    void RDCYCLE(GPR rd) noexcept;
    void RDCYCLEH(GPR rd) noexcept;

    void RDINSTRET(GPR rd) noexcept;
    void RDINSTRETH(GPR rd) noexcept;

    void RDTIME(GPR rd) noexcept;
    void RDTIMEH(GPR rd) noexcept;

    // RV32M Extension Instructions

    void DIV(GPR rd, GPR rs1, GPR rs2) noexcept;
    void DIVU(GPR rd, GPR rs1, GPR rs2) noexcept;
    void MUL(GPR rd, GPR rs1, GPR rs2) noexcept;
    void MULH(GPR rd, GPR rs1, GPR rs2) noexcept;
    void MULHSU(GPR rd, GPR rs1, GPR rs2) noexcept;
    void MULHU(GPR rd, GPR rs1, GPR rs2) noexcept;
    void REM(GPR rd, GPR rs1, GPR rs2) noexcept;
    void REMU(GPR rd, GPR rs1, GPR rs2) noexcept;

    // RV64M Extension Instructions

    void DIVW(GPR rd, GPR rs1, GPR rs2) noexcept;
    void DIVUW(GPR rd, GPR rs1, GPR rs2) noexcept;
    void MULW(GPR rd, GPR rs1, GPR rs2) noexcept;
    void REMW(GPR rd, GPR rs1, GPR rs2) noexcept;
    void REMUW(GPR rd, GPR rs1, GPR rs2) noexcept;

    // RV32A Extension Instructions

    void AMOADD_W(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void AMOAND_W(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void AMOMAX_W(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void AMOMAXU_W(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void AMOMIN_W(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void AMOMINU_W(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void AMOOR_W(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void AMOSWAP_W(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void AMOXOR_W(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void LR_W(Ordering ordering, GPR rd, GPR rs) noexcept;
    void SC_W(Ordering ordering, GPR rd, GPR rs1, GPR rs2) noexcept;

    // RV64A Extension Instructions

    void AMOADD_D(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void AMOAND_D(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void AMOMAX_D(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void AMOMAXU_D(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void AMOMIN_D(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void AMOMINU_D(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void AMOOR_D(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void AMOSWAP_D(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void AMOXOR_D(Ordering ordering, GPR rd, GPR rs2, GPR rs1) noexcept;
    void LR_D(Ordering ordering, GPR rd, GPR rs) noexcept;
    void SC_D(Ordering ordering, GPR rd, GPR rs1, GPR rs2) noexcept;

    // RV32F Extension Instructions

    void FADD_S(FPR rd, FPR rs1, FPR rs2, RMode rmode = RMode::DYN) noexcept;
    void FCLASS_S(GPR rd, FPR rs1) noexcept;
    void FCVT_S_W(FPR rd, GPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_S_WU(FPR rd, GPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_W_S(GPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_WU_S(GPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FDIV_S(FPR rd, FPR rs1, FPR rs2, RMode rmode = RMode::DYN) noexcept;
    void FEQ_S(GPR rd, FPR rs1, FPR rs2) noexcept;
    void FLE_S(GPR rd, FPR rs1, FPR rs2) noexcept;
    void FLT_S(GPR rd, FPR rs1, FPR rs2) noexcept;
    void FLW(FPR rd, uint32_t offset, GPR rs) noexcept;
    void FMADD_S(FPR rd, FPR rs1, FPR rs2, FPR rs3, RMode rmode = RMode::DYN) noexcept;
    void FMAX_S(FPR rd, FPR rs1, FPR rs2) noexcept;
    void FMIN_S(FPR rd, FPR rs1, FPR rs2) noexcept;
    void FMSUB_S(FPR rd, FPR rs1, FPR rs2, FPR rs3, RMode rmode = RMode::DYN) noexcept;
    void FMUL_S(FPR rd, FPR rs1, FPR rs2, RMode rmode = RMode::DYN) noexcept;
    void FMV_W_X(FPR rd, GPR rs1) noexcept;
    void FMV_X_W(GPR rd, FPR rs1) noexcept;
    void FNMADD_S(FPR rd, FPR rs1, FPR rs2, FPR rs3, RMode rmode = RMode::DYN) noexcept;
    void FNMSUB_S(FPR rd, FPR rs1, FPR rs2, FPR rs3, RMode rmode = RMode::DYN) noexcept;
    void FSGNJ_S(FPR rd, FPR rs1, FPR rs2) noexcept;
    void FSGNJN_S(FPR rd, FPR rs1, FPR rs2) noexcept;
    void FSGNJX_S(FPR rd, FPR rs1, FPR rs2) noexcept;
    void FSQRT_S(FPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FSUB_S(FPR rd, FPR rs1, FPR rs2, RMode rmode = RMode::DYN) noexcept;
    void FSW(FPR rs2, uint32_t offset, GPR rs1) noexcept;

    void FABS_S(FPR rd, FPR rs) noexcept;
    void FMV_S(FPR rd, FPR rs) noexcept;
    void FNEG_S(FPR rd, FPR rs) noexcept;

    // RV64F Extension Instructions

    void FCVT_L_S(GPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_LU_S(GPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_S_L(FPR rd, GPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_S_LU(FPR rd, GPR rs1, RMode rmode = RMode::DYN) noexcept;

    // RV32D Extension Instructions

    void FADD_D(FPR rd, FPR rs1, FPR rs2, RMode rmode = RMode::DYN) noexcept;
    void FCLASS_D(GPR rd, FPR rs1) noexcept;
    void FCVT_D_W(FPR rd, GPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_D_WU(FPR rd, GPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_W_D(GPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_WU_D(GPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_D_S(FPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_S_D(FPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FDIV_D(FPR rd, FPR rs1, FPR rs2, RMode rmode = RMode::DYN) noexcept;
    void FEQ_D(GPR rd, FPR rs1, FPR rs2) noexcept;
    void FLE_D(GPR rd, FPR rs1, FPR rs2) noexcept;
    void FLT_D(GPR rd, FPR rs1, FPR rs2) noexcept;
    void FLD(FPR rd, uint32_t offset, GPR rs) noexcept;
    void FMADD_D(FPR rd, FPR rs1, FPR rs2, FPR rs3, RMode rmode = RMode::DYN) noexcept;
    void FMAX_D(FPR rd, FPR rs1, FPR rs2) noexcept;
    void FMIN_D(FPR rd, FPR rs1, FPR rs2) noexcept;
    void FMSUB_D(FPR rd, FPR rs1, FPR rs2, FPR rs3, RMode rmode = RMode::DYN) noexcept;
    void FMUL_D(FPR rd, FPR rs1, FPR rs2, RMode rmode = RMode::DYN) noexcept;
    void FNMADD_D(FPR rd, FPR rs1, FPR rs2, FPR rs3, RMode rmode = RMode::DYN) noexcept;
    void FNMSUB_D(FPR rd, FPR rs1, FPR rs2, FPR rs3, RMode rmode = RMode::DYN) noexcept;
    void FSGNJ_D(FPR rd, FPR rs1, FPR rs2) noexcept;
    void FSGNJN_D(FPR rd, FPR rs1, FPR rs2) noexcept;
    void FSGNJX_D(FPR rd, FPR rs1, FPR rs2) noexcept;
    void FSQRT_D(FPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FSUB_D(FPR rd, FPR rs1, FPR rs2, RMode rmode = RMode::DYN) noexcept;
    void FSD(FPR rs2, uint32_t offset, GPR rs1) noexcept;

    void FABS_D(FPR rd, FPR rs) noexcept;
    void FMV_D(FPR rd, FPR rs) noexcept;
    void FNEG_D(FPR rd, FPR rs) noexcept;

    // RV64D Extension Instructions

    void FCVT_L_D(GPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_LU_D(GPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_D_L(FPR rd, GPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_D_LU(FPR rd, GPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FMV_D_X(FPR rd, GPR rs1) noexcept;
    void FMV_X_D(GPR rd, FPR rs1) noexcept;

    // RV32Q Extension Instructions

    void FADD_Q(FPR rd, FPR rs1, FPR rs2, RMode rmode = RMode::DYN) noexcept;
    void FCLASS_Q(GPR rd, FPR rs1) noexcept;
    void FCVT_Q_W(FPR rd, GPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_Q_WU(FPR rd, GPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_W_Q(GPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_WU_Q(GPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_Q_D(FPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_D_Q(FPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_Q_S(FPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_S_Q(FPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FDIV_Q(FPR rd, FPR rs1, FPR rs2, RMode rmode = RMode::DYN) noexcept;
    void FEQ_Q(GPR rd, FPR rs1, FPR rs2) noexcept;
    void FLE_Q(GPR rd, FPR rs1, FPR rs2) noexcept;
    void FLT_Q(GPR rd, FPR rs1, FPR rs2) noexcept;
    void FLQ(FPR rd, uint32_t offset, GPR rs) noexcept;
    void FMADD_Q(FPR rd, FPR rs1, FPR rs2, FPR rs3, RMode rmode = RMode::DYN) noexcept;
    void FMAX_Q(FPR rd, FPR rs1, FPR rs2) noexcept;
    void FMIN_Q(FPR rd, FPR rs1, FPR rs2) noexcept;
    void FMSUB_Q(FPR rd, FPR rs1, FPR rs2, FPR rs3, RMode rmode = RMode::DYN) noexcept;
    void FMUL_Q(FPR rd, FPR rs1, FPR rs2, RMode rmode = RMode::DYN) noexcept;
    void FNMADD_Q(FPR rd, FPR rs1, FPR rs2, FPR rs3, RMode rmode = RMode::DYN) noexcept;
    void FNMSUB_Q(FPR rd, FPR rs1, FPR rs2, FPR rs3, RMode rmode = RMode::DYN) noexcept;
    void FSGNJ_Q(FPR rd, FPR rs1, FPR rs2) noexcept;
    void FSGNJN_Q(FPR rd, FPR rs1, FPR rs2) noexcept;
    void FSGNJX_Q(FPR rd, FPR rs1, FPR rs2) noexcept;
    void FSQRT_Q(FPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FSUB_Q(FPR rd, FPR rs1, FPR rs2, RMode rmode = RMode::DYN) noexcept;
    void FSQ(FPR rs2, uint32_t offset, GPR rs1) noexcept;

    void FABS_Q(FPR rd, FPR rs) noexcept;
    void FMV_Q(FPR rd, FPR rs) noexcept;
    void FNEG_Q(FPR rd, FPR rs) noexcept;

    // RV64Q Extension Instructions

    void FCVT_L_Q(GPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_LU_Q(GPR rd, FPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_Q_L(FPR rd, GPR rs1, RMode rmode = RMode::DYN) noexcept;
    void FCVT_Q_LU(FPR rd, GPR rs1, RMode rmode = RMode::DYN) noexcept;

    // RVC Extension Instructions

    void C_ADDI(GPR rd, int32_t imm) noexcept;
    void C_ADDIW(GPR rd, int32_t imm) noexcept;
    void C_ADDI4SPN(GPR rd, uint32_t imm) noexcept;
    void C_ADDI16SP(int32_t imm) noexcept;
    void C_FLD(FPR rd, uint32_t imm, GPR rs) noexcept;
    void C_FLW(FPR rd, uint32_t imm, GPR rs) noexcept;
    void C_FSD(FPR rs2, uint32_t imm, GPR rs1) noexcept;
    void C_FSW(FPR rs2, uint32_t imm, GPR rs1) noexcept;
    void C_J(int32_t offset) noexcept;
    void C_J(Label* label) noexcept;
    void C_JAL(Label* label) noexcept;
    void C_JAL(int32_t offset) noexcept;
    void C_LD(GPR rd, uint32_t imm, GPR rs) noexcept;
    void C_LI(GPR rd, int32_t imm) noexcept;
    void C_LQ(GPR rd, uint32_t imm, GPR rs) noexcept;
    void C_LUI(GPR rd, uint32_t imm) noexcept;
    void C_LW(GPR rd, uint32_t imm, GPR rs) noexcept;
    void C_NOP() noexcept;
    void C_SD(GPR rs2, uint32_t imm, GPR rs1) noexcept;
    void C_SQ(GPR rs2, uint32_t imm, GPR rs1) noexcept;
    void C_SRAI(GPR rd, uint32_t shift) noexcept;
    void C_SRLI(GPR rd, uint32_t shift) noexcept;
    void C_SW(GPR rs2, uint32_t imm, GPR rs1) noexcept;
    void C_UNDEF() noexcept;

private:
    // Emits an atomic instruction.
    void EmitAtomic(uint32_t funct5, Ordering ordering, GPR rs2, GPR rs1, uint32_t funct3, GPR rd, uint32_t opcode) noexcept;

    // Emits a B type RISC-V instruction. These consist of:
    // imm[12|10:5] | rs2 | rs1 | funct3 | imm[4:1] | imm[11] | opcode
    void EmitBType(uint32_t imm, GPR rs2, GPR rs1, uint32_t funct3, uint32_t opcode) noexcept;

    // Emits a I type RISC-V instruction. These consist of:
    // imm[11:0] | rs1 | funct3 | rd | opcode
    void EmitIType(uint32_t imm, Register rs1, uint32_t funct3, Register rd, uint32_t opcode) noexcept;

    // Emits a J type RISC-V instruction. These consist of:
    // imm[20|10:1|11|19:12] | rd | opcode
    void EmitJType(uint32_t imm, GPR rd, uint32_t opcode) noexcept;

    // Emits a R type RISC instruction. These consist of:
    // funct7 | rs2 | rs1 | funct3 | rd | opcode
    void EmitRType(uint32_t funct7, Register rs2, Register rs1, uint32_t funct3, Register rd, uint32_t opcode) noexcept;

    // Emits a R type RISC instruction. These consist of:
    // funct7 | rs2 | rs1 | funct3 | rd | opcode
    void EmitRType(uint32_t funct7, FPR rs2, FPR rs1, RMode funct3, FPR rd, uint32_t opcode) noexcept;

    // Emits a R4 type RISC instruction. These consist of:
    // rs3 | funct2 | rs2 | rs1 | funct3 | rd | opcode
    void EmitR4Type(FPR rs3, uint32_t funct2, FPR rs2, FPR rs1, RMode funct3, FPR rd, uint32_t opcode) noexcept;

    // Emits a S type RISC-V instruction. These consist of:
    // imm[11:5] | rs2 | rs1 | funct3 | imm[4:0] | opcode
    void EmitSType(uint32_t imm, Register rs2, GPR rs1, uint32_t funct3, uint32_t opcode) noexcept;

    // Emits a U type RISC-V instruction. These consist of:
    // imm[31:12] | rd | opcode
    void EmitUType(uint32_t imm, GPR rd, uint32_t opcode) noexcept;

    // Emits a fence instruction
    void EmitFENCE(uint32_t fm, FenceOrder pred, FenceOrder succ, GPR rs, uint32_t funct3, GPR rd, uint32_t opcode) noexcept;

    // Emits a compressed jump instruction. These consist of:
    // funct3 | imm | op
    void EmitCompressedJump(uint32_t funct3, int32_t offset, uint32_t op) noexcept;

    // Emits a compress immediate instruction. These consist of:
    // funct3 | imm | rd | imm | op
    void EmitCompressedImmediate(uint32_t funct3, uint32_t imm, GPR rd, uint32_t op) noexcept;

    // Emits a compressed load instruction. These consist of:
    // funct3 | imm | rs1 | imm | rd | op
    void EmitCompressedLoad(uint32_t funct3, uint32_t imm, GPR rs, Register rd, uint32_t op) noexcept;

    // Emits a compressed store instruction. These consist of:
    // funct3 | imm | rs1 | imm | rs2 | op
    void EmitCompressedStore(uint32_t funct3, uint32_t imm, GPR rs1, Register rs2, uint32_t op) noexcept;

    // Emits a compressed wide immediate instruction. These consist of:
    // funct3 | imm | rd | opcode
    void EmitCompressedWideImmediate(uint32_t funct3, uint32_t imm, GPR rd, uint32_t op) noexcept;

    // Binds a label to a given offset.
    void BindToOffset(Label* label, Label::LocationOffset offset);

    // Links the given label and returns the offset to it.
    ptrdiff_t LinkAndGetOffset(Label* label);

    // Resolves all label offsets and patches any necessary
    // branch offsets into the branch instructions that
    // requires them.
    void ResolveLabelOffsets(Label* label);

    CodeBuffer m_buffer;
};

} // namespace biscuit
