#usage python3 plot.py [sample_freq]

import matplotlib.pyplot as plt
import math 
import sys

def cyclic_shift(y):
  y_1 = y[:len(y)//2]
  y_2 = y[len(y)//2:]

  return y_2 + y_1

def conv(x):
  x_r   = [0] * len(x)
  x_i   = [0] * len(x)
  x_abs = [0] * len(x)

  for i in range(len(x)):
    tmp = x[i].strip() # strip return char
    tmp = tmp.split(' + i') # split re and im
    x_r[i] = float(tmp[0])
    x_i[i] = float(tmp[1])
    x_abs[i] = round(math.sqrt(x_r[i] * x_r[i] + x_i[i] * x_i[i]), 5)
  
  return x_r, x_i, x_abs

# MAIN

sample_freq = 1

if len(sys.argv) >= 2:
    sample_freq = int(sys.argv[1])
print("Sampling freq.: " + str(sample_freq))

# Read signals from files (assumption: one sample per second)
x_file = open("input.txt", "r")
x = x_file.readlines()
x_r, x_i, x_abs = conv(x)

y_file = open("output.txt", "r")
y = y_file.readlines()
y = cyclic_shift(y)
y_r, y_i, y_abs = conv(y)

# Define x axis
samples = range(len(x))

time = [x / sample_freq for x in samples]

freq = [0] * len(time) 
for t in range(len(time) // 2):
  f = (samples[t] * sample_freq / float(len(time)))
  freq[    t + len(time) // 2] = f
  freq[-t -1 + len(time) // 2] = -f

# Plot
fig, axs = plt.subplots(1,2)

axs[0].plot(time, x_abs)
axs[0].set_title('abs(x[n])')
axs[0].set(xlabel='s')

axs[1].plot(freq, y_abs)
axs[1].set_title('abs(y[n])')
axs[1].set(xlabel='Hz')

plt.show()
