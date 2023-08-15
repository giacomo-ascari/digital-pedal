import math
import numpy as np
import matplotlib.pyplot as plt
import scipy.fftpack

# PLOT 1

sig = []
vol = []

SAMPLING_RATE = 48000
SAMPLES_COUNT = 48000

def signals(harm, i):
    sum = 0
    for h in harm:
        sum += h[0]*math.sin(2*math.pi*h[1]*i/SAMPLING_RATE)
    return sum

# create original signal
for i in range(SAMPLES_COUNT):
    if i < SAMPLES_COUNT / 1.55:
        sig.append(signals([(-17000, 50), (1000, 100), (2400, 33), (10000, 47)], i))
    else:
        sig.append(0)

# process signal
last_derivative = 0
curr_derivative = 0
curr_volume = 0
vol.append(0)
for i in range(1, SAMPLES_COUNT):
    if abs(sig[i]) >= curr_volume:
        curr_volume = abs(sig[i])
    else:
        curr_derivative = sig[i] - sig[i-1]
        if curr_derivative <= 0 and last_derivative > 0:
            curr_volume = sig[i]
        elif curr_derivative >= 0 and last_derivative < 0:
            curr_volume = -sig[i]
    vol.append(curr_volume)
    last_derivative = curr_derivative


# plot
fig, ax = plt.subplots()
ax.set(xlabel='t', ylabel='eee', title='gniii')
ax.plot(sig, label="Signal", color='#bbbbbb')
ax.plot(vol, label="Volume", color='#ff0000')
ax.legend()
plt.show()
