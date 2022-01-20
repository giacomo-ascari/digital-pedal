from lib2to3.refactor import get_all_fix_names
from scipy.io import wavfile
import wave, struct, math, random

def main():
    samples = read("sound-in.wav")
    print("Read completed")
    distortion_hard(samples)
    print("DSP completed")
    write(samples, "sound-out.wav")
    print("Write completed")

def distortion_hard(samples):
    for i in range(len(samples)):
        samples[i] *= 4 # gain
        samples[i] = min(32767, samples[i]) #clipping
        samples[i] = max(-32767, samples[i]) #clipping

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