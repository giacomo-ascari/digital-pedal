# Commands
- header: uint8
- subheader: uint8
- payload: union
  - bytes: uint8[256]
  - shorts: int16[128]
  - ints: int32[64]
  - floats: floats[64]
  - chars: char[256]

Note to myself
- Request: from f103 to f407
- Response: from f407 to f103

## Preview
Commands sent while f103's menu is in PREVIEW page.
- header = 0
### First Request
- subheader = 0
- payload = empty
### First Response
- subheader = PEDAL_INDEX
- payload = PEDAL_CONFIG

## Plot
Commands sent while f103's menu is in PLOT page.
- header = 1
### First Request
- subheader = 0
- payload[0] = X_SCALE
- payload[1] = Y_SCALE
### First Response
- subheader = 0
- payload = PLOT_INPUT_OUTPUT
### Periodic Request
- subheader = 1
- payload[0] = X_SCALE
- payload[1] = Y_SCALE
### Periodic Response
- subheader = 1
- payload = PLOT_INPUT_OUTPUT

## Edit
Commands sent while f103's menu is in EDIT page.
- header = 2
### First Request
- subheader = 0
- payload = empty
### First Response
- subheader = PEDAL_INDEX
- payload = PEDAL_CONFIG
### User Request
- subheader = 2
- payload = PEDAL_CONFIG
- payload[PEDAL_SIZE] = PEDAL_INDEX
### User Response
- subheader = 2
- payload = empty

## Mode
Commands sent while f103's menu is in MODE page.
- header = 3
### First Request
- subheader = 0
- payload = empty
### First Response
- subheader = 0
- payload[0] = ACTIVE_MODE
### User Request
- subheader = 2
- payload[0] = SELECTED_MODE
### User Response
- subheader = 2
- payload[0] = ACTIVE_MODE
