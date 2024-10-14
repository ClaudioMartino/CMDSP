import argparse
import matplotlib.pyplot as plt
import sys

# line equation, given 2 points (x1,y1) and (x2,y2)
def line(x1, x2, y1, y2, x):
  y = (x-x1) / (x2-x1) * (y2 - y1) + y1
  return y

# Scale x vector between y1 and y2
def scale(x, y1, y2):
  x1 = x[0]
  x2 = x[-1]
  y = [line(x1, x2, y1, y2, xi) for xi in x]
  return y

# t = s / fs -> s = t * fs
def time_to_sample(time, fs):
  return fs * time

def sample_to_time(sample, fs):
  return sample / fs

# f = s * fs / N -> s = f * N / fs
def freq_to_sample(f, fs, N):
  return f * N / fs

def sample_to_freq(sample, fs, N):
  return sample * fs / N

# Convert text to array and return number of samples
def convert(a, fmin, fmax, tmin, tmax, fs):
  # Input: y-axis = time, x-axis = freq
  # Output: y-axis = freq, x-axis = time

  # Split elements
  a = [ai.strip().split() for ai in a]

  n_time = len(a)
  n_freq = len(a[0])
  n_tot = n_time * n_freq

  # Take positive freq. only
  a = [ai[0:len(ai) // 2] for ai in a]
  # Reverse freq
  a = [ai[::-1] for ai in a]

  # Select time
  if(tmax == "max"):
    tmax = n_tot
  else:
    tmax = time_to_sample(tmax, fs)
  tmin = time_to_sample(tmin, fs)

  if(tmin != 0 or tmax != n_time):
    a = a[tmin // n_freq : tmax // n_freq]

  # Convert to float and transpose
  n_time2 = len(a)
  n_freq2 = len(a[0])
  at = [[float(a[j][i]) for j in range(n_time2)] for i in range(n_freq2)]

  print(len(at))
  # Select freq
  if(fmax == "max"):
    fmax = n_freq
  else:
    fmax = int(freq_to_sample(fmax, fs, n_freq))
  fmin = int(freq_to_sample(fmin, fs, n_freq))

  if(fmin != 0 or fmax != n_freq):
    at = at[fmin : fmax]

  return at, n_tot, n_freq

# MAIN

parser = argparse.ArgumentParser(prog='spectrogram', description="Spectrogram. The Y axis corresponding to frequency and the magnitude of each frequency within a given time frame shown by the brightness or colour of the pixels corresponding to that frequency.")
parser.add_argument('-fs', default=1, help="Sample frequency (default: 1)")
parser.add_argument('-t1', default=0, help="Begin time (default: 0)")
parser.add_argument('-t2', default="max", help="End time (default: max)")
parser.add_argument('-f1', default=0, help="Begin frequency (default: 0)")
parser.add_argument('-f2', default="max", help="End frequency (default:max)")
parser.add_argument('-i',  default="nearest", help="Interpolation (default: nearest)")
parser.add_argument('-nt', default=5, help="Number of ticks in time (default: 5)")
parser.add_argument('-nf', default=5, help="Number of ticks in frequency (default: 5)")
args = parser.parse_args()

sample_freq = int(args.fs)
t_bottom = int(args.t1)
t_high = args.t2
if(t_high != "max"):
  t_high = int(t_high)
f_bottom = int(args.f1)
f_high = args.f2
if(f_high != "max"):
  f_high = int(f_high)
interp = args.i
time_ticks_num = int(args.nt)
freq_ticks_num = int(args.nf)

# Size of plot (the only thing that matters is the ratio)
height = 100
width = height * 2;
ext = [0, width, 0, height] # left, right, bottom, top

max_time_ticks_num = width
if(time_ticks_num > max_time_ticks_num):
  print("-nt must be smaller than " + str(max_time_ticks_num))
  exit(1)

max_freq_ticks_num = height
if(freq_ticks_num > max_freq_ticks_num):
  print("-nf must be smaller than " + str(max_freq_ticks_num))
  exit(1)

print("Sampling freq: " + str(sample_freq))
print("t bottom: " + str(t_bottom))
print("t high: " + str(t_high))
print("f bottom: " + str(f_bottom))
print("f high: " + str(f_high))
print("Interpolation: " + str(interp))
print("Time ticks: " + str(time_ticks_num))
print("Freq ticks: " + str(freq_ticks_num))

# Read file
cur_dir = '/'.join((__file__.split('/'))[0:-1])
a_file = open(cur_dir + "/spectrogram.txt", "r")
a = a_file.readlines()

at, time_samples, fft_size = convert(a, f_bottom, f_high, t_bottom, t_high, sample_freq)

# Set max values
t_max = sample_to_time(time_samples, sample_freq)
if(t_high == "max"):
  t_high = t_max
f_max = sample_to_freq(fft_size, sample_freq, fft_size) / 2
if(f_high == "max"):
  f_high = f_max

fig, ax = plt.subplots()
im = ax.imshow(at, cmap='inferno', interpolation=interp, extent=ext)

# Set x axis (time)
time_step = width // time_ticks_num
# vector of width samples from 0 to width-1
time = range(0, width)
ax.set_xticks(time[::time_step])
# vector from o to t max (in sec)
time = scale(time, 0, t_max)
# vector from t_bottom to t_high
time = scale(time, t_bottom, t_high)
time_labels = ['{:3.1f}'.format(t) for t in time]
ax.set_xticklabels(time_labels[::time_step])
ax.set_xlabel("Time (s)")

# Set y axis (frequencies)
freq_step = height // freq_ticks_num
freq = range(0, height)
ax.set_yticks(freq[::freq_step])
freq = scale(freq, 0, f_max)
freq = scale(freq, f_bottom, f_high)
freq_labels = ['{:3.1f}'.format(f) for f in freq]
ax.set_yticklabels(freq_labels[::freq_step])
ax.set_ylabel("Frequency (Hz)")

# Bar
cbar = ax.figure.colorbar(im, ax=ax)

plt.show()
