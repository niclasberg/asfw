# Introduction
A library implementing drivers as composable, asynchronous operations for the stm32f407 microcontroller. The foundation build on the [executors proposal](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p0443r14.html) for c++23.

# TODO
- Containers
    - Result/expected
- Bind
    - Handle the monadic bind for `ManySender<Sender>` 
- Add transformDone algorithm
- Add transformError algorithm
- WhenAll
    - Make it work for non-void senders
- Spi
    - Stop token support for read/write
    - DMA read/write support
- I2S
    - Initialization
    - Reuse algorithms developed for spi
- I2C 
    - Slave implementation
