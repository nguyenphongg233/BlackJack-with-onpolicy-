import numpy as np
import matplotlib.pyplot as plt
import subprocess

# Chạy dealer.exe trong cùng thư mục với file Python
subprocess.run(["dealer.exe"])

data = np.loadtxt('dealer.out', delimiter=',', skiprows=1)  

x, y, xx, yy = data.T

plt.subplot(1,2,1)
plt.scatter(x, y)
plt.grid()
plt.xlabel("Dealer's first card")
plt.ylabel("Player's current point")
plt.title("No usable ACE")

plt.subplot(1,2,2)
plt.scatter(xx, yy)
plt.grid()
plt.xlabel("Dealer's first card")
plt.ylabel("Player's current point")
plt.title("usable ACE")

plt.show()
