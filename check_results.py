import numpy as np

check = np.loadtxt("check_data/check.txt")
coarsegrain = np.loadtxt("check_data/result_coarsegrain.txt")
finegrain = np.loadtxt("check_data/result_finegrain.txt")

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
    