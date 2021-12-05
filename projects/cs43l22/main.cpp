#include <board/board.hpp>
#include <board/clock_config.hpp>
#include <drivers/i2c.hpp>
#include <drivers/gpio.hpp>
#include <drivers/dma.hpp>
#include <async/receive.hpp>
#include <async/then.hpp>
#include <async/repeat.hpp>
#include <drivers/cs43l22.hpp>
#include <schedulers/cooperative_scheduler.hpp>
#include <async/execute_sync.hpp>
#include <drivers/i2s.hpp>
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
            clock::pllInput(clock::PllSource::HSI, clock::divide<8>),
			clock::systemClock(clock::SystemClockSource::PLL, clock::multiply<192>, clock::divide<4>),
			clock::i2sClock(clock::multiply<192>, clock::divide<4>),
			clock::AhbPrescaler::NO_DIV,
			clock::Apb1Prescaler::DIV4,
			clock::Apb2Prescaler::DIV2));
    auto scheduler = makeCooperativeScheduler(boardDescriptor);

    // Audio buffers
    uint16_t audioBuffer[2*BUFFER_SIZE];
	std::memset(audioBuffer, 0, 2*BUFFER_SIZE);
    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        float amp = std::sin((2.f * i * M_PI) / float(BUFFER_SIZE));
        std::int16_t val = 4000 * amp;

        // Convert to 2s complement
        std::uint16_t valToSend= *std::launder(reinterpret_cast<std::uint16_t *>(&val));
        audioBuffer[2*i] = val;
        audioBuffer[2*i+1] = val;
    }

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
		dma::DataSize::BITS16,
		dma::Mode::DOUBLE_BUFFER);

    // Setup I2C1 for audio DAC
	// Pins: SCKL (PB6) and SDA (PB9)
    auto i2cDev = i2c::makeI2CMaster(
        boardDescriptor,
        deviceId<0>,
        i2c::serialClockFrequency<100'000>,
        i2c::clockPin<1, 6>,
        i2c::dataPin<1, 9>);

    Cs43l22 dac(i2cDev);

    // Bring CS43L22 reset pin (PD4) high to turn on the DAC
	auto dacResetPin = gpio::makeOutputPin(boardDescriptor, PIN<3, 4>);
	dacResetPin.write(true);

    // Initialize DAC and range finder
    auto status = async::executeSync(scheduler, dac.init());

    if (status != async::ExecutionStatus::COMPLETED_WITH_VALUE)
    {
        for (;;) { }
    }

    /*auto status2 = async::executeSync(scheduler,
        async::repeat(
            dacI2S.write(audioBuffer, 2*BUFFER_SIZE)));*/

    for (;;) 
    { 
        async::executeSync(scheduler,
            dacI2S.write(audioBuffer, 2*BUFFER_SIZE));
    }
}


extern "C" // Prevent name mangling
{
	void SystemInit(void)
	{
		/* Enable fpu */
		*((volatile uint32_t *) 0xE000ED88) |= ((3UL << 10*2)|(3UL << 11*2));  // set CP10 and CP11 Full Access
	}
}
