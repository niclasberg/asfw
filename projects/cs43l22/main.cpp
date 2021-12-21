#include <board/board.hpp>
#include <board/clock_config.hpp>
#include <drivers/i2c.hpp>
#include <drivers/gpio.hpp>
#include <drivers/dma.hpp>
#include <drivers/cs43l22.hpp>
#include <drivers/i2s.hpp>

#include <async/receive.hpp>
#include <async/then.hpp>
#include <async/repeat.hpp>
#include <async/execute_sync.hpp>

#include <schedulers/cooperative_scheduler.hpp>

#include <cstring>
#include <cmath>

using namespace drivers;
using namespace schedulers;

constexpr std::uint32_t SAMPLE_FREQ = 48'000;
constexpr std::uint32_t BUFFER_SIZE = 128;
namespace clock = board::clock;

struct Oscillator
{
    float phase = 0;
    const float dPhase = 400.f / float(SAMPLE_FREQ);

    float getSample()
    {
        float val = 2.f * phase - 1.f;

        // Polyblep band limiting
        if (phase < dPhase)
        {
            const float t = phase / dPhase;
            val -= 2.f*t - t*t - 1.f;
        }
        else if (t > (1.f - dt))
        {
            const float t = (phase - 1.f) / dPhase + 1.f;
            val -= t*t;
        }

        phase += dPhase;
        if (phase > 1.0f) phase -= 1.f;
        return val;
    }
};

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

    // Bring CS43L22 reset pin (PD4) high to turn on the DAC
	auto dacResetPin = gpio::makeOutputPin(boardDescriptor, PIN<3, 4>);
	dacResetPin.write(true);

    // Initialize DAC and range finder
    auto status = async::executeSync(scheduler, 
        async::sequence(
            dac.init(),
            rangeFinder.init()));

    if (status != async::ExecutionStatus::COMPLETED_WITH_VALUE) { for (;;) { } }

    Oscillator osc{};

    status = async::executeSync(scheduler, 
        dacI2S.writeContinuous(i2sDma, audioBuffers, 2*BUFFER_SIZE)
        | async::transform([&osc](std::uint16_t * buffer) {
            for (int i = 0; i < BUFFER_SIZE; i += 1)
            {
                std::int16_t val = (std::int16_t) (30000.f * osc.getSample());
                *(buffer++) = val;
                *(buffer++) = val;
            }
        }));

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
