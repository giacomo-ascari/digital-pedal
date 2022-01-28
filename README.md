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
<p align="center">
  <img src="https://github.com/giacomo-ascari/digital-pedal/blob/main/logos/logo360x.png?raw=true" />
</p>

## Hardware
The system will have a STM32F407VGT6 at it's core, with its 168Mhz clock and 192kB of memory.
ADC? Still don't know.
DAC? Probably one already on the STM32F4 MCU.

## Pedals
|           Name          |     Family     |             DSP             | Amplitude | Time/ frequency | Implemented |
|:-----------------------:|:--------------:|:---------------------------:|:---------:|:---------------:|:-----------:|
| Amplifier               | Utilities      | Gain > hard clip            |     X     |                 |      X      |
| Bypass                  | Utilities      | Bypass signal (default)     |           |                 |      X      |
| Bitcrusher (resolution) | Distortion     | Reduce sample resolution    |     X     |                 |      X      |
| Dynamic amplifier       | Utilities      | Gain > hard clip            |     X     |        X        |     WIP     |
| Fuzz                    | Distortion     | Gain > hard clip > sum wave |     X     |                 |      X      |
| Low pass filter         | Filter         | Low pass filter             |           |        X        |      X      |
| Overdrive               | Distortion     | Gain > soft clip            |     X     |                 |      X      |
| Overdrive (log)         | Distortion     | Gain > soft clip            |     X     |                 |      X      |
| Tremolo                 | Distortion     | Multiply by wave            |     X     |                 |      X      |

## Would-be-nice-to-have pedals
|           Name          |     Family     |             DSP             | Amplitude | Time/ frequency |
|:-----------------------:|:--------------:|:---------------------------:|:---------:|:---------------:|
| Bitcrusher (rate)       | Distortion     | Reduce sampling rate        |           |        X        |
| Wah wah                 | Filter         |                             |           |        X        |
| Compressor              | Compression    |                             |     X     |        X        |
| Equalizer               | Equalisation   |                             |     X     |        X        |
| Delay/echo              | Delay Lines    |                             |           |        X        |
| Vibrato                 | Distortion     |                             |           |        X        |
| Flanger                 | Flangers       |                             |           |                 |
| Phaser                  | Phasers        |                             |           |        X        |
| Octaver                 | Pitch Shifting |                             |           |        X        |
| Chorus                  | Chorus         |                             |           |        X        |
