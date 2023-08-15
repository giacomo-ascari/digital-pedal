import os
import numpy as np
import matplotlib.pyplot as plt
import scipy.fftpack
from scipy.io import wavfile
import wave, struct, math, random

def main():
    
    output_file = "samples.bin"

    if os.path.exists(output_file):
        os.remove(output_file)

    bytes = bytearray()
    sector_size = 4096 #bytes
    
    sectors_count = []
    
    for input_file in os.listdir("samples"):

        print(input_file)
        memorized_bytes = len(bytes)

        # read wav and get samples
        fps, data = wavfile.read(os.path.join("samples", input_file))
        for idx, sample in enumerate(data):
            bytes.append(int(sample).to_bytes(2, "big", signed=True)[0])
            bytes.append(int(sample).to_bytes(2, "big", signed=True)[1])

        # fill to sector size
        while len(bytes) % sector_size != 0:
            bytes.append(0)

        # set number of sectors for i-sample
        sectors_count.append(int((len(bytes) - memorized_bytes) / sector_size))
    
    # create index
    index_bytes = bytearray()
    for s in sectors_count:
        index_bytes.append(s.to_bytes(1, "big", signed=False)[0])
    while len(index_bytes) % sector_size != 0:
        index_bytes.append(0)

    # insert index in main bin
    for i in range(0, len(index_bytes)):
        bytes.insert(i, index_bytes[i])

    # output
    with open(output_file, "wb") as f:
        f.write(bytes)
    

if __name__ == "__main__":
    main()