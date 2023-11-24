import numpy as np

# Verifica si los archivos check, coarsegrain y finegrain existen
try:
    check = np.loadtxt("../C_scripts/check_data/check.txt")
    coarsegrain = np.loadtxt("../C_scripts/check_data/result_coarsegrain.txt")
    finegrain = np.loadtxt("../C_scripts/check_data/result_finegrain.txt")
    python = np.loadtxt("../C_scripts/check_data/result_python.txt")
except:
    print("No se encontraron los archivos check, coarsegrain o finegrain")
    exit()

# Verifica si check y coarsegrain son exactamente iguales
if np.array_equal(check, coarsegrain):
    print("Son iguales check y coarsegrain")
else:
    print("No son iguales check y coarsegrain")

# Verifica si check y finegrain son exactamente iguales
if np.array_equal(check, finegrain):
    print("Son iguales check y finegrain")
else:
    print("No son iguales check y finegrain")

# Verifica si check y python son exactamente iguales
if np.array_equal(check, python):
    print("Son iguales check y python")
else:
    print("No son iguales check y python")
