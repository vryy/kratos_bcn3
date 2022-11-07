//
//   Project Name:        KratosPfemFluidDynamicsApplication $
//   Created by:          $Author:               JMCarbonell $
//   Last modified by:    $Co-Author:                        $
//   Date:                $Date:               February 2016 $
//   Revision:            $Revision:                     0.0 $
//
//

#include "pfem_fluid_dynamics_application_variables.h"

namespace Kratos
{

///@name Type Definitions
///@{

///@}

///@name Kratos Globals
///@{

//Create Variables

// KRATOS_CREATE_VARIABLE(double, M_MODULUS )
// KRATOS_CREATE_VARIABLE(int, PATCH_INDEX )
// KRATOS_CREATE_VARIABLE(double, NORMVELOCITY )
KRATOS_CREATE_VARIABLE(bool, NO_MESH)
KRATOS_CREATE_VARIABLE(bool, FREESURFACE)
KRATOS_CREATE_VARIABLE(bool, PREVIOUS_FREESURFACE)
KRATOS_CREATE_VARIABLE(double, INITIAL_DELTA_TIME)
KRATOS_CREATE_VARIABLE(double, CURRENT_DELTA_TIME)
KRATOS_CREATE_VARIABLE(bool, TIME_INTERVAL_CHANGED)
KRATOS_CREATE_VARIABLE(bool, BAD_VELOCITY_CONVERGENCE)
KRATOS_CREATE_VARIABLE(bool, BAD_PRESSURE_CONVERGENCE)
KRATOS_CREATE_VARIABLE(unsigned int, STEPS_WITH_CHANGED_DT)
KRATOS_CREATE_VARIABLE(bool, ISOLATED_NODE)
KRATOS_CREATE_VARIABLE(double, NODAL_H_WALL)
KRATOS_CREATE_VARIABLE(unsigned int, MAIN_MATERIAL_PROPERTY)


//Papanastasiou variables
KRATOS_CREATE_VARIABLE(double, FLOW_INDEX)
KRATOS_CREATE_VARIABLE(bool, YIELDED)
KRATOS_CREATE_VARIABLE(double, YIELD_SHEAR)
KRATOS_CREATE_VARIABLE(double, ADAPTIVE_EXPONENT)

//Frictional Viscoplastic variables
KRATOS_CREATE_VARIABLE(double, COHESION)

//mu(I)-rheology variables
KRATOS_CREATE_VARIABLE(double, STATIC_FRICTION)
KRATOS_CREATE_VARIABLE(double, DYNAMIC_FRICTION)
KRATOS_CREATE_VARIABLE(double, INERTIAL_NUMBER_ZERO)
KRATOS_CREATE_VARIABLE(double, GRAIN_DIAMETER)
KRATOS_CREATE_VARIABLE(double, GRAIN_DENSITY)
KRATOS_CREATE_VARIABLE(double, REGULARIZATION_COEFFICIENT)

KRATOS_CREATE_VARIABLE(double, PRESSURE_VELOCITY)
KRATOS_CREATE_VARIABLE(double, PRESSURE_ACCELERATION)

KRATOS_CREATE_VARIABLE(double, NODAL_ERROR_XX)

KRATOS_CREATE_VARIABLE(Vector, NODAL_CAUCHY_STRESS)
KRATOS_CREATE_VARIABLE(Vector, NODAL_DEVIATORIC_CAUCHY_STRESS)
KRATOS_CREATE_VARIABLE(Vector, NODAL_SFD_NEIGHBOURS)
KRATOS_CREATE_VARIABLE(Vector, NODAL_SFD_NEIGHBOURS_ORDER)
KRATOS_CREATE_VARIABLE(Matrix, NODAL_DEFORMATION_GRAD)
KRATOS_CREATE_VARIABLE(Matrix, NODAL_DEFORMATION_GRAD_VEL)
KRATOS_CREATE_VARIABLE(Vector, NODAL_SPATIAL_DEF_RATE)
KRATOS_CREATE_VARIABLE(double, NODAL_VOLUMETRIC_DEF_RATE)
KRATOS_CREATE_VARIABLE(double, NODAL_EQUIVALENT_STRAIN_RATE)
KRATOS_CREATE_VARIABLE(double, NODAL_MEAN_MESH_SIZE)
KRATOS_CREATE_VARIABLE(double, NODAL_TAU)
KRATOS_CREATE_VARIABLE(double, NODAL_FREESURFACE_AREA)
KRATOS_CREATE_VARIABLE(double, VOLUMETRIC_COEFFICIENT)
KRATOS_CREATE_VARIABLE(double, DEVIATORIC_COEFFICIENT)
KRATOS_CREATE_VARIABLE(bool, INTERFACE_NODE)

KRATOS_CREATE_VARIABLE(double, SOLID_NODAL_VOLUME)
KRATOS_CREATE_VARIABLE(Vector, SOLID_NODAL_CAUCHY_STRESS)
KRATOS_CREATE_VARIABLE(Vector, SOLID_NODAL_DEVIATORIC_CAUCHY_STRESS)
KRATOS_CREATE_VARIABLE(Vector, SOLID_NODAL_SFD_NEIGHBOURS)
KRATOS_CREATE_VARIABLE(Vector, SOLID_NODAL_SFD_NEIGHBOURS_ORDER)
KRATOS_CREATE_VARIABLE(Matrix, SOLID_NODAL_DEFORMATION_GRAD)
KRATOS_CREATE_VARIABLE(Matrix, SOLID_NODAL_DEFORMATION_GRAD_VEL)
KRATOS_CREATE_VARIABLE(Vector, SOLID_NODAL_SPATIAL_DEF_RATE)
KRATOS_CREATE_VARIABLE(double, SOLID_NODAL_VOLUMETRIC_DEF_RATE)
KRATOS_CREATE_VARIABLE(double, SOLID_NODAL_EQUIVALENT_STRAIN_RATE)
KRATOS_CREATE_VARIABLE(double, SOLID_NODAL_MEAN_MESH_SIZE)
KRATOS_CREATE_VARIABLE(double, SOLID_DENSITY)
KRATOS_CREATE_VARIABLE(double, SOLID_NODAL_TAU)
KRATOS_CREATE_VARIABLE(double, SOLID_NODAL_FREESURFACE_AREA)
KRATOS_CREATE_VARIABLE(double, SOLID_VOLUMETRIC_COEFFICIENT)
KRATOS_CREATE_VARIABLE(double, SOLID_DEVIATORIC_COEFFICIENT)
KRATOS_CREATE_VARIABLE(bool, SOLID_INTERFACE_NODE)

///@}

} // namespace Kratos
