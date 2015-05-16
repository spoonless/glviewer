#include "PathArg.hpp"

namespace sys
{

template<> void Argument<Path>::reset(Path &path)
{
    path = "";
}

template<> OperationResult Argument<Path>::convert(Path &path, char const *v)
{
    path = v;
    return OperationResult::succeeded();
}

OperationResult PathArg::convert(char const *value)
{
    OperationResult result = Argument::convert(value);
    if(result && _pathResolver)
    {
        _pathResolver(_value);
    }
    return result;
}

void PathArg::pathResolver(std::function<void(Path &)> resolver)
{
    _pathResolver = resolver;
}

void PathArg::pathResolver(PathResolver &pathResolver)
{
    _pathResolver = [&pathResolver](Path &path){pathResolver(path);};
}

void PathResolver::pushParent(const Path &path)
{
    _parentPaths.push_back(path);
}

void PathResolver::popParent()
{
    if (!_parentPaths.empty())
    {
        _parentPaths.pop_back();
    }
}

void PathResolver::operator() (Path &path)
{
    if(!_parentPaths.empty())
    {
        path = Path(_parentPaths.back(), path);
    }
}

}
