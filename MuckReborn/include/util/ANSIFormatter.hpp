#ifndef ASNI_FORMATTER_HPP
#define ASNI_FORMATTER_HPP

#include <fmt/core.h>
#include <string>

namespace ANSIFormatter
{
    std::string replaceAll(std::string str, const std::string& from, const std::string& to)
    {
        size_t start_pos = 0;

        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }

        return str;
    }

    template<typename... Args>
    std::string format(std::string format, Args&&... args)
    {
        format = fmt::format(format, args...);

        format = replaceAll(format, "&f", "\033[1;37m");
        format = replaceAll(format, "&r", "\033[0m");
        format = replaceAll(format, "&0", "\033[1;30m");
        format = replaceAll(format, "&1", "\033[1;34m");
        format = replaceAll(format, "&2", "\033[1;32m");
        format = replaceAll(format, "&3", "\033[1;36m");
        format = replaceAll(format, "&4", "\033[1;31m");
        format = replaceAll(format, "&5", "\033[1;35m");
        format = replaceAll(format, "&6", "\033[1;33m");

        return format;
    }

    std::string deFormat(std::string format)
    {
        format = replaceAll(format, "\033[1;37m", "");
        format = replaceAll(format, "\033[0m", "");
        format = replaceAll(format, "\033[1;30m", "");
        format = replaceAll(format, "\033[1;34m", "");
        format = replaceAll(format, "\033[1;32m", "");
        format = replaceAll(format, "\033[1;36m", "");
        format = replaceAll(format, "\033[1;31m", "");
        format = replaceAll(format, "\033[1;35m", "");
        format = replaceAll(format, "\033[1;33m", "");

        return format;
    }
}

#endif // !ASNI_FORMATTER_HPP