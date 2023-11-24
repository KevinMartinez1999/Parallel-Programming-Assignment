import numpy as np
import matplotlib.pyplot as plt
import os

try:
    # Carga los datos de los archivos de salida
    a = np.loadtxt("../database/sequential.txt")
    b = np.loadtxt("../database/coarsegrain.txt")
    c = np.loadtxt("../database/finegrain.txt")
    d = np.loadtxt("../database/python.txt")
except:
    print("No se encontraron los archivos de salida")
    exit()

nhilos = np.arange(1, 33)

# Verifica si existe la carpeta figures, si no existe la crea
if not os.path.exists("figures"):
    os.makedirs("figures")

# Grafica los datos
plt.figure(figsize=(10, 4))
plt.plot(1, a, label="Secuencial", color="black", marker="o", linestyle="",
         markerfacecolor="none", zorder=3, clip_on=False)
plt.hlines(a, 1, 32, color="black", linestyle="--", zorder=2, clip_on=False)
plt.plot(nhilos, b, label="Grano grueso", color="blue", marker="^", 
         markerfacecolor="none", zorder=3, clip_on=False)
plt.plot(nhilos, c, label="Grano fino", color="red", marker="s",
            markerfacecolor="none", zorder=3, clip_on=False)
plt.plot(nhilos, d, label="Gano grueso Python", color="green", marker="D",
            markerfacecolor="none", zorder=3, clip_on=False)
plt.legend(loc="best")
plt.xlabel("Número de hilos")
plt.ylabel("Tiempo de ejecución [ms]")
plt.xticks(np.arange(1, 33))
plt.yticks(np.arange(250, 471, 20))
plt.xlim([1, 32])
plt.ylim([250, 470])
plt.grid(zorder=0, clip_on=False, linestyle="--", color="gray", alpha=0.5)
plt.savefig("figures/plot.png", format="png", bbox_inches="tight", pad_inches=0)
