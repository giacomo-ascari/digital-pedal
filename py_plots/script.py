import math
import numpy as np
import matplotlib.pyplot as plt
import scipy.fftpack


N = 500 # samplepoints
START = 0
STOP = 2*np.pi

A = 1 # amplitude
W = 1 # pulsation
P = 0 # phase
B = 12 # steps per unit

x = np.linspace(START, STOP, N)
y0 = A*np.sin(x * W + P)
y1 = np.floor(A*np.sin(x * W + P)*B)/B

plt.title("Sample quantization")
plt.plot(x, y0)
plt.plot(x, y1)
plt.show()



# ====================
#        FFT
# ====================

# Number of samplepoints
N = 600
# sample spacing
T = 1.0 / 800.0
x = np.linspace(0.0, N*T, N)
y = np.sin(50.0 * 2.0*np.pi*x) + 0.5*np.sin(80.0 * 2.0*np.pi*x)
yf = scipy.fftpack.fft(y)
xf = np.linspace(0.0, 1.0/(2.0*T), N//2)

fig, ax = plt.subplots()
ax.plot(xf, 2.0/N * np.abs(yf[:N//2]))
#plt.show()