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
        STATIC_REQUIRE(Clock::getSystemClockFrequency() == 16'000'000);
    }

    SECTION("System clock frequency (HSI) - pllInput configured")
    {
        using Clock = decltype(
            clock::makeClockConfig(
                clock::systemClock(clock::SystemClockSource::HSI),
                clock::pllInput(clock::PllSource::HSI, clock::divide<8>)));
        STATIC_REQUIRE(Clock::getSystemClockFrequency() == 16'000'000);
    }

    SECTION("System clock frequency (HSE)")
    {
        using Clock = decltype(
            clock::makeClockConfig(
                clock::systemClock(clock::SystemClockSource::HSE)));
        STATIC_REQUIRE(Clock::getSystemClockFrequency() == 8'000'000);
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
        STATIC_REQUIRE(Clock::getSystemClockFrequency() == 96'000'000);
        STATIC_REQUIRE(Clock::getApb1ClockFrequency() == 24'000'000);
        STATIC_REQUIRE(Clock::getApb2ClockFrequency() == 48'000'000);
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
        STATIC_REQUIRE(Clock::getSystemClockFrequency() == 60'000'000);
    }

    SECTION("I2S clock frequency")
    {
        using Clock = decltype(
            clock::makeClockConfig(
                clock::pllInput(clock::PllSource::HSI, clock::divide<8>),
                clock::i2sClock(clock::multiply<192>, clock::divide<4>)));
        // 16 / 8 * 192 / 4 = 96
        STATIC_REQUIRE(Clock::getI2SClockFrequency() == 96'000'000);
    }
    
    SECTION("Ahb clock frequency (HSI / 4)")
    {
        using Clock = decltype(
            clock::makeClockConfig(
                clock::systemClock(clock::SystemClockSource::HSI), 
                clock::AhbPrescaler::DIV4));
        STATIC_REQUIRE(Clock::getAhbClockFrequency() == 4'000'000);
    }

    SECTION("Apb1 clock frequency (HSI / 4 / 2)")
    {
        using Clock = decltype(
            clock::makeClockConfig(
                clock::systemClock(clock::SystemClockSource::HSI),
                clock::AhbPrescaler::DIV4, 
                clock::Apb1Prescaler::DIV2,
                clock::Apb2Prescaler::DIV8));
        STATIC_REQUIRE(Clock::getApb1ClockFrequency() == 2'000'000);
        STATIC_REQUIRE(Clock::getApb2ClockFrequency() == 500'000);
    }
}