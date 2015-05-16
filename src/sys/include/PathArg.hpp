#ifndef PATH_ARG_HPP
#define PATH_ARG_HPP

#include <functional>
#include "Argument.hpp"
#include <list>
#include "Path.hpp"

namespace sys
{

class PathResolver
{
public:
    void operator() (Path &path);
    void pushParent(const Path &path);
    void popParent();
private:
    std::list<Path> _parentPaths;
};

class PathArg : public Argument<Path>
{
public:
    OperationResult convert(char const *value) override;
    void pathResolver(std::function<void(Path &)> resolver);
    void pathResolver(PathResolver &pathResolver);

private:
    std::function<void(Path &)> _pathResolver;
};

template<> void Argument<Path>::reset(Path &path);
template<> OperationResult Argument<Path>::convert(Path &path, char const *v);

}

#endif
