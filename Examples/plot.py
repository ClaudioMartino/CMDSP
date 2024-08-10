import matplotlib.pyplot as plt

x_file = open("input.txt", "r")
x = x_file.read().split()
for i in range(len(x)):
  x[i] = float(x[i])

y_file = open("output.txt", "r")
y = y_file.read().split()
for i in range(len(y)):
  y[i] = float(y[i])

fig, axs = plt.subplots(2)

axs[0].plot(range(len(x)), x)
axs[1].plot(range(len(x)), y)

plt.show()
