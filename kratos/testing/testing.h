//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Pooyan Dadvand
//
//

#pragma once

// System includes

// External includes

// Project includes
#include "testing/test_suite.h" // This includes the test_case.h which includes tester.h
#include "includes/checks.h"  // It is almost always necessary. includes the exception
#include "includes/data_communicator.h"
#include "testing/test_skipped_exception.h" // Macros and exception class used to skip tests.

namespace Kratos::Testing 
{

DataCommunicator& GetDefaultDataCommunicator();

}
