#include "../catch.hpp"
#include "board/clock_config.hpp"

TEST_CASE("Clock tree configuration")
{
    using namespace board;

    SECTION("System clock frequency (HSI)")
    {
        using Clock = decltype(
            clock::makeClockConfig(
                clock::systemClock(clock::SystemClockSource::HSI)));
        STATIC_REQUIRE(hana::equal(Clock::getSystemClockFrequency(), std::ratio<16000000>{}));
    }

    SECTION("System clock frequency (HSI) - pllInput configured")
    {
        using Clock = decltype(
            clock::makeClockConfig(
                clock::systemClock(clock::SystemClockSource::HSI),
                clock::pllInput(clock::PllSource::HSI, clock::divide<8>)));
        STATIC_REQUIRE(hana::equal(Clock::getSystemClockFrequency(), std::ratio<16000000>{}));
    }

    SECTION("System clock frequency (HSE)")
    {
        using Clock = decltype(
            clock::makeClockConfig(
                clock::systemClock(clock::SystemClockSource::HSE)));
        STATIC_REQUIRE(hana::equal(Clock::getSystemClockFrequency(), std::ratio<8000000>{}));
    }

    SECTION("System clock frequency (PLL)")
    {
        using Clock = decltype(
            clock::makeClockConfig(
                clock::systemClock(clock::SystemClockSource::PLL, clock::multiply<192>, clock::divide<4>),
                clock::pllInput(clock::PllSource::HSE, clock::divide<4>),
                clock::Apb1Prescaler::DIV4,     // Needed to keep  
                clock::Apb2Prescaler::DIV2));   // clock constraints
        
        // 8 / 4 * 192 / 4 = 96
        STATIC_REQUIRE(hana::equal(Clock::getSystemClockFrequency(), std::ratio<96000000>{}));
        STATIC_REQUIRE(hana::equal(Clock::getApb1ClockFrequency(), std::ratio<24000000>{}));
        STATIC_REQUIRE(hana::equal(Clock::getApb2ClockFrequency(), std::ratio<48000000>{}));
    }

    SECTION("System clock frequency (PLL) - non-integer partial result")
    {
        using Clock = decltype(
            clock::makeClockConfig(
                clock::pllInput(clock::PllSource::HSE, clock::divide<6>),
                clock::systemClock(clock::SystemClockSource::PLL, clock::multiply<180>, clock::divide<4>),
                clock::Apb1Prescaler::DIV4,     // Needed to keep  
                clock::Apb2Prescaler::DIV2));   // clock constraints
        // 8 / 6 * 180 / 4 = 60
        STATIC_REQUIRE(hana::equal(Clock::getSystemClockFrequency(), std::ratio<60000000>{}));
    }

    SECTION("I2S clock frequency")
    {
        using Clock = decltype(
            clock::makeClockConfig(
                clock::pllInput(clock::PllSource::HSI, clock::divide<8>),
                clock::i2sClock(clock::multiply<192>, clock::divide<4>)));
        // 16 / 8 * 192 / 4 = 96
        STATIC_REQUIRE(hana::equal(Clock::getI2SClockFrequency(), std::ratio<96000000>{}));
    }
    
    SECTION("Ahb clock frequency (HSI / 4)")
    {
        using Clock = decltype(
            clock::makeClockConfig(
                clock::systemClock(clock::SystemClockSource::HSI), 
                clock::AhbPrescaler::DIV4));
        STATIC_REQUIRE(hana::equal(Clock::getAhbClockFrequency(), std::ratio<4000000>{}));
    }

    SECTION("Apb1 clock frequency (HSI / 4 / 2)")
    {
        using Clock = decltype(
            clock::makeClockConfig(
                clock::systemClock(clock::SystemClockSource::HSI),
                clock::AhbPrescaler::DIV4, 
                clock::Apb1Prescaler::DIV2,
                clock::Apb2Prescaler::DIV8));
        STATIC_REQUIRE(hana::equal(Clock::getApb1ClockFrequency(), std::ratio<2000000>{}));
        STATIC_REQUIRE(hana::equal(Clock::getApb2ClockFrequency(), std::ratio<500000>{}));
    }
}