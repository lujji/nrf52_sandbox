from math import *
import numpy as np
import matplotlib.pyplot as plt

with open("log2.txt", 'r') as f:
    lines = f.readlines()
    values = [i for i in [l.split() for l in lines]]

phis = []
xx = []
yy = []
zz = []
for v in values:
    x, y, z = int(v[0]), int(v[1]), int(v[2])
    xx.append(x)
    yy.append(y)
    #phi = degrees(acos(x/sqrt(x**2 + y**2 + z**2)))
    phi = sqrt(x**2 + y**2)
    phis.append(phi)


# f = plt.subplot(111)
plt.plot(xx, '-', yy, '-')
plt.show()
