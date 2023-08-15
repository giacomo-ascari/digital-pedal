import os
import numpy as np
import matplotlib.pyplot as plt
import scipy.fftpack
from scipy.io import wavfile
import wave, struct, math, random

def main():

    output_file = "code.txt"

    if os.path.exists(output_file):
        os.remove(output_file)
    
    total_samples = 0

    for filename in os.listdir("wav"):
        print(filename)
        #with open(os.path.join(os.cwd(), filename), 'r') as f:

        # read wav and get samples
        x = []
        fps, data = wavfile.read(os.path.join("wav", filename))
        for idx, sample in enumerate(data):
            #print(type(int(sample)))
            x.append(int(sample))

        print("raw", str(len(x)))

        # remove the last zeroes
        removing = True
        for i in range(len(x)-1, 0-1, -1):
            if x[i] > 0 or x[i] < 0:
                removing = False
            if removing:
                x.pop()

        print("cut", str(len(x)))
        
        # compact two values in a 16bit sample
        compacting = True
        c = []
        for i in range(len(x)-1, 0-1, -2):

            if x[i] > 127 or x[i] < -128 or x[i-1] > 127 or x[i-1] < -128:
                compacting = False
            if compacting:
                c.insert(0, x[i])
                c.insert(0, x[i-1])
                x.pop()
                x.pop()
        for i in range(0, len(c), 2):
            x.append(c[i] * 256 + c[i+1])
        print("comp", str(len(x)))
        total_samples += len(x)

        with open(output_file, "a") as f:
            f.write("const int16_t " + filename[:-4] + "[" + str(len(x)) + "] = { ")
            for i in range(len(x)):
                s = x[i]
                if i == len(x) - 1:
                    f.write(str(int(s)) + "};\n")
                elif i % 2000 == 0 and i != 0:
                    f.write(str(int(s)) + ",\n\t")
                else:
                    f.write(str(int(s)) + ", ")

    print("total samples", total_samples)
    print("kB", total_samples * 2 / 1024)
    

if __name__ == "__main__":
    main()