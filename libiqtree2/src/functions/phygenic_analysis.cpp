#include "libiqtree2_functions.h"
#include <pybind11/pybind11.h>
#include <functional> 
#include <sstream>
#include <iostream>



// callback stream buffer to redirect std::cout to a passed in callback function
class CallbackStreambuf : public std::streambuf {
public:
    CallbackStreambuf(std::function<void(const std::string&)> callback)
        : callback_(callback) {}

protected:
    virtual std::streamsize xsputn(const char* s, std::streamsize n) {
        if (callback_) { // Check if the callback is not null
            std::string str(s, n);
            pybind11::gil_scoped_acquire acquire; // Acquire the Python Global Interpreter Lock (GIL)
            callback_(str);
        }
        return n;
    }

    virtual int overflow(int c) {
        if (callback_) { // Check if the callback is not null
            char ch = c;
            pybind11::gil_scoped_acquire acquire; // Acquire the GIL
            callback_(std::string(&ch, 1));
        }
        return c;
    }

private:
    std::function<void(const std::string&)> callback_;
};

std::string phylogenic_analysis(
    const std::string& alignment, 
    const std::string& partition, 
    const std::string& model, 
    const std::string& initial_tree,
    std::function<void(const std::string)> log_callback) {

    // Redirect std::cout to the callback function only if it's not null
    CallbackStreambuf* callback_buf = nullptr;
    std::streambuf* old_cout_buf = nullptr;
    if (log_callback) {
        callback_buf = new CallbackStreambuf(log_callback);
        old_cout_buf = std::cout.rdbuf(callback_buf);
    }

    // ... perform analysis writing out log messages to std::cout ...
    std::cout << "sample log message\n";

    // Restore the original std::cout buffer
    if (log_callback) {
        std::cout.rdbuf(old_cout_buf);
        delete callback_buf;
    }

    return "Analysis results";
}