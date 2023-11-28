#include "trace.hpp"

trace::trace()
    : transitions(std::list<transition *>()), states(std::list<state *>())
{
}

trace::~trace()
{
    for (auto t : this->transitions)
    {
        delete t;
    }
    for (auto s : this->states)
    {
        delete s;
    }
}
