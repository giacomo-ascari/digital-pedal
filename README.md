# DIGITAL PEDAL
This project aims at the creation of a multi-effect stompbox.
```
        ┌─┐_┌───┐    ┌─────┐    ┌───────┐    ┌─────┐       /│   \
┌┴┴┐════┴═════  │    │ ADC │    │  MCU  │    │ DAC │    ┌─┤ │  \ \
└┬┬┘════┬═════  │ -→ │     │ -→ │ [DSP] │ -→ │     │ -→ └─┤ │  / /
        └─┘‾└───┘    └─────┘    └───────┘    └─────┘       \│   /
                     └─────────────────────────────┘
                              DIGITAL-PEDAL
```

## Hardware
The system will have a STM32F407VTG6 at it's core, with its 168Mhz clock and 192kB of memory.
ADC? Still don't know.
DAC? Probably one already on the STM32F4 MCU.

## Amplitude based pedals

- ### Distortion/Overdrive
```
- gain (gain intensity)
- soft_clip (clip threshold, soft threshold, softener)
- mix (balance)
```

- ### Fuzz
```
- gain (gain intensity)
- hardclip (clip threshold)
- overlap_lastfrequency (height, speed)
- mix (balance)
```

- ### Bitcrusher (resolution)
```
- reduce_resolution (reduction intensity)
- mix (balance)
```

- ### Bitcrusher (rate)
```
- reduce_rate (reduction intensity)
- mix (balance)
```

- ### Tremolo
```
- multiply_by_lfo (speed)
- mix (balance)
```

- ### LPF
```
- low_pass_filter (width)
```

- ### Amplifier
```
- gain (gain intensity)
```

## Time based pedals
Probably won't ever be implemented. I need more memoryyyyy.

- ### Delay

- ### Echo

- ### Phaser

- ### Reverb

- ### Octaver

- ### Chorus

## NOTES
Hardclipper is always present at the end of the DSP, in order to make sure that the DAC receives admitabble values.