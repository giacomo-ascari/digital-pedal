from scipy.io import wavfile
import wave, struct, math, random
from matplotlib import pyplot as plt
import numpy as np
import copy

def main():
    samples = read("sound-in.wav")
    print("Read completed")
    
    raw_samples = copy.copy(samples)

    # DISTORTION (gain)
    '''gain(samples, 4)
    soft_clip(samples, 32767, 24000)'''
    
    # BITCRUSHER (rate)
    '''low_pass_filter(samples, 2)
    scale_rate(samples, 8)'''

    # BITCRUSHER (resolution)
    '''low_pass_filter(samples, 2)
    scale_resolution(samples, 2048)'''

    # FUZZ
    '''gain(samples, 8)
    hard_clip(samples, 32767)
    overlap_freq(samples, 32767, 2000, 3.14159 * 4)
    hard_clip(samples, 32767)
    low_pass_filter(samples, 2)'''

    # TREMOLO
    '''multiply_lfo(samples, 440, 0.3)'''

    print("DSP completed")

    write(samples, "sound-out.wav")
    print("Write completed")

    plt.title("yeehaw") 
    plt.xlabel("time") 
    plt.ylabel("sample") 
    plt.plot(raw_samples[int(48000*9):int(48000*9.1)], "b") 
    plt.plot(samples[int(48000*9):int(48000*9.1)], "r")
    plt.show()

def multiply_lfo(samples, freq, a):
    f = []
    for i in range(len(samples)):
        f.append(math.sin(i * 1 / (3.14159 * 2 * (48000 / freq))))
    for i in range(len(samples)):
        samples[i] *= a * f[i] + (1-a)
        f[i] *= 5000

def overlap_freq(samples, thr, height, width):
    last_rising = [0, 0]
    for i in range(1, len(samples)):

        if samples[i-1] <= 0 and samples[i] > 0:
            last_rising[0] = last_rising[1]
            last_rising[1] = i
            period = last_rising[1] - last_rising[0]
        if samples[i] == thr:
            samples[i] = samples[i] + math.sin((i + period)/width)*height - height
        elif samples[i] == -thr-1:
            samples[i] = samples[i] + math.sin((i + period)/width)*height + height

def low_pass_filter(samples, b):
    for i in range(b-1, len(samples)):
        s = 0
        for j in range(i-b+1, i+1):
            s = s + samples[j]
        samples[i] = s / b

def scale_rate(samples, a):
    for i in range(len(samples)):
        samples[i] = samples[int(i / a) * a]

def scale_resolution(samples, a):
    for i in range(len(samples)):
        samples[i] = int(samples[i] / a) * a

def gain(samples, g):
    for i in range(len(samples)):
        samples[i] *= g

def hard_clip(samples, thr):
    for i in range(len(samples)):
        samples[i] = min(thr, samples[i])
        samples[i] = max(-thr-1, samples[i])

def soft_clip(samples, thr, mid):
    for i in range(len(samples)):
        if samples[i] > mid:
            samples[i] = mid + (samples[i] - mid) / 4
            samples[i] = min(thr, samples[i])
        elif samples[i] < -mid:
            samples[i] = - mid + (mid + samples[i]) / 4
            samples[i] = max(-thr-1, samples[i])

def read(filename):
    samples = []
    fps, data = wavfile.read(filename)
    for idx, sample in enumerate(data):
        samples.append(sample[0])
    return samples

def write(samples, filename):
    sampleRate = 48000.0 # hertz
    obj = wave.open(filename, 'w')
    obj.setnchannels(1) # mono
    obj.setsampwidth(2)
    obj.setframerate(sampleRate)
    for s in samples:
        data = struct.pack('<h', int(s))
        obj.writeframesraw(data)
    obj.close()

if __name__ == "__main__":
    main()