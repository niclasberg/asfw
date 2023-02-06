#pragma once
#include "types.hpp"
#include "async/event.hpp"

namespace board
{
    namespace detail
    {
        // To be defined elsewhere...
        async::EventEmitter getInterruptEvent(int);
    }

    struct InterruptControl
    {
        static void enableIRQs()
        {
            //asm volatile ("cpsie i" : : : "memory");
        }

        static void disableIRQs()
        {
            //asm volatile ("cpsid i" : : : "memory");
        }
    };

    namespace Interrupts
    {
        constexpr auto SysTick                = Interrupt<-1>{ };    /*!< SysTick Interrupt                                         */
        constexpr auto WWDG                   = Interrupt<0>{ };     /*!< Window WatchDog Interrupt                                         */
        constexpr auto PVD                    = Interrupt<1>{ };     /*!< PVD through EXTI Line detection Interrupt                         */
        constexpr auto TAMP_STAMP             = Interrupt<2>{ };     /*!< Tamper and TimeStamp interrupts through the EXTI line             */
        constexpr auto RTC_WKUP               = Interrupt<3>{ };     /*!< RTC Wakeup interrupt through the EXTI line                        */
        constexpr auto FLASH                  = Interrupt<4>{ };     /*!< FLASH global Interrupt                                            */
        constexpr auto RCC                    = Interrupt<5>{ };     /*!< RCC global Interrupt                                              */
        constexpr auto EXTI0                  = Interrupt<6>{ };     /*!< EXTI Line0 Interrupt                                              */
        constexpr auto EXTI1                  = Interrupt<7>{ };     /*!< EXTI Line1 Interrupt                                              */
        constexpr auto EXTI2                  = Interrupt<8>{ };     /*!< EXTI Line2 Interrupt                                              */
        constexpr auto EXTI3                  = Interrupt<9>{ };     /*!< EXTI Line3 Interrupt                                              */
        constexpr auto EXTI4                  = Interrupt<10>{ };     /*!< EXTI Line4 Interrupt                                              */
        constexpr auto DMA1_Stream0           = Interrupt<11>{ };     /*!< DMA1 Stream 0 global Interrupt                                    */
        constexpr auto DMA1_Stream1           = Interrupt<12>{ };     /*!< DMA1 Stream 1 global Interrupt                                    */
        constexpr auto DMA1_Stream2           = Interrupt<13>{ };     /*!< DMA1 Stream 2 global Interrupt                                    */
        constexpr auto DMA1_Stream3           = Interrupt<14>{ };     /*!< DMA1 Stream 3 global Interrupt                                    */
        constexpr auto DMA1_Stream4           = Interrupt<15>{ };     /*!< DMA1 Stream 4 global Interrupt                                    */
        constexpr auto DMA1_Stream5           = Interrupt<16>{ };     /*!< DMA1 Stream 5 global Interrupt                                    */
        constexpr auto DMA1_Stream6           = Interrupt<17>{ };     /*!< DMA1 Stream 6 global Interrupt                                    */
        constexpr auto ADC                    = Interrupt<18>{ };     /*!< ADC1, ADC2 and ADC3 global Interrupts                             */
        constexpr auto CAN1_TX                = Interrupt<19>{ };     /*!< CAN1 TX Interrupt                                                 */
        constexpr auto CAN1_RX0               = Interrupt<20>{ };     /*!< CAN1 RX0 Interrupt                                                */
        constexpr auto CAN1_RX1               = Interrupt<21>{ };     /*!< CAN1 RX1 Interrupt                                                */
        constexpr auto CAN1_SCE               = Interrupt<22>{ };     /*!< CAN1 SCE Interrupt                                                */
        constexpr auto EXTI9_5                = Interrupt<23>{ };     /*!< External Line[9:5] Interrupts                                     */
        constexpr auto TIM1_BRK_TIM9          = Interrupt<24>{ };     /*!< TIM1 Break interrupt and TIM9 global interrupt                    */
        constexpr auto TIM1_UP_TIM10          = Interrupt<25>{ };     /*!< TIM1 Update Interrupt and TIM10 global interrupt                  */
        constexpr auto TIM1_TRG_COM_TIM11     = Interrupt<26>{ };     /*!< TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
        constexpr auto TIM1_CC                = Interrupt<27>{ };     /*!< TIM1 Capture Compare Interrupt                                    */
        constexpr auto TIM2                   = Interrupt<28>{ };     /*!< TIM2 global Interrupt                                             */
        constexpr auto TIM3                   = Interrupt<29>{ };     /*!< TIM3 global Interrupt                                             */
        constexpr auto TIM4                   = Interrupt<30>{ };     /*!< TIM4 global Interrupt                                             */
        constexpr auto I2C1_EV                = Interrupt<31>{ };     /*!< I2C1 Event Interrupt                                              */
        constexpr auto I2C1_ER                = Interrupt<32>{ };     /*!< I2C1 Error Interrupt                                              */
        constexpr auto I2C2_EV                = Interrupt<33>{ };     /*!< I2C2 Event Interrupt                                              */
        constexpr auto I2C2_ER                = Interrupt<34>{ };     /*!< I2C2 Error Interrupt                                              */
        constexpr auto SPI1                   = Interrupt<35>{ };     /*!< SPI1 global Interrupt                                             */
        constexpr auto SPI2                   = Interrupt<36>{ };     /*!< SPI2 global Interrupt                                             */
        constexpr auto USART1                 = Interrupt<37>{ };     /*!< USART1 global Interrupt                                           */
        constexpr auto USART2                 = Interrupt<38>{ };     /*!< USART2 global Interrupt                                           */
        constexpr auto USART3                 = Interrupt<39>{ };     /*!< USART3 global Interrupt                                           */
        constexpr auto EXTI15_10              = Interrupt<40>{ };     /*!< External Line[15:10] Interrupts                                   */
        constexpr auto RTC_Alarm              = Interrupt<41>{ };     /*!< RTC Alarm (A and B) through EXTI Line Interrupt                   */
        constexpr auto OTG_FS_WKUP            = Interrupt<42>{ };     /*!< USB OTG FS Wakeup through EXTI line interrupt                     */
        constexpr auto TIM8_BRK_TIM12         = Interrupt<43>{ };     /*!< TIM8 Break Interrupt and TIM12 global interrupt                   */
        constexpr auto TIM8_UP_TIM13          = Interrupt<44>{ };     /*!< TIM8 Update Interrupt and TIM13 global interrupt                  */
        constexpr auto TIM8_TRG_COM_TIM14     = Interrupt<45>{ };     /*!< TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
        constexpr auto TIM8_CC                = Interrupt<46>{ };     /*!< TIM8 Capture Compare global interrupt                             */
        constexpr auto DMA1_Stream7           = Interrupt<47>{ };     /*!< DMA1 Stream7 Interrupt                                            */
        constexpr auto FSMC                   = Interrupt<48>{ };     /*!< FSMC global Interrupt                                             */
        constexpr auto SDIO                   = Interrupt<49>{ };     /*!< SDIO global Interrupt                                             */
        constexpr auto TIM5                   = Interrupt<50>{ };     /*!< TIM5 global Interrupt                                             */
        constexpr auto SPI3                   = Interrupt<51>{ };     /*!< SPI3 global Interrupt                                             */
        constexpr auto UART4                  = Interrupt<52>{ };     /*!< UART4 global Interrupt                                            */
        constexpr auto UART5                  = Interrupt<53>{ };     /*!< UART5 global Interrupt                                            */
        constexpr auto TIM6_DAC               = Interrupt<54>{ };     /*!< TIM6 global and DAC1&2 underrun error  interrupts                 */
        constexpr auto TIM7                   = Interrupt<55>{ };     /*!< TIM7 global interrupt                                             */
        constexpr auto DMA2_Stream0           = Interrupt<56>{ };     /*!< DMA2 Stream 0 global Interrupt                                    */
        constexpr auto DMA2_Stream1           = Interrupt<57>{ };     /*!< DMA2 Stream 1 global Interrupt                                    */
        constexpr auto DMA2_Stream2           = Interrupt<58>{ };     /*!< DMA2 Stream 2 global Interrupt                                    */
        constexpr auto DMA2_Stream3           = Interrupt<59>{ };     /*!< DMA2 Stream 3 global Interrupt                                    */
        constexpr auto DMA2_Stream4           = Interrupt<60>{ };     /*!< DMA2 Stream 4 global Interrupt                                    */
        constexpr auto ETH                    = Interrupt<61>{ };     /*!< Ethernet global Interrupt                                         */
        constexpr auto ETH_WKUP               = Interrupt<62>{ };     /*!< Ethernet Wakeup through EXTI line Interrupt                       */
        constexpr auto CAN2_TX                = Interrupt<63>{ };     /*!< CAN2 TX Interrupt                                                 */
        constexpr auto CAN2_RX0               = Interrupt<64>{ };     /*!< CAN2 RX0 Interrupt                                                */
        constexpr auto CAN2_RX1               = Interrupt<65>{ };     /*!< CAN2 RX1 Interrupt                                                */
        constexpr auto CAN2_SCE               = Interrupt<66>{ };     /*!< CAN2 SCE Interrupt                                                */
        constexpr auto OTG_FS                 = Interrupt<67>{ };     /*!< USB OTG FS global Interrupt                                       */
        constexpr auto DMA2_Stream5           = Interrupt<68>{ };     /*!< DMA2 Stream 5 global interrupt                                    */
        constexpr auto DMA2_Stream6           = Interrupt<69>{ };     /*!< DMA2 Stream 6 global interrupt                                    */
        constexpr auto DMA2_Stream7           = Interrupt<70>{ };     /*!< DMA2 Stream 7 global interrupt                                    */
        constexpr auto USART6                 = Interrupt<71>{ };     /*!< USART6 global interrupt                                           */
        constexpr auto I2C3_EV                = Interrupt<72>{ };     /*!< I2C3 event interrupt                                              */
        constexpr auto I2C3_ER                = Interrupt<73>{ };     /*!< I2C3 error interrupt                                              */
        constexpr auto OTG_HS_EP1_OUT         = Interrupt<74>{ };     /*!< USB OTG HS End Point 1 Out global interrupt                       */
        constexpr auto OTG_HS_EP1_IN          = Interrupt<75>{ };     /*!< USB OTG HS End Point 1 In global interrupt                        */
        constexpr auto OTG_HS_WKUP            = Interrupt<76>{ };     /*!< USB OTG HS Wakeup through EXTI interrupt                          */
        constexpr auto OTG_HS                 = Interrupt<77>{ };     /*!< USB OTG HS global interrupt                                       */
        constexpr auto DCMI                   = Interrupt<78>{ };     /*!< DCMI global interrupt                                             */
        constexpr auto RNG                    = Interrupt<80>{ };     /*!< RNG global Interrupt                                              */
        constexpr auto FPU                    = Interrupt<81>{ };     /*!< FPU global interrupt                                               */
    }
}
