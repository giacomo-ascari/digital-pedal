from scipy.io import wavfile
import wave, struct, math, random
from matplotlib import pyplot as plt
import numpy as np
import subprocess

def process(samples, types):
    with open("samples-in.tmp", "w") as f:
        for s in samples:
            f.write(str(s) + "\n")
    subprocess.run(['/bin/sh', './compile.sh'])
    args = ['./out', "samples-in.tmp", "samples-out.tmp"]
    for t in types:
        args.append(t)
    subprocess.run(args)
    proc_samples = []
    with open("samples-out.tmp", "r") as f:
        for line in f.readlines():
            proc_samples.append(int(line))
    return proc_samples

def main():

    samples = read("sound-in.wav")
    print("Reading completed")
    
    proc_samples = process(samples, ["damp"])
    print("Processing completed")

    write(proc_samples, "sound-out.wav")
    print("Write completed")

    interval_lower, interval_upper = int(48000*9), int(48000*9.2)
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