

int main()
{
    auto streamAudioData =
        audioStream.writeBufferReady(dma, buffer)
        | transform([](auto & buffer) {

        });

    auto processKeyboardInput = 
        every(1ms)
        | bind(buttons.read(dma, buffer))
        | transform([&](auto values) {

        });
    
    scheduler.execute(
        whenAll(
            std::move(streamAudioData),
            std::move(processKeyboardInput),

        )   
        | catchError()
}