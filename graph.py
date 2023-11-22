import numpy as np
import matplotlib.pyplot as plt
import os

# Carga los datos de los archivos de salida
a = np.loadtxt("database/salida_seq.txt")
b = np.loadtxt("database/salida_coarsegrain.txt")
c = np.loadtxt("database/salida_finegrain.txt")
d = np.loadtxt("database/salida_python.txt")
nhilos = np.arange(1, 33)

# Verifica si existe la carpeta figures, si no existe la crea
if not os.path.exists("figures"):
    os.makedirs("figures")

# Grafica los datos
plt.figure(figsize=(10, 4))
plt.plot(1, a[0], label="Secuencial", color="black", marker="o", linestyle="",
         markerfacecolor="none", zorder=3, clip_on=False)
plt.hlines(a[0], 1, 32, color="black", linestyle="--", zorder=2, clip_on=False)
plt.plot(nhilos, b, label="Grano grueso", color="blue", marker="^", 
         markerfacecolor="none", zorder=3, clip_on=False)
plt.plot(nhilos, c, label="Grano fino", color="red", marker="s",
            markerfacecolor="none", zorder=3, clip_on=False)
plt.plot(nhilos, d, label="Gano grueso Python", color="green", marker="D",
            markerfacecolor="none", zorder=3, clip_on=False)
plt.legend(loc="center right")
plt.xlabel("Número de hilos")
plt.ylabel("Tiempo de ejecución [ms]")
plt.xticks(np.arange(1, 33))
plt.yticks(np.arange(310, 381, 5))
plt.xlim([1, 32])
plt.ylim([310, 380])
plt.grid(zorder=0, clip_on=False, linestyle="--", color="gray", alpha=0.5)
plt.savefig("figures/plot.png", format="png", bbox_inches="tight", pad_inches=0)
plt.savefig("figures/plot.svg", format="svg", bbox_inches="tight", pad_inches=0)
