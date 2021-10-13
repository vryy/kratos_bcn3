//
//  KRATOS  _____________
//         /  _/ ____/   |
//         / // / __/ /| |
//       _/ // /_/ / ___ |
//      /___/\____/_/  |_| Application
//
//  Main authors:   Manuel Messmer
//

// System includes

// External includes
#include <pybind11/pybind11.h>

// Project includes
#include "includes/define.h"
#include "spaces/ublas_space.h"
#include "linear_solvers/linear_solver.h"
#include "custom_python/add_custom_solvers_to_python.h"
#include "custom_solvers/additive_schwarz_preconditioner.h"

#include "iga_application_variables.h"


namespace Kratos {
namespace Python {

void AddCustomSolversToPython(
    pybind11::module& m)
{
    namespace py = pybind11;


    typedef UblasSpace<double, CompressedMatrix, boost::numeric::ublas::vector<double>> SpaceType;
    typedef UblasSpace<double, Matrix, Vector> LocalSpaceType;

    typedef LinearSolver<LocalSpaceType, LocalSpaceType > LinearSolverTypeDense;
    typedef LinearSolverTypeDense::Pointer LinearSolverPointerTypeDense;
    typedef LocalSpaceType::MatrixType DenseMatrixType;

    typedef Preconditioner<SpaceType,  LocalSpaceType> PreconditionerType;
    typedef AdditiveSchwarzPreconditioner<SpaceType,  LocalSpaceType, LinearSolverTypeDense> AdditiveSchwarzPreconditionerType;

    py::class_<AdditiveSchwarzPreconditionerType, AdditiveSchwarzPreconditionerType::Pointer, PreconditionerType>(m,"AdditiveSchwarzPreconditioner")
    .def(py::init<DenseMatrixType&, LinearSolverPointerTypeDense>() )
    .def("__str__", PrintObject<AdditiveSchwarzPreconditionerType>)
    ;
}

} // namespace Python
} // Namespace Kratos
