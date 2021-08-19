
#include "ErrorReporter.h"


ErrorReporter::ErrorReporter(std::ostream &error_stream) : err(error_stream) {}

void ErrorReporter::error(Position pos, const std::string &msg)
{
    report(pos, msg, "Error");
}

void ErrorReporter::warn(Position pos, const std::string &msg)
{
    report(pos, msg, "Warning");
}

void ErrorReporter::report(Position pos, const std::string &msg, const std::string &prefix)
{
    err << prefix << " at position " << pos << ": " << msg << std::endl;
}
