#include <types.hpp>
#include <async/event.hpp>
#include <board/interrupts.hpp>
#include <array>

std::array<async::Event, 128> events = {};

async::EventEmitter board::detail::getInterruptEvent(int irqNo)
{
    return async::EventEmitter{&events[irqNo+1]};
}

extern "C" void Default_Handler(void);

#define MAKE_IRQ_HANDLER(NAME) \
    extern "C" void NAME##_IRQHandler(void) \
    { events[hana::value(board::Interrupts::NAME)+1].raise(); }

extern "C" void SysTick_Handler()
{
    events[0].raise();
}

MAKE_IRQ_HANDLER(WWDG);
MAKE_IRQ_HANDLER(PVD);
MAKE_IRQ_HANDLER(TAMP_STAMP);
MAKE_IRQ_HANDLER(RTC_WKUP);
MAKE_IRQ_HANDLER(FLASH);
MAKE_IRQ_HANDLER(RCC);
MAKE_IRQ_HANDLER(EXTI0);
MAKE_IRQ_HANDLER(EXTI1);
MAKE_IRQ_HANDLER(EXTI2);
MAKE_IRQ_HANDLER(EXTI3);
MAKE_IRQ_HANDLER(EXTI4);
MAKE_IRQ_HANDLER(DMA1_Stream0);
MAKE_IRQ_HANDLER(DMA1_Stream1);
MAKE_IRQ_HANDLER(DMA1_Stream2);
MAKE_IRQ_HANDLER(DMA1_Stream3);
MAKE_IRQ_HANDLER(DMA1_Stream4);
MAKE_IRQ_HANDLER(DMA1_Stream5);
MAKE_IRQ_HANDLER(DMA1_Stream6);
MAKE_IRQ_HANDLER(ADC);
MAKE_IRQ_HANDLER(CAN1_TX);
MAKE_IRQ_HANDLER(CAN1_RX0);
MAKE_IRQ_HANDLER(CAN1_RX1);
MAKE_IRQ_HANDLER(CAN1_SCE);
MAKE_IRQ_HANDLER(EXTI9_5);
MAKE_IRQ_HANDLER(TIM1_BRK_TIM9);
MAKE_IRQ_HANDLER(TIM1_UP_TIM10);
MAKE_IRQ_HANDLER(TIM1_TRG_COM_TIM11);
MAKE_IRQ_HANDLER(TIM1_CC);
MAKE_IRQ_HANDLER(TIM2);
MAKE_IRQ_HANDLER(TIM3);
MAKE_IRQ_HANDLER(TIM4);
MAKE_IRQ_HANDLER(I2C1_EV);
MAKE_IRQ_HANDLER(I2C1_ER);
MAKE_IRQ_HANDLER(I2C2_EV);
MAKE_IRQ_HANDLER(I2C2_ER);
MAKE_IRQ_HANDLER(SPI1);
MAKE_IRQ_HANDLER(SPI2);
MAKE_IRQ_HANDLER(USART1);
MAKE_IRQ_HANDLER(USART2);
MAKE_IRQ_HANDLER(USART3);
MAKE_IRQ_HANDLER(EXTI15_10);
MAKE_IRQ_HANDLER(RTC_Alarm);
MAKE_IRQ_HANDLER(OTG_FS_WKUP);
MAKE_IRQ_HANDLER(TIM8_BRK_TIM12);
MAKE_IRQ_HANDLER(TIM8_UP_TIM13);
MAKE_IRQ_HANDLER(TIM8_TRG_COM_TIM14);
MAKE_IRQ_HANDLER(TIM8_CC);
MAKE_IRQ_HANDLER(DMA1_Stream7);
MAKE_IRQ_HANDLER(FSMC);
MAKE_IRQ_HANDLER(SDIO);
MAKE_IRQ_HANDLER(TIM5);
MAKE_IRQ_HANDLER(SPI3);
MAKE_IRQ_HANDLER(UART4);
MAKE_IRQ_HANDLER(UART5);
MAKE_IRQ_HANDLER(TIM6_DAC);
MAKE_IRQ_HANDLER(TIM7);
MAKE_IRQ_HANDLER(DMA2_Stream0);
MAKE_IRQ_HANDLER(DMA2_Stream1);
MAKE_IRQ_HANDLER(DMA2_Stream2);
MAKE_IRQ_HANDLER(DMA2_Stream3);
MAKE_IRQ_HANDLER(DMA2_Stream4);
MAKE_IRQ_HANDLER(ETH);
MAKE_IRQ_HANDLER(ETH_WKUP);
MAKE_IRQ_HANDLER(CAN2_TX);
MAKE_IRQ_HANDLER(CAN2_RX0);
MAKE_IRQ_HANDLER(CAN2_RX1);
MAKE_IRQ_HANDLER(CAN2_SCE);
MAKE_IRQ_HANDLER(OTG_FS);
MAKE_IRQ_HANDLER(DMA2_Stream5);
MAKE_IRQ_HANDLER(DMA2_Stream6);
MAKE_IRQ_HANDLER(DMA2_Stream7);
MAKE_IRQ_HANDLER(USART6);
MAKE_IRQ_HANDLER(I2C3_EV);
MAKE_IRQ_HANDLER(I2C3_ER);
MAKE_IRQ_HANDLER(OTG_HS_EP1_OUT);
MAKE_IRQ_HANDLER(OTG_HS_EP1_IN);
MAKE_IRQ_HANDLER(OTG_HS_WKUP);
MAKE_IRQ_HANDLER(OTG_HS);
MAKE_IRQ_HANDLER(DCMI);
MAKE_IRQ_HANDLER(RNG);
MAKE_IRQ_HANDLER(FPU);

#undef MAKE_IRQ_HANDLER