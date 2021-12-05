#include "../catch.hpp"
#include "drivers/dma.hpp"
#include "peripheral_types.hpp"
#include "../mocks/mock_peripheral.hpp"
#include "../mocks/mock_board.hpp"
#include "async/event.hpp"
#include "async/receive.hpp"
#include "reg/read.hpp"

using MockDma = MockPeripheral<board::dma::tag>;

namespace {
    volatile async::Event interruptEvent;
}

struct Peripherals
{
    constexpr MockDma getPeripheral(PeripheralTypes::tags::Dma<0>) const { return {}; }
    constexpr MockDma getPeripheral(PeripheralTypes::tags::Dma<1>) const { return {}; }

    async::EventEmitter getInterruptEvent(decltype(board::Interrupts::DMA1_Stream0)) { return {&interruptEvent}; }
    async::EventEmitter getInterruptEvent(decltype(board::Interrupts::DMA1_Stream1)) { return {&interruptEvent}; }
    async::EventEmitter getInterruptEvent(decltype(board::Interrupts::DMA1_Stream2)) { return {&interruptEvent}; }
    async::EventEmitter getInterruptEvent(decltype(board::Interrupts::DMA1_Stream3)) { return {&interruptEvent}; }
    async::EventEmitter getInterruptEvent(decltype(board::Interrupts::DMA1_Stream4)) { return {&interruptEvent}; }
    async::EventEmitter getInterruptEvent(decltype(board::Interrupts::DMA1_Stream5)) { return {&interruptEvent}; }
    async::EventEmitter getInterruptEvent(decltype(board::Interrupts::DMA1_Stream6)) { return {&interruptEvent}; }
    async::EventEmitter getInterruptEvent(decltype(board::Interrupts::DMA1_Stream7)) { return {&interruptEvent}; }
};

using namespace hana::literals;

TEST_CASE("Dma")
{
    MockDma mockDma;
    resetPeripheral(mockDma);
    auto mockBoard = makeMockBoard(mockPeripherals<Peripherals>);

    using namespace drivers;
    SECTION("Init defaults")
    {
        dma::makeStream(mockBoard, dma::DMA_STREAM<0,1>);

        REQUIRE(reg::read(MockDma{}, board::dma::CR::CHSEL[1_c]) == 0);
        REQUIRE(reg::read(MockDma{}, board::dma::CR::PL[1_c]) == 0);
        REQUIRE(reg::read(MockDma{}, board::dma::CR::PSIZE[1_c]) == 0);
        REQUIRE(!bitIsSet(MockDma{}, board::dma::CR::DBM[1_c]));
        REQUIRE(!bitIsSet(MockDma{}, board::dma::CR::CIRC[1_c]));
        REQUIRE(reg::read(MockDma{}, board::dma::CR::DIR[1_c]) == 0);
    }

    SECTION("Init with 8 bit data size")
    {
        dma::makeStream(mockBoard, dma::DMA_STREAM<0,3>, dma::DataSize::BITS8);
        REQUIRE(reg::read(MockDma{}, board::dma::CR::PSIZE[3_c]) == 0);
    }

    SECTION("Init with 16 bit data size")
    {
        dma::makeStream(mockBoard, dma::DMA_STREAM<0, 4>, dma::DataSize::BITS16);
        REQUIRE(reg::read(MockDma{}, board::dma::CR::PSIZE[4_c]) == 1);
    }

    SECTION("Init with 32 bit data size")
    {
        dma::makeStream(mockBoard, dma::DMA_STREAM<0, 5>, dma::DataSize::BITS32);
        REQUIRE(reg::read(MockDma{}, board::dma::CR::PSIZE[5_c]) == 2);
    }

    SECTION("Init in normal mode")
    {
        dma::makeStream(mockBoard, dma::DMA_STREAM<0, 0>, dma::Mode::NORMAL);
        REQUIRE(!bitIsSet(MockDma{}, board::dma::CR::CIRC[0_c]));
        REQUIRE(!bitIsSet(MockDma{}, board::dma::CR::DBM[0_c]));
    }
    
    SECTION("Init in circular mode")
    {
        dma::makeStream(mockBoard, dma::DMA_STREAM<0, 0>, dma::Mode::CIRCULAR);
        REQUIRE(bitIsSet(MockDma{}, board::dma::CR::CIRC[0_c]));
        REQUIRE(!bitIsSet(MockDma{}, board::dma::CR::DBM[0_c]));
    }

    SECTION("Init in double buffer mode")
    {
        dma::makeStream(mockBoard, dma::DMA_STREAM<0, 0>, dma::Mode::DOUBLE_BUFFER);
        REQUIRE(bitIsSet(MockDma{}, board::dma::CR::CIRC[0_c]));
        REQUIRE(bitIsSet(MockDma{}, board::dma::CR::DBM[0_c]));
    }

    SECTION("Init with non-default options")
    {
        dma::makeStream(mockBoard, dma::DMA_STREAM<0, 7>, 
            dma::Channel::CHANNEL3, 
            dma::Priority::MEDIUM,
            dma::DataSize::BITS32,
            dma::Mode::DOUBLE_BUFFER);

        REQUIRE(reg::read(MockDma{}, board::dma::CR::CHSEL[7_c]) == 3);
        REQUIRE(reg::read(MockDma{}, board::dma::CR::PL[7_c]) == 1);
        REQUIRE(reg::read(MockDma{}, board::dma::CR::PSIZE[7_c]) == 2);
        REQUIRE(bitIsSet(MockDma{}, board::dma::CR::DBM[7_c]));
        REQUIRE(bitIsSet(MockDma{}, board::dma::CR::CIRC[7_c]));
    }

    SECTION("SingleShotRead for memory to peripheral")
    {
        const std::uint32_t srcAddress = 0x1234;
        const std::uint32_t dstAddress = 0x2345;
        const std::uint16_t size = 55;

        bool receivedValue = false;
        auto dmaDev = dma::makeStream(mockBoard, dma::DMA_STREAM<0,0>, dma::Mode::NORMAL);

        auto op = async::connect(
            dmaDev.transfer(dma::MemoryAddress{srcAddress}, dma::PeripheralAddress{dstAddress}, size),
            async::receiveValue([&]() {
                receivedValue = true;
            }));
        async::start(op);

        REQUIRE(reg::bitIsSet(mockDma, board::dma::CR::TCIE[0_c]));
        REQUIRE(reg::bitIsSet(mockDma, board::dma::CR::TEIE[0_c]));
        REQUIRE(reg::read(mockDma, board::dma::M0AR::M0A[0_c]) == srcAddress);
        REQUIRE(reg::read(mockDma, board::dma::PAR::PA[0_c]) == dstAddress);

        /*setRegisterBit(mockDma, board::dma::CR::)
        interruptEvent.raise();

        REQUIRE(receivedValue);*/
    }
}