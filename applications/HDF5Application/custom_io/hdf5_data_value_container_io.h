//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 license: HDF5Application/license.txt
//
//  Main author:    Michael Andre, https://github.com/msandre
//

#if !defined(KRATOS_HDF5_DATA_VALUE_CONTAINER_IO_H_INCLUDED)
#define KRATOS_HDF5_DATA_VALUE_CONTAINER_IO_H_INCLUDED

// System includes
#include <string>

// External includes

// Project includes
#include "includes/define.h"
#include "containers/data_value_container.h"

// Application includes
#include "hdf5_application_define.h"
#include "custom_io/hdf5_file.h"

namespace Kratos
{
namespace HDF5
{
namespace Internals
{
///@addtogroup HDF5Application
///@{

void ReadDataValueContainer(File& rFile, std::string const& rPrefix, DataValueContainer& rData);

void WriteDataValueContainer(File& rFile, std::string const& rPrefix, DataValueContainer const& rData);

///@} addtogroup
} // namespace Internals.
} // namespace HDF5.
} // namespace Kratos.

#endif // KRATOS_HDF5_DATA_VALUE_CONTAINER_IO_H_INCLUDED defined
