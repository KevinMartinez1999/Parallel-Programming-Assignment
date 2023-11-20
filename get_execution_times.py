import os
import subprocess
import numpy as np

def ejecutar_comando_n_veces(comando, n):
    for i in range(n):
        # Generar un nombre de archivo único para cada ejecución
        archivo_salida = f"salida_{i}.txt"
        # Ejecutar el comando y redirigir la salida al archivo
        subprocess.run(f"{comando} > {archivo_salida}", shell=True)
        # Leer el contenido del archivo de salida y guardarlo en un arreglo
        with open(archivo_salida, 'r') as file:
            contenido = file.read()
            # Guardar el contenido en un arreglo
            arreglo_salida.append(contenido)
        # Eliminar el archivo de salida
        os.remove(archivo_salida)

# Ejemplo de uso
# comando = "./matmulseq_file "
# comando = "./coarsegrain_mmparallel "
comando = "./finegrain_mmparallel "
n = 200
arreglo_salida = []
aux = []
ejecutar_comando_n_veces("./finegrain_mmparallel 10", 2600)

# A partir de aqui empieza a guardar los datos en un arreglo
for i in range(32):
    ncomando = comando + str(i + 1)
    print(ncomando)
    ejecutar_comando_n_veces(ncomando, n)
    with open("salida.txt", 'w') as file:
        for salida in arreglo_salida:
            file.write(salida)
    a = np.loadtxt("salida.txt")
    aux.append(np.mean(a))
    os.remove("salida.txt")
np.savetxt("salida_finegrain2.txt", aux)
