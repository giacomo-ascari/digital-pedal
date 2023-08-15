import os
import numpy as np
import matplotlib.pyplot as plt
import scipy.fftpack
from scipy.io import wavfile
import wave, struct, math, random

def main():

    output_file = "code.txt"

    os.remove(output_file)

    for filename in os.listdir("wav"):
        print(filename)
        #with open(os.path.join(os.cwd(), filename), 'r') as f:

        # read wav and get samples
        samples = []
        fps, data = wavfile.read(os.path.join("wav", filename))
        for idx, sample in enumerate(data):
            samples.append(sample)

        with open(output_file, "a") as f:
            f.write("const int16_t " + filename[:-4] + "[" + str(len(samples)) + "] = { ")
            for i in range(len(samples)):
                s = samples[i]
                if i == len(samples) - 1:
                    f.write(str(int(s)) + "};\n")
                elif i % 2000 == 0 and i != 0:
                    f.write(str(int(s)) + ",\n\t")
                else:
                    f.write(str(int(s)) + ", ")

    

if __name__ == "__main__":
    main()