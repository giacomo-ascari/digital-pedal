from scipy.io import wavfile
import wave, struct, math, random
from matplotlib import pyplot as plt
import numpy as np

def main():
    samples = read("sound-in.wav")
    print("Read completed")

    gain(samples, 4)
    hard_clip(samples, 32767)
    #soft_clip(samples, 32767, 24000)
    print("DSP completed")
    
    plt.title("yeehaw") 
    plt.xlabel("time") 
    plt.ylabel("sample") 
    plt.plot(samples[int(48000*9):int(48000*9.1)], "ob") 
    plt.show()

    write(samples, "sound-out.wav")
    print("Write completed")

def gain(samples, g):
    for i in range(len(samples)):
        samples[i] *= g

def hard_clip(samples, thr):
    for i in range(len(samples)):
        samples[i] = min(thr, samples[i])
        samples[i] = max(-thr-1, samples[i])

def soft_clip(samples, thr, mid):
    for i in range(len(samples)):
        if abs(samples[i] * gain) <= mid:
            samples[i] *= gain
        else:
            if samples[i] > 0:
                samples[i] = mid + (thr - mid) * (math.exp(-samples[i]*gain)-1)
            else:
                samples[i] *= gain
            #    samples[i] = -(mid + (thr - mid) * (math.exp(-abs(samples[i])*gain)-1))

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