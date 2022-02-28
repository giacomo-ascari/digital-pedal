from scipy.io import wavfile
import wave, struct, math, random
from matplotlib import pyplot as plt
import numpy as np
import subprocess
import os

def process(samples, types):
    with open("demo/samples-in.tmp", "w") as f:
        for s in samples:
            f.write(str(s) + "\n")
    args = ['./demo/out', "demo/samples-in.tmp", "demo/samples-out.tmp"]
    for t in types:
        args.append(t)
    subprocess.run(args)
    proc_samples = []
    with open("demo/samples-out.tmp", "r") as f:
        for line in f.readlines():
            proc_samples.append(int(line))
    return proc_samples

def main():

    print("BUILDING LIBRARIES...", end=" ")
    subprocess.run(['python3', './builder.py'])
    print("DONE")

    print("COMPILING SOURCE CODE...", end=" ")
    subprocess.run(['/bin/sh', './compile.sh'])
    print("DONE")

    print("READING RAW SAMPLES .wav...", end=" ")
    samples = read("demo/sound-in.wav")
    print("DONE")

    print("PROCESSING RAW SAMPLES...", end=" ")
    proc_samples = process(samples, ["trm"])
    print("DONE")
    
    print("WRITING PROCESSED SAMPLES .wav...", end=" ")
    write(proc_samples, "demo/sound-out.wav")
    print("DONE")

    print("CLEANING UP...", end=" ")
    if os.path.exists("demo/samples-in.tmp"):
        os.remove("demo/samples-in.tmp")
    if os.path.exists("demo/samples-out.tmp"):
        os.remove("demo/samples-out.tmp")
    print("DONE")


    #interval_lower, interval_upper = int(48000*48), int(48000*49.2)
    interval_lower, interval_upper = int(48000*9), int(48000*9.2)
    #interval_lower, interval_upper = int(48000*8), int(48000*11)
    plt.title("yeehaw") 
    plt.xlabel("time") 
    plt.ylabel("sample")
    plt.axhline(y=0, color='lightgrey')
    plt.axhline(y=32767, color='lightgrey')
    plt.axhline(y=-32768, color='lightgrey')
    plt.plot(samples[interval_lower:interval_upper], label="Raw")
    plt.plot(proc_samples[interval_lower:interval_upper])
    plt.show()

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
        data = struct.pack('<h', s)
        obj.writeframesraw(data)
    obj.close()

if __name__ == "__main__":
    main()