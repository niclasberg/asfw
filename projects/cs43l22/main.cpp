#include <board/board.hpp>
#include <board/clock_config.hpp>
#include <drivers/i2c.hpp>
#include <drivers/gpio.hpp>
#include <drivers/dma.hpp>
#include <drivers/cs43l22.hpp>
#include <drivers/vl6180.hpp>
#include <drivers/i2s.hpp>
#include <drivers/adc.hpp>

#include <async/receive.hpp>
#include <async/then.hpp>
#include <async/repeat.hpp>
#include <async/delay.hpp>
#include <async/execute_sync.hpp>
#include <async/on_signal.hpp>

#include <schedulers/cooperative_scheduler.hpp>

#include "Oscillator.h"
#include "Filter.h"
#include "Envelope.h"
#include "InputController.h"

#include <cstring>
#include <cmath>

using namespace drivers;
using namespace schedulers;

constexpr std::uint32_t SAMPLE_FREQ = 48'000;
constexpr std::uint32_t BUFFER_SIZE = 128;
namespace clock = board::clock;

int main()
{
    auto boardDescriptor = board::makeBoard(
        clock::makeClockConfig(
            clock::pllInput(clock::PllSource::HSI, clock::divide<16>),
			clock::systemClock(clock::SystemClockSource::PLL, clock::multiply<336>, clock::divide<2>),
			clock::i2sClock(clock::multiply<192>, clock::divide<2>),
			clock::AhbPrescaler::NO_DIV,
			clock::Apb1Prescaler::DIV4,
			clock::Apb2Prescaler::DIV2));
    auto scheduler = makeCooperativeScheduler(boardDescriptor);

    // Audio buffers
    std::uint16_t audioBuffer0[2*BUFFER_SIZE];
    std::uint16_t audioBuffer1[2*BUFFER_SIZE];
    std::uint16_t * audioBuffers[2] = {audioBuffer0, audioBuffer1};
	std::memset(audioBuffer0, 0, 2*BUFFER_SIZE);
    std::memset(audioBuffer1, 0, 2*BUFFER_SIZE);

    // Setup I2S3 for audio streaming
    // Pins: MCKL (PC7), SCKL (PC10), SerialData (PC12), WordSelect (PA4)
    auto dacI2S = i2s::makeI2sMaster(
        boardDescriptor,
		deviceId<2>, 
		i2s::sampleFreq<SAMPLE_FREQ>,
		i2s::TransferMode::TX_ONLY,
		i2s::MasterClockOut::ON,
        i2s::masterClockPin<2, 7>,
        i2s::serialClockPin<2, 10>,
        i2s::serialDataPin<2, 12>,
        i2s::wordSelectPin<0, 4>);

    auto i2sDma = dma::makeStream(
        boardDescriptor,
		dma::streamId<0, 5>, 
		dma::Channel::CHANNEL0,
		dma::DataSize::BITS16);

    // Setup I2C1 for audio DAC
	// Pins: SCKL (PB6) and SDA (PB9)
    auto i2cDev = i2c::makeI2CMaster(
        boardDescriptor,
        deviceId<0>,
        i2c::serialClockFrequency<100'000>,
        i2c::clockPin<1, 6>,
        i2c::dataPin<1, 9>);
    Cs43l22 dac(i2cDev);

    // Setup I2C3 for Vl6180 range finder
    // Pins: SCKL (PA8), SDA (PC9)
    auto rangeFinderI2c = i2c::makeI2CMaster(
        boardDescriptor,
        deviceId<2>, 
        i2c::serialClockFrequency<100'000>,
        i2c::clockPin<0, 8>,
        i2c::dataPin<2, 9>);
    Vl6180 rangeFinder{rangeFinderI2c};

    // Setup ADC1 for key input
    auto keyInput = adc::makeAdc(
        boardDescriptor,
        deviceId<0>,
        adc::analogPins(
            PIN<2, 1>,
            PIN<2, 2>, PIN<2, 3>,
            PIN<0, 0>, PIN<0, 1>, 
            PIN<0, 2>, PIN<0, 3>, 
            PIN<0, 5>, 
            PIN<0, 7>,
            PIN<2, 4>, PIN<2, 5>,
            PIN<1, 1>));

    auto keyInputDma = dma::makeStream(
        boardDescriptor,
        dma::streamId<1, 0>,
        dma::Channel::CHANNEL0,
        dma::DataSize::BITS16);

    // Bring CS43L22 reset pin (PD4) high to turn on the DAC
	auto dacResetPin = gpio::makeOutputPin(boardDescriptor, PIN<3, 4>);
	dacResetPin.write(true);

    // Initialize DAC and range finder
    auto status = async::executeSync(scheduler, 
        dac.init(),
        rangeFinder.init());

    if (status != async::CompletionStatus::COMPLETED_WITH_VALUE) { for (;;) { } }

    Oscillator oscillator(SAMPLE_FREQ);
    Filter filter{SAMPLE_FREQ};
    Envelope envelope{SAMPLE_FREQ};
    InputController inputController{keyInput, keyInputDma, envelope, oscillator};

    status = async::executeSync(scheduler, 
        dacI2S.writeContinuous(i2sDma, audioBuffers, 2*BUFFER_SIZE)
        | async::onSignal([&](std::uint16_t * buffer) {
            for (int i = 0; i < BUFFER_SIZE; i += 1)
            {
                std::int16_t val = (std::int16_t) (30000.f * 
                    filter(oscillator()) * envelope());
                *(buffer++) = val;
                *(buffer++) = val;
            }
        }),

        rangeFinder.readRangeContinuous()
        | async::onSignal([&filter](std::uint8_t range) {
            filter.setCutoff(800.f * float(range) / 255.f + 10.f);
        }),

        async::repeat(
            async::delay(inputController.update(), 5))
    );

    for (;;) {}
}


extern "C" // Prevent name mangling
{
	void SystemInit(void)
	{
		/* Enable fpu */
		*((volatile uint32_t *) 0xE000ED88) |= ((3UL << 10*2)|(3UL << 11*2));  // set CP10 and CP11 Full Access
	}
}
