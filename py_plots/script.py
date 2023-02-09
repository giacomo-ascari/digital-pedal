import math
import numpy as np
import matplotlib.pyplot as plt
import scipy.fftpack


A = 1 # amplitude
W = 1 # pulsation
P = 0 # phase
B = 10 # steps per unit

# PLOT 1

x = np.linspace(0, 2*np.pi, 500)
y0 = A*np.sin(x * W + P)
y1 = np.floor(A*np.sin(x * W + P)*B)/B

fig, ax = plt.subplots()
ax.set(xlabel='x', ylabel='f(x)', title='Sample quantization')
ax.plot(x, y0, label="Original signal", color='#bbbbbb')
ax.plot(x, y1, label="Quantized signal", color='#ff0000')
ax.legend()
plt.show()

# PLOT 2

x0 = np.linspace(0, 2*np.pi, 500)
y0 = A*np.sin(x0 * W + P)
x1 = np.linspace(0, 2*np.pi, int(500/20))
y1 = A*np.sin(x1 * W + P)

fig, ax = plt.subplots()
ax.set(xlabel='x', ylabel='f(x)', title='Time discretization')
ax.plot(x0, y0, label="Original signal", color='#bbbbbb')
ax.stem(x1, y1, label="Discretized signal", linefmt='b', markerfmt='b.', basefmt='--b')
ax.legend()
plt.show()

# PLOT 3

x0 = np.linspace(0, 2*np.pi, 500)
y0 = A*np.sin(x0 * W + P)
x1 = np.linspace(0, 2*np.pi, int(500/20))
y1 = np.floor(A*np.sin(x1 * W + P)*B)/B

fig, ax = plt.subplots()
ax.set(xlabel='x', ylabel='f(x)', title='Sample quantization + Time discretization')
ax.plot(x0, y0, label="Original signal", color='#bbbbbb')
ax.step(x1, y1, label='Converted signal', where='post', color="#00bb00")
ax.legend()
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