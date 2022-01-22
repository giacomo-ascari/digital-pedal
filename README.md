# DIGITAL PEDAL - G33KY-TOAD
This project aims at the creation of a multi-effect stompbox.
```
        ┌─┐_┌───┐    ┌─────┐    ┌───────┐    ┌─────┐       /│   \
┌┴┴┐════┴═════  │    │ ADC │    │  MCU  │    │ DAC │    ┌─┤ │  \ \
└┬┬┘════┬═════  │ -> │     │ -> │ [DSP] │ -> │     │ -> └─┤ │  / /
        └─┘‾└───┘    └─────┘    └───────┘    └─────┘       \│   /
                     └─────────────────────────────┘
                                G33KY-TOAD
                             [DIGITAL-PEDAL]
```

## Hardware
The system will have a STM32F407VTG6 at it's core, with its 168Mhz clock and 192kB of memory.
ADC? Still don't know.
DAC? Probably one already on the STM32F4 MCU.

## Pedals
|           Name          |             DSP             | Amplitude | Time/ frequency | Implemented |
|:-----------------------:|:---------------------------:|:---------:|:---------------:|:-----------:|
| Overdrive               | Gain > soft clip            |     X     |                 |      X      |
| Fuzz                    | Gain > hard clip > sum wave |     X     |                 |      X      |
| Bitcrusher (resolution) | Reduce sample resolution    |     X     |                 |      X      |
| Bitcrusher (rate)       | Reduce sampling rate        |           |        X        |      X      |
| Tremolo                 | Multiply by LFO             |     X     |                 |      X      |
| Low pass filter         |                             |           |        X        |      X      |
| Amplifier               | Gain                        |     X     |                 |      X      |
| Balancer                | Mixer                       |     X     |                 |      X      |
| Compressor              |                             |     X     |        X        |             |
| Delay/echo              |                             |           |        X        |             |
| Vibrato                 |                             |           |        X        |             |
| Phaser                  |                             |           |        X        |             |
| Reverb                  |                             |           |        X        |             |
| Octaver                 |                             |           |        X        |             |
| Chorus                  |                             |           |        X        |             |
