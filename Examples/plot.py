import matplotlib.pyplot as plt
import math 

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

# Read signals from files
x_file = open("input.txt", "r")
x = x_file.readlines()
x_r, x_i, x_abs = conv(x)

y_file = open("output.txt", "r")
y = y_file.readlines()
y_r, y_i, y_abs = conv(y)

# Plot
fig, axs = plt.subplots(6, sharex=True)

axs[0].plot(x_r)
axs[1].plot(x_i)
axs[2].plot(x_abs)
axs[0].set_title('re(x[n])')
axs[1].set_title('im(x[n])')
axs[2].set_title('abs(x[n])')

axs[3].plot(y_r)
axs[4].plot(y_i)
axs[5].plot(y_abs)
axs[3].set_title('re(y[n])')
axs[4].set_title('im(y[n])')
axs[5].set_title('abs(y[n])')

plt.show()
