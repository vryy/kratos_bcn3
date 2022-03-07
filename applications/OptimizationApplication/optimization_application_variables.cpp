// ==============================================================================
//  KratosOptimizationApplication
//
//  License:         BSD License
//                   license: OptimizationApplication/license.txt
//
//  Main authors:    Reza Najian Asl, https://github.com/RezaNajian
//
// ==============================================================================

#include "optimization_application_variables.h"

namespace Kratos
{

    // KRATOS_CREATE_VARIABLE(double,TEST_MAP);

    //strain energy
	KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_STRAIN_ENERGY_D_X);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_STRAIN_ENERGY_D_CX);
	KRATOS_CREATE_VARIABLE(double, D_STRAIN_ENERGY_D_T);
    KRATOS_CREATE_VARIABLE(double, D_STRAIN_ENERGY_D_CT);
	KRATOS_CREATE_VARIABLE(double, D_STRAIN_ENERGY_D_P);
    KRATOS_CREATE_VARIABLE(double, D_STRAIN_ENERGY_D_CP);    

    //mass
	KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_MASS_D_X);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_MASS_D_CX);
	KRATOS_CREATE_VARIABLE(double, D_MASS_D_T);
    KRATOS_CREATE_VARIABLE(double, D_MASS_D_CT);
	KRATOS_CREATE_VARIABLE(double, D_MASS_D_P);
    KRATOS_CREATE_VARIABLE(double, D_MASS_D_CP);  

    //eigenfrequency
	KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_EIGEN_FREQ_D_X);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_EIGEN_FREQ_D_CX);
	KRATOS_CREATE_VARIABLE(double, D_EIGEN_FREQ_D_T);
    KRATOS_CREATE_VARIABLE(double, D_EIGEN_FREQ_D_CT);
	KRATOS_CREATE_VARIABLE(double, D_EIGEN_FREQ_D_P);
    KRATOS_CREATE_VARIABLE(double, D_EIGEN_FREQ_D_CP);       

    //local_stress
	KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_LOCAL_STRESS_D_X);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_LOCAL_STRESS_D_CX);
	KRATOS_CREATE_VARIABLE(double, D_LOCAL_STRESS_D_T);
    KRATOS_CREATE_VARIABLE(double, D_LOCAL_STRESS_D_CT);
	KRATOS_CREATE_VARIABLE(double, D_LOCAL_STRESS_D_P);
    KRATOS_CREATE_VARIABLE(double, D_LOCAL_STRESS_D_CP);     

    //max_stress
	KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_MAX_STRESS_D_X);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_MAX_STRESS_D_CX);
	KRATOS_CREATE_VARIABLE(double, D_MAX_STRESS_D_T);
    KRATOS_CREATE_VARIABLE(double, D_MAX_STRESS_D_CT);
	KRATOS_CREATE_VARIABLE(double, D_MAX_STRESS_D_P);
    KRATOS_CREATE_VARIABLE(double, D_MAX_STRESS_D_CP);       

    // shape control
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(CX);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_CX);  
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_X);         

    // For implicit vertex-morphing with Helmholtz PDE
	KRATOS_CREATE_VARIABLE( Matrix, HELMHOLTZ_MASS_MATRIX_SHAPE );
	KRATOS_CREATE_VARIABLE( double, HELMHOLTZ_RADIUS_SHAPE );
	KRATOS_CREATE_VARIABLE( bool, COMPUTE_CONTROL_POINTS_SHAPE );
    KRATOS_CREATE_VARIABLE( double, HELMHOLTZ_POISSON_RATIO_SHAPE );
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( HELMHOLTZ_VARS_SHAPE);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( HELMHOLTZ_SOURCE_SHAPE);  

}
