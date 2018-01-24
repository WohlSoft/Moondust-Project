#ifndef FMT_FORMAT_NE_H
#define FMT_FORMAT_NE_H

#include <fmt/fmt_format.h>
#include "logger.h"

namespace fmt
{
/*
    Exception-less fmt::format version. Instead of exception, the error message will be logged into file
*/
template <typename... Args>
std::string format_ne(CStringRef format_str, const Args & ... args)
{
    try
    {
        return format(format_str, std::forward<const Args&>(args)...);
    }
    catch(FormatError e)
    {
        std::string out;
        pLogWarning("fmt::format error: Thrown exception [%s] on attempt to process string [%s]",
                    e.what(),
                    format_str.c_str());
        out.append(e.what());
        out.append(" [");
        out.append(format_str.c_str());
        out.push_back(']');
        return out;
    }
    catch(...)
    {
        pLogWarning("fmt::format error: Thrown unknown exception on attempt to process string [%s]", format_str.c_str());
        return "<ERROR OF " + std::string(format_str.c_str()) + ">";
    }
}

}

#endif // FMT_FORMAT_NE_H
