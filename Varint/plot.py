import matplotlib.pyplot as plt
import pandas as pd


data = pd.read_csv('probs.txt', sep=' ', header=None).values
plt.plot(data[:, 0], data[:, 1])
plt.xscale('log')
# plt.yscale('log')
plt.show()
