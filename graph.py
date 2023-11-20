import numpy as np
import matplotlib.pyplot as plt

a = np.loadtxt("database/salida_seq.txt")
b = np.loadtxt("database/salida_coarsegrain.txt")
c = np.loadtxt("database/salida_finegrain.txt")
nhilos = np.arange(1, 33)

plt.figure(figsize=(10, 5))
plt.plot(nhilos, a, label="Secuencial", color="black", marker="d", 
         markerfacecolor="none", zorder=3, clip_on=False)
plt.plot(nhilos, b, label="Grano grueso", color="blue", marker="^", 
         markerfacecolor="none", zorder=3, clip_on=False)
plt.plot(nhilos, c, label="Grano fino", color="red", marker="s",
            markerfacecolor="none", zorder=3, clip_on=False)
plt.legend(fontsize=12, loc="center right")
plt.xlabel("Número de hilos")
plt.ylabel("Tiempo de ejecución [ms]")
plt.xticks(np.arange(1, 33))
plt.xlim([1, 32])
plt.ylim([310, 380])
plt.savefig("database/plot.png", format="png", bbox_inches="tight", pad_inches=0)
plt.savefig("database/plot.svg", format="svg", bbox_inches="tight", pad_inches=0)
