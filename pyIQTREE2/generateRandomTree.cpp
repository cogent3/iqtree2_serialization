#include <alisim.h>
#include <pyutils.h>

/**
 * @brief  Impleemnts PyIQTREE2.generateRandomTree
 * @param  self     - the pyIQTREE instance (a Python object)
 * @param  args     - arguments (a Python object)
 * @param  keywords - keywords (a Python object)
 * @return PyObject* - on success, a python string representing the 
 *                     newick-format representation of the phylogenetic tree
 *                   - on failure, nullptr (PyErr_SetString is called to
 *                     report back to Python what the problem was).
 */
static PyObject* pyIQTREE2_generateRandomTree(PyObject* self, PyObject* args, 
                                            PyObject* keywords)
{
    const char* argument_names[] = {
        "length", nullptr
    };
    int            length            = 0;

    if (!PyArg_ParseTupleAndKeywords(args, keywords, "sOO|iii", 
                                        const_cast<char**>(&argument_names[0]),
                                        &length)) 
    {
        //Parsing failure
        return nullptr;
    }        
    tree_string = generateRandomTree(length);

    if (tree_string.empty()) {
        PyErr_SetString(PyExc_TypeError, complaint.str().c_str());
        return nullptr;
    } else {
        PyObject* tree_result = StringToPythonString(tree_string);
        return tree_result;
    }
}