//     ______     _____ _           ________
//    / ____/___ / ___/(_)___ ___  /  _/ __ |
//   / /   / __ \\__ \/ / __ `__ \ / // / / /
//  / /___/ /_/ /__/ / / / / / / // // /_/ /
//  \____/\____/____/_/_/ /_/ /_/___/\____/
//  Kratos CoSimulationApplication
//
//  License:         BSD License, see license.txt
//
//  Main authors:    Pooyan Dadvand
//                   Philipp Bucher (https://github.com/philbucher)
//

#ifndef CO_SIM_IO_STREAM_SERIALIZER_INCLUDED
#define CO_SIM_IO_STREAM_SERIALIZER_INCLUDED

// System includes

// Project includes
#include "serializer.hpp"

namespace CoSimIO {
namespace Internals {

// This class provides a simpler interface for serialization to a string instead of to a file
// Note that you may not override any load or save method of the Serializer. They are not virtual.
class CO_SIM_IO_API StreamSerializer : public Serializer
{
  public:
    ///this constructor simply wraps the standard Serializer and defines output to basic_iostream
    ///@param rTrace type of serialization to be employed
    explicit StreamSerializer(TraceType const& rTrace=SERIALIZER_NO_TRACE);

    //this constructor generates a standard Serializer AND fills the buffer with the data contained in "data"
    ///@param rData a string contained the data to be used in filling the buffer
    ///@param rTrace type of serialization to be employed
    StreamSerializer(const std::string& rData, TraceType const& rTrace=SERIALIZER_NO_TRACE);

    //get a string representation of the serialized data
    std::string GetStringRepresentation() {
        return ((std::stringstream*)(this->pGetBuffer()))->str();
    }

    /// Assignment operator.
    StreamSerializer& operator=(StreamSerializer const& rOther) = delete;

    /// Copy constructor.
    StreamSerializer(StreamSerializer const& rOther) = delete;
};

} // namespace Internals
} // namespace CoSimIO

#endif // CO_SIM_IO_STREAM_SERIALIZER_INCLUDED
