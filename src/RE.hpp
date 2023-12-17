#ifndef RUNTIMEERROR
#define RUNTIMEERROR

#define TRE "throw RuntimeError(\"RuntimeError\")"

#include <exception>
#include <string>

class RuntimeError : std::exception 
{
private:
    std :: string s;
public:
    RuntimeError(std :: string);
    std :: string message() const;
};

#endif