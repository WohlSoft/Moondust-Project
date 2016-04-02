#ifndef CSVUtils_H
#define CSVUtils_H

#include <exception>
#include <string>

// prints the explanatory string of an exception. If the exception is nested,
// recurses to print the explanatory of the exception it holds
inline std::string exception_to_pretty_string(const std::exception& e, int level = 0)
{
    std::string prettyString = std::string(level, ' ') + "exception: " + std::string(e.what()) + "\n";
    try {
        std::rethrow_if_nested(e);
    }
    catch (const std::exception& e) {
        prettyString += exception_to_pretty_string(e, level + 1);
    }
    catch (...) {}

    return prettyString;
}

#endif

