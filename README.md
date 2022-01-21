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
## Amplitude based pedals

### Distortion
```
- gain (gain intensity)
- soft_clip (clip threshold, soft threshold, softener)
- mix (balance)
```

### Fuzz
```
- gain (gain intensity)
- hardclip (clip threshold)
- overlap_lastfrequency (height, speed)
- mix (balance)
```

### Bitcrusher (resolution)
```
- reduce_resolution (reduction intensity)
- mix (balance)
```

### Bitcrusher (rate)
```
- reduce_rate (reduction intensity)
- mix (balance)
```

### Tremolo
```
- multiply_by_LFO (speed)
- mix (balance)
```

### LPF
```
- low_pass_filter (width)
```

### Amplifier
```
- gain (gain intensity)
```

## Time based pedals
```
...
```

### Delay/echo
```
...
```

### Phaser
```
...
```

### Reverb
```
...
```

### Octaver
```
...
```

## NOTES
Hardclipper is always present at the end of the DSP, in order to make sure that the DAC receives admitabble values.