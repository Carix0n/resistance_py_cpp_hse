import resistance
from time import time
import sys
from xml.dom.minidom import parse

#parse the input
doc = parse(sys.argv[1])
id_list = []
for node in doc.getElementsByTagName("net"):
    id_list.append(int(node.getAttribute("id")))
id_list.sort()
N = len(id_list)
inf = float('inf')
matrix = [[0.0 if i == j else inf for i in range(N)] for j in range(N)]

#division by 0 is acceptable
def div(a, b):
    try:
        res = a / b
    except ZeroDivisionError:
        res = float('inf')
    return res

#create a graph and compute connections
for node in doc.getElementsByTagName("resistor") + doc.getElementsByTagName("capactor") + doc.getElementsByTagName("diode"):
    net_from_id = int(node.getAttribute("net_from"))
    net_to_id = int(node.getAttribute("net_to"))
    _resistance = float(node.getAttribute("resistance"))
    _reverse_resistance = float(node.getAttribute("reverse_resistance")) if node.tagName == "diode" else _resistance
    net_from_index = id_list.index(net_from_id)
    net_to_index = id_list.index(net_to_id)
    matrix[net_from_index][net_to_index] =  div(1.0, div(1.0, matrix[net_from_index][net_to_index]) + div(1.0, _resistance))
    matrix[net_to_index][net_from_index] = div(1.0, div(1.0, matrix[net_to_index][net_from_index]) + div(1.0, _reverse_resistance))

matrix_cpp = [row.copy() for row in matrix]

#shortest path between nodes
execution_time = time()
for k in range(N):
    for i in range(N):
        for j in range(N):
            matrix[i][j] = div(1.0, div(1.0, matrix[i][j]) + div(1.0, matrix[i][k] + matrix[k][j]))
python_time = time() - execution_time

#the same using cpp
execution_time = time()
resistance.Floyd_Warshall(matrix_cpp)
cpp_time = time() - execution_time

#output the result                
output = open(sys.argv[2], 'w')
output.writelines(["%.6f," * N % tuple(row) + "\n" for row in matrix])
output.close()

#compare algorithms
print("Ratio python/cpp is %s" % (div(python_time, cpp_time)))
