#include <vector>
#include <limits>

extern "C" {
#include <Python.h>
}

namespace resistance {	
	typedef std::vector<double>  row_t;
	typedef std::vector<row_t>   matrix_t;
	double divide(double a, double b)
	{
		double result;
		try
		{
			result = a / b;
		}
		catch (...)
		{
			result = std::numeric_limits<double>::max();
		}
		return result;
	}
	static matrix_t Floyd_Warshall(const matrix_t &matrix)
	{
		matrix_t result(matrix);
		size_t size = matrix.size();
		for (size_t k = 0; k < size; ++k)
		{
			for (size_t i = 0; i < size; ++i)
			{
				for (size_t j = 0; j < size; ++j)
				{
					result[i][j] = divide(1.0, divide(1.0, matrix[i][j]) + divide(1.0, matrix[i][k] + matrix[k][j]));
				}
			}
		}
		return result;
	}
}

static resistance::matrix_t pyobject_to_cxx(PyObject * py_matrix)
{
	resistance::matrix_t result;
	result.resize(PyObject_Length(py_matrix));
	for (size_t i = 0; i < result.size(); ++i)
	{
		PyObject * py_row = PyList_GetItem(py_matrix, i);
		resistance::row_t & row = result[i];
		row.resize(PyObject_Length(py_row));
		for (size_t j = 0; j < row.size(); ++j)
		{
			PyObject * py_elem = PyList_GetItem(py_row, j);
			const double elem = PyFloat_AsDouble(py_elem);
			row[j] = elem;
		}
	}
	return result;
}

static PyObject * cxx_to_pyobject(const resistance::matrix_t &matrix)
{
	PyObject * result = PyList_New(matrix.size());
	for (size_t i = 0; i < matrix.size(); ++i) {
		const resistance::row_t & row = matrix[i];
		PyObject * py_row = PyList_New(row.size());
		PyList_SetItem(result, i, py_row);
		for (size_t j = 0; j < row.size(); ++j) {
			const double elem = row[j];
			PyObject * py_elem = PyFloat_FromDouble(elem);
			PyList_SetItem(py_row, j, py_elem);
		}
	}
	return result;
}

static PyObject * resistance_Floyd_Warshall(PyObject *module, PyObject *args)
{
	PyObject * pyM = PyTuple_GetItem(args, 0);

	const resistance::matrix_t cxxM = pyobject_to_cxx(pyM);
	const resistance::matrix_t result = resistance::Floyd_Warshall(cxxM);
	PyObject * py_result = cxx_to_pyobject(result);
	
	return py_result;
}

PyMODINIT_FUNC PyInit_resistance()
{
	static PyMethodDef ModuleMethods[] = {
		{ "Floyd_Warshall", resistance_Floyd_Warshall, METH_VARARGS, "Floyd-Warshall algorithm"},
		{ NULL, NULL, 0, NULL }
	};
	static PyModuleDef ModuleDef = {
		PyModuleDef_HEAD_INIT,
		"resistance",
		"Calculation of resistance",
		-1, ModuleMethods, 
		NULL, NULL, NULL, NULL
	};
	PyObject * module = PyModule_Create(&ModuleDef);
	return module;
}