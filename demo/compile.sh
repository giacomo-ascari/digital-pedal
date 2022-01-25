gcc -o out \
    ../pedalboard-lib/main.c \
    ../pedalboard-lib/pedalboard.c \
    ../pedalboard-lib/pedals/amplifier/amplifier.c \
    ../pedalboard-lib/pedals/bitcrusher-rs/bitcrusher-rs.c \
    ../pedalboard-lib/pedals/bypass/bypass.c \
    ../pedalboard-lib/pedals/fuzz/fuzz.c \
    ../pedalboard-lib/pedals/low-pass-filter/low-pass-filter.c \
    ../pedalboard-lib/pedals/overdrive/overdrive.c \
    ../pedalboard-lib/pedals/tremolo/tremolo.c
