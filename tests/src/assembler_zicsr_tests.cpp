#include <catch/catch.hpp>

#include <biscuit/assembler.hpp>

TEST_CASE("CSRRC", "[Zicsr]") {
    using namespace biscuit;

    uint32_t value = 0;
    Assembler as(reinterpret_cast<uint8_t*>(&value), sizeof(value));

    as.CSRRC(x31, Assembler::CSR::Cycle, x15);
    REQUIRE(value == 0xC007BFF3);

    as.RewindBuffer();

    as.CSRRC(x31, Assembler::CSR::FFlags, x15);
    REQUIRE(value == 0x0017BFF3);

    as.RewindBuffer();

    as.CSRRC(x31, Assembler::CSR::FRM, x15);
    REQUIRE(value == 0x0027BFF3);

    as.RewindBuffer();

    as.CSRRC(x31, Assembler::CSR::FCSR, x15);
    REQUIRE(value == 0x0037BFF3);
}

TEST_CASE("CSRRCI", "[Zicsr]") {
    using namespace biscuit;

    uint32_t value = 0;
    Assembler as(reinterpret_cast<uint8_t*>(&value), sizeof(value));

    as.CSRRCI(x31, Assembler::CSR::Cycle, 0);
    REQUIRE(value == 0xC0007FF3);

    as.RewindBuffer();

    as.CSRRCI(x31, Assembler::CSR::FFlags, 0x1F);
    REQUIRE(value == 0x001FFFF3);

    as.RewindBuffer();

    as.CSRRCI(x31, Assembler::CSR::FRM, 0x7);
    REQUIRE(value == 0x0023FFF3);
}

TEST_CASE("CSRRS", "[Zicsr]") {
    using namespace biscuit;

    uint32_t value = 0;
    Assembler as(reinterpret_cast<uint8_t*>(&value), sizeof(value));

    as.CSRRS(x31, Assembler::CSR::Cycle, x15);
    REQUIRE(value == 0xC007AFF3);

    as.RewindBuffer();

    as.CSRRS(x31, Assembler::CSR::FFlags, x15);
    REQUIRE(value == 0x0017AFF3);

    as.RewindBuffer();

    as.CSRRS(x31, Assembler::CSR::FRM, x15);
    REQUIRE(value == 0x0027AFF3);

    as.RewindBuffer();

    as.CSRRS(x31, Assembler::CSR::FCSR, x15);
    REQUIRE(value == 0x0037AFF3);
}

TEST_CASE("CSRRSI", "[Zicsr]") {
    using namespace biscuit;

    uint32_t value = 0;
    Assembler as(reinterpret_cast<uint8_t*>(&value), sizeof(value));

    as.CSRRSI(x31, Assembler::CSR::Cycle, 0);
    REQUIRE(value == 0xC0006FF3);

    as.RewindBuffer();

    as.CSRRSI(x31, Assembler::CSR::FFlags, 0x1F);
    REQUIRE(value == 0x001FEFF3);

    as.RewindBuffer();

    as.CSRRSI(x31, Assembler::CSR::FRM, 0x7);
    REQUIRE(value == 0x0023EFF3);
}

TEST_CASE("CSRRW", "[Zicsr]") {
    using namespace biscuit;

    uint32_t value = 0;
    Assembler as(reinterpret_cast<uint8_t*>(&value), sizeof(value));

    as.CSRRW(x31, Assembler::CSR::Cycle, x15);
    REQUIRE(value == 0xC0079FF3);

    as.RewindBuffer();

    as.CSRRW(x31, Assembler::CSR::FFlags, x15);
    REQUIRE(value == 0x00179FF3);

    as.RewindBuffer();

    as.CSRRW(x31, Assembler::CSR::FRM, x15);
    REQUIRE(value == 0x00279FF3);

    as.RewindBuffer();

    as.CSRRW(x31, Assembler::CSR::FCSR, x15);
    REQUIRE(value == 0x00379FF3);
}

TEST_CASE("CSRRWI", "[Zicsr]") {
    using namespace biscuit;

    uint32_t value = 0;
    Assembler as(reinterpret_cast<uint8_t*>(&value), sizeof(value));

    as.CSRRWI(x31, Assembler::CSR::Cycle, 0);
    REQUIRE(value == 0xC0005FF3);

    as.RewindBuffer();

    as.CSRRWI(x31, Assembler::CSR::FFlags, 0x1F);
    REQUIRE(value == 0x001FDFF3);

    as.RewindBuffer();

    as.CSRRWI(x31, Assembler::CSR::FRM, 0x7);
    REQUIRE(value == 0x0023DFF3);
}
