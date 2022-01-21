# DIGITAL PEDAL
This project aims at the creation of a multi-effect stompbox.
```
        ┌─┐_┌───┐    ┌─────┐    ┌───────┐    ┌─────┐       /│   \
┌┴┴┐════┴═════  │ -→ │ ADC │ -→ │  MCU  │ -→ │ DAC │ -→ ┌─┤ │  \ \
└┬┬┘════┬═════  │    │     │    │ [DSP] │    │     │    └─┤ │  / /
        └─┘‾└───┘    └─────┘    └───────┘    └─────┘       \│   /
                     └─────────────────────────────┘
                               DIGITAL-PEDAL
```
## Amplitude based pedals

### Distortion
```
- gain
- soft_clip
```

### Fuzz
```
- gain
- hardclip
- overlap_lastfrequency
```

### Bitcrusher (resolution)
```
- scale_resolution
```

### Bitcrusher (rate)
```
- scale_rate
```

### Tremolo
```
- multiply_by_LFO
```

### LPF
```
- low_pass_filter
```

### Amplifier
```
- gain
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