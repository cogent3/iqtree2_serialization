#include "pyutils.h"
#include <string>
#include <sstream>
// #include <alisim.h>

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

extern "C" {
    PyObject* pyIQTREE2_generateRandomTree(PyObject* self, PyObject* args, PyObject* kwargs) 
    {
        const char* argument_names[] = {
            "length", nullptr
        };
        int            length            = 0;

        if (!PyArg_ParseTupleAndKeywords(args, kwargs, "i", 
                                            const_cast<char**>(&argument_names[0]),
                                            &length)) 
        {
            //Parsing failure
            return nullptr;
        }        
        // TODO: call the generateRanmdomTree function in alisim
        std::string tree_string = "((A:0.1,B:0.2):0.3,C:0.4);";

        if (tree_string.empty()) {
            std::ostringstream complaint;
            complaint << "Failed to generate tree string.";
            PyErr_SetString(PyExc_TypeError, complaint.str().c_str());
            return nullptr;
        } else {
            PyObject* tree_result = StringToPythonString(tree_string);
            return tree_result;
        }
    }

}