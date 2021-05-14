/*
 * File:   DEM_application_variables.h
 * Author: Salva Latorre
 *
 * Created on October 9, 2014, 10:54 AM
 */

#ifndef KRATOS_DEM_APPLICATION_VARIABLES_H
#define	KRATOS_DEM_APPLICATION_VARIABLES_H

#include "includes/define.h"
#include "includes/variables.h"
#include "includes/dem_variables.h"
#include "includes/condition.h"
#include "utilities/quaternion.h"
#include "custom_utilities/cluster_information.h"
#include "custom_strategies/schemes/dem_integration_scheme.h"
#include "custom_utilities/properties_proxies.h"

namespace Kratos
{

#define DEM_COPY_SECOND_TO_FIRST_3(a, b)            a[0]  = b[0]; a[1]  = b[1]; a[2]  = b[2];
#define DEM_COPY_SECOND_TO_FIRST_4(a, b)            a[0]  = b[0]; a[1]  = b[1]; a[2]  = b[2]; a[3] = b[3];
#define DEM_ADD_SECOND_TO_FIRST(a, b)               a[0] += b[0]; a[1] += b[1]; a[2] += b[2];
#define DEM_SET_COMPONENTS_TO_ZERO_3(a)             a[0]  = 0.0;  a[1]  = 0.0;  a[2]  = 0.0;
#define DEM_SET_COMPONENTS_TO_ZERO_3x3(a)           a[0][0] = 0.0; a[0][1] = 0.0; a[0][2] = 0.0; a[1][0] = 0.0; a[1][1] = 0.0; a[1][2] = 0.0; a[2][0] = 0.0; a[2][1] = 0.0; a[2][2] = 0.0;
#define DEM_MULTIPLY_BY_SCALAR_3(a, b)              a[0] *= (b); a[1] *= (b); a[2] *= (b);
#define DEM_MODULUS_3(a)                            std::sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2])
#define DEM_MODULUS_2(a)                            std::sqrt(a[0] * a[0] + a[1] * a[1])
#define DEM_INNER_PRODUCT_3(a, b)                            (a[0] * b[0] + a[1] * b[1] + a[2] * b[2])
#define DEM_SET_TO_CROSS_OF_FIRST_TWO_3(a, b, c)    c[0] = a[1] * b[2] - a[2] * b[1]; c[1] = a[2] * b[0] - a[0] * b[2]; c[2] = a[0] * b[1] - a[1] * b[0];
#define DEM_COPY_SECOND_TO_FIRST_3x3(a, b)          a[0][0] = b[0][0]; a[0][1] = b[0][1]; a[0][2] = b[0][2]; \
                                                    a[1][0] = b[1][0]; a[1][1] = b[1][1]; a[1][2] = b[1][2]; \
                                                    a[2][0] = b[2][0]; a[2][1] = b[2][1]; a[2][2] = b[2][2];

#define DEM_DETERMINANT_3x3(a0, a1, a2)            (a0[0] * (a1[1] * a2[2] - a1[2] * a2[1]) - a0[1] * (a1[0] * a2[2] - a1[2] * a2[0]) + a0[2] * (a1[0] * a2[1] - a1[1] * a2[0]))

  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, GlobalPointersVector< Element >, CONTINUUM_INI_NEIGHBOUR_ELEMENTS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, GlobalPointersVector< Element >, NODE_TO_NEIGH_ELEMENT_POINTER)

  //constitutive law
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, std::string, DEM_CONTINUUM_CONSTITUTIVE_LAW_NAME)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, std::string, DEM_DISCONTINUUM_CONSTITUTIVE_LAW_NAME)

  //scheme
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, std::string, DEM_TRANSLATIONAL_INTEGRATION_SCHEME_NAME)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, std::string, DEM_ROTATIONAL_INTEGRATION_SCHEME_NAME)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, DEMIntegrationScheme::Pointer, DEM_TRANSLATIONAL_INTEGRATION_SCHEME_POINTER)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, DEMIntegrationScheme::Pointer, DEM_ROTATIONAL_INTEGRATION_SCHEME_POINTER)

  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, std::string, PROBABILITY_DISTRIBUTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, std::string, EXCENTRICITY_PROBABILITY_DISTRIBUTION)

  // OPTIONS AND FLAGS
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, TOP)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, BOTTOM)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, FORCE_INTEGRATION_GROUP)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, bool, IS_STICKY)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, TABLE_NUMBER_VELOCITY) // JIG: Backward compatibility, to erase
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, TABLE_NUMBER_ANGULAR_VELOCITY) // JIG: Backward compatibility, to erase
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, TABLE_NUMBER_FORCE) // JIG: Backward compatibility, to erase
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, TABLE_NUMBER_MOMENT) // JIG: Backward compatibility, to erase
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, CONTACT_IMPULSE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, BOUNDING_BOX_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, ROTATION_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, CRITICAL_TIME_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, VIRTUAL_MASS_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, SEARCH_CONTROL)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, bool, IS_TIME_TO_PRINT)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, COORDINATION_NUMBER)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CONTINUUM_SEARCH_RADIUS_AMPLIFICATION_FACTOR)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, MAX_AMPLIFICATION_RATIO_OF_THE_SEARCH_RADIUS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, bool, LOCAL_COORDINATION_NUMBER_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, bool, GLOBAL_COORDINATION_NUMBER_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, bool, AUTOMATIC_SKIN_COMPUTATION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, SKIN_FACTOR_RADIUS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, CLEAN_INDENT_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, TRIHEDRON_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, ROLLING_FRICTION_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, POISSON_EFFECT_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, SHEAR_STRAIN_PARALLEL_TO_BOND_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, NEIGH_INITIALIZED)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, TRIAXIAL_TEST_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, FIX_VELOCITIES_FLAG)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, COMPUTE_STRESS_TENSOR_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, PARTICLE_ID)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, bool, CONTAINS_CLUSTERS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, bool, RANDOM_ORIENTATION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, LOCAL_RESOLUTION_METHOD)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, COMPUTE_FEM_RESULTS_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, BREAKABLE_CLUSTER)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, ClusterInformation, CLUSTER_INFORMATION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, std::string, CLUSTER_FILE_NAME)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, std::string, INJECTOR_ELEMENT_TYPE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, CONTINUUM_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, FLOATING_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DEM_ENGINE_POWER)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DEM_MAX_ENGINE_FORCE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DEM_THRESHOLD_VELOCITY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DEM_ENGINE_PERFORMANCE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DEM_DRAG_CONSTANT_X)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DEM_DRAG_CONSTANT_Y)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DEM_DRAG_CONSTANT_Z)


  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, INITIAL_VELOCITY_X_VALUE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, INITIAL_VELOCITY_Y_VALUE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, INITIAL_VELOCITY_Z_VALUE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, INITIAL_ANGULAR_VELOCITY_X_VALUE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, INITIAL_ANGULAR_VELOCITY_Y_VALUE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, INITIAL_ANGULAR_VELOCITY_Z_VALUE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, bool, IS_GHOST)

  // *************** Continuum only BEGIN *************
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, bool, DELTA_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, CASE_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, SKIN_SPHERE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, PARTICLE_COHESION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, PARTICLE_INITIAL_COHESION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, AMOUNT_OF_COHESION_FROM_STRESS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, PARTICLE_TENSION)

  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, PROPERTIES_ID)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, CONTACT_MESH_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, MAX_NUMBER_OF_INTACT_BONDS_TO_CONSIDER_A_SPHERE_BROKEN)
  //KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, FAILURE_CRITERION_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, CONCRETE_TEST_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, COHESIVE_GROUP)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, IF_BOUNDARY_ELEMENT)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, Vector, IF_BOUNDARY_FACE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, DenseVector<int>, PARTICLE_CONTACT_FAILURE_ID)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DEM_PRECONSOLIDATION_PRESSURE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DEM_M_CAMCLAY_SLOPE)
  // *************** Continuum only END ***************

  // ********** Beam related variables BEGIN **********
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CROSS_AREA)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, BEAM_PARTICLES_DISTANCE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, BEAM_LENGTH)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, I22) // Planar moment of inertia 22
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, I33) // Planar moment of inertia 33
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, BEAM_INERTIA_ROT_UNIT_LENGHT_X) // Moment of inertia X / (mass * beam length)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, BEAM_INERTIA_ROT_UNIT_LENGHT_Y) // Moment of inertia Y / (mass * beam length)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, BEAM_INERTIA_ROT_UNIT_LENGHT_Z) // Moment of inertia Z / (mass * beam length)
  // **********  Beam related variables END  **********

  KRATOS_DEFINE_VARIABLE(std::vector<Condition*>, WALL_POINT_CONDITION_POINTERS)
  typedef std::vector<array_1d<double, 3> > std_vector_of_arrays_3d;
  KRATOS_DEFINE_VARIABLE(std_vector_of_arrays_3d, WALL_POINT_CONDITION_ELASTIC_FORCES)
  KRATOS_DEFINE_VARIABLE(std_vector_of_arrays_3d, WALL_POINT_CONDITION_TOTAL_FORCES)

  // MATERIAL PARAMETERS
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, std::vector<PropertiesProxy>, VECTOR_OF_PROPERTIES_PROXIES)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, NODAL_MASS_COEFF)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, PARTICLE_MOMENT_OF_INERTIA)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, ROLLING_FRICTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, ROLLING_FRICTION_WITH_WALLS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, HISTORICAL_MIN_K)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, PARTICLE_INERTIA)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, PARTICLE_DENSITY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, FRICTION) //deprecated since April 6th, 2020
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, STATIC_FRICTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DYNAMIC_FRICTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, FRICTION_DECAY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, COEFFICIENT_OF_RESTITUTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, PARTICLE_ROTATION_DAMP_RATIO)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DAMPING_GAMMA)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, K_NORMAL)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, K_TANGENTIAL)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CONICAL_DAMAGE_CONTACT_RADIUS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CONICAL_DAMAGE_MAX_STRESS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CONICAL_DAMAGE_ALPHA)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CONICAL_DAMAGE_ALPHA_FUNCTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CONICAL_DAMAGE_GAMMA)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, LEVEL_OF_FOULING)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, EXCENTRICITY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, EXCENTRICITY_STANDARD_DEVIATION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, ROTATIONAL_MOMENT_COEFFICIENT)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, POISSON_VALUE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, INTERNAL_COHESION)

  // *************** Continuum only BEGIN *************
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, SLOPE_FRACTION_N1)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, SLOPE_FRACTION_N2)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, SLOPE_FRACTION_N3)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, SLOPE_LIMIT_COEFF_C1)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, SLOPE_LIMIT_COEFF_C2)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, SLOPE_LIMIT_COEFF_C3)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, YOUNG_MODULUS_PLASTIC)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, PLASTIC_YIELD_STRESS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DAMAGE_FACTOR)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, SHEAR_ENERGY_COEF)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DONZE_G1)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DONZE_G2)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DONZE_G3)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DONZE_MAX_DEF)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CONTACT_FAILURE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CONTACT_ORIENTATION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CONTACT_SIGMA)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CONTACT_TAU)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, FAILURE_CRITERION_STATE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, UNIDIMENSIONAL_DAMAGE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CONTACT_SIGMA_MIN)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, TENSION_LIMIT_INCREASE_SLOPE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CONTACT_TAU_ZERO)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CONTACT_INTERNAL_FRICC)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, KDEM_STANDARD_DEVIATION_TAU_ZERO)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, KDEM_STANDARD_DEVIATION_FRICTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, PERTURBED_TAU_ZERO)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, PERTURBED_INTERNAL_FRICTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, SIGMA_3_AVERAGE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, LOOSE_MATERIAL_YOUNG_MODULUS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, BONDED_MATERIAL_YOUNG_MODULUS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, bool, DEBUG_PRINTING_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, DEBUG_PRINTING_ID_1)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, DEBUG_PRINTING_ID_2)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, FRACTURE_ENERGY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, SIGMA_SLOPE_CHANGE_THRESHOLD)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, INTERNAL_FRICTION_AFTER_THRESHOLD)

  // *************** Continuum only END *************

  // GEOMETRIC PARAMETERS

  // *************** Continuum only BEGIN *************
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, LOCAL_CONTACT_AREA_HIGH)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, LOCAL_CONTACT_AREA_LOW)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, MEAN_CONTACT_AREA)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, REPRESENTATIVE_VOLUME)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, DenseVector<int>,  NEIGHBOUR_IDS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, Vector,  NEIGHBOURS_CONTACT_AREAS)
  // *************** Continuum only END ***************

  // INLET PARAMETERS

  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double,INLET_START_TIME)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double,INLET_STOP_TIME)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double,INLET_NUMBER_OF_PARTICLES)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double,STANDARD_DEVIATION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double,DEVIATION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double,MAXIMUM_RADIUS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double,MINIMUM_RADIUS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double,INITIAL_RADIUS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double,MAX_RAND_DEVIATION_ANGLE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, bool,IMPOSED_MASS_FLOW_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double,MASS_FLOW)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, LINEAR_VELOCITY)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, INLET_INITIAL_VELOCITY)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, INLET_INITIAL_PARTICLES_VELOCITY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double,INLET_MAX_PARTICLES_VELOCITY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, bool,DENSE_INLET)

  // KINEMATICS
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, PARTICLE_ROTATION_ANGLE)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, EULER_ANGLES)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, bool, DOMAIN_IS_PERIODIC)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, DOMAIN_MIN_CORNER)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, DOMAIN_MAX_CORNER)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, DELTA_DISPLACEMENT)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, DELTA_ROTA_DISPLACEMENT)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, VELOCITY_START_TIME)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, VELOCITY_STOP_TIME)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, ANGULAR_VELOCITY_START_TIME)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, ANGULAR_VELOCITY_STOP_TIME)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, RIGID_BODY_MOTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, FREE_BODY_MOTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, bool, RIGID_BODY_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, RIGID_BODY_MASS)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, RIGID_BODY_CENTER_OF_MASS)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, RIGID_BODY_INERTIAS)
  // ****************** Quaternion Integration BEGIN ******************
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, Quaternion<double>, AUX_ORIENTATION)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, LOCAL_AUX_ANGULAR_VELOCITY)
  // ******************* Quaternion Integration END *******************

  // FORCE AND MOMENTUM

  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, PARTICLE_MOMENT)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, ROLLING_RESISTANCE_MOMENT)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, MAX_ROTA_MOMENT)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, ELASTIC_FORCES)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, CONTACT_FORCES)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, RIGID_ELEMENT_FORCE)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, TANGENTIAL_ELASTIC_FORCES)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, FORCE_REACTION)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, MOMENT_REACTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DEM_PRESSURE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, DEM_NODAL_AREA)

  // ENERGY

  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, PARTICLE_ELASTIC_ENERGY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, PARTICLE_TRANSLATIONAL_KINEMATIC_ENERGY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, PARTICLE_ROTATIONAL_KINEMATIC_ENERGY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, PARTICLE_GRAVITATIONAL_ENERGY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, PARTICLE_INELASTIC_VISCODAMPING_ENERGY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, PARTICLE_INELASTIC_FRICTIONAL_ENERGY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, COMPUTE_ENERGY_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, GLOBAL_DAMPING)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, NORMAL_IMPACT_VELOCITY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, TANGENTIAL_IMPACT_VELOCITY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, FACE_NORMAL_IMPACT_VELOCITY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, FACE_TANGENTIAL_IMPACT_VELOCITY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, LINEAR_IMPULSE)

  // *************** Continuum only BEGIN *************
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, INITIAL_ROTA_MOMENT)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, Vector, PARTICLE_BLOCK_CONTACT_FORCE)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, LOCAL_CONTACT_FORCE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, VectorArray3Double, PARTICLE_CONTACT_FORCES )
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, NEIGHBOUR_SIZE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, NEIGHBOUR_RATIO)

  // CONCRETE TEST

  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, FIXED_VEL_TOP)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, FIXED_VEL_BOT)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, AREA_VERTICAL_TAPA)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, AREA_VERTICAL_CENTRE)

  // TENSION

  KRATOS_DEFINE_APPLICATION_VARIABLE( DEM_APPLICATION, Matrix, DEM_STRESS_TENSOR )

  // APPLIED LOADS

  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, BLAST_RADIUS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int   , BLAST_CURVE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, BLAST_PRESSURE_MAX)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, BLAST_TIME_PRESSURE_MAX)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, BLAST_SHAPE_FACTOR)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, BLAST_TIME_DELAY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int   , BLAST_BOREHOLE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int   , BLAST_NPOINTS)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, BLAST_COORDINATES_1)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, BLAST_COORDINATES_2)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, BLAST_COORDINATES_3)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, BLAST_COORDINATES_4)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, BLAST_COORDINATES_5)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, BLAST_COORDINATES_6)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, BLAST_COORDINATES_7)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, BLAST_COORDINATES_8)
  // *************** Continuum only END *************

  // Possible future blocks (no FEM) interaction

  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, Vector, PARTICLE_BLOCK_CONTACT_FAILURE_ID)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, Vector, PARTICLE_BLOCK_IF_INITIAL_CONTACT)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, GlobalPointersVector<Element >,    NEIGHBOUR_PARTICLE_BLOCK_ELEMENTS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, GlobalPointersVector<Condition >,  NEIGHBOUR_RIGID_FACES)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, GlobalPointersVector<Element >,    NEIGHBOUR_PARTICLE_OF_RIGID_FACE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, Vector,  NEIGHBOUR_RIGID_FACES_PRAM)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, Vector,  NEIGHBOUR_RIGID_FACES_ELASTIC_CONTACT_FORCE)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, Vector,  NEIGHBOUR_RIGID_FACES_TOTAL_CONTACT_FORCE)

  // DUMMIES INT AND DOUBLE VARIABLES
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, DUMMY_SWITCH)

  // EXPORTS

  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, EXPORT_ID)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, EXPORT_PARTICLE_FAILURE_ID)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, PRINT_EXPORT_ID)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, PRINT_STRESS_TENSOR_OPTION)

  // For DEM_FEM Element

  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, LOCAL_DAMP_RATIO)

  // For the DEM_Clusters Element

  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CLUSTER_VOLUME)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, PRINCIPAL_MOMENTS_OF_INERTIA)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, LOCAL_ANGULAR_VELOCITY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CHARACTERISTIC_LENGTH)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, SPRAYED_MATERIAL)

  // DUMMY VARIABLES FOR CALCULATE
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CALCULATE_COMPUTE_NEW_NEIGHBOURS_HISTORICAL_DATA)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CALCULATE_COMPUTE_NEW_RIGID_FACE_NEIGHBOURS_HISTORICAL_DATA)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CALCULATE_SET_INITIAL_DEM_CONTACTS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, CALCULATE_SET_INITIAL_FEM_CONTACTS)

  //Cfeng,131013,RigidFace

  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, RIGID_FACE_ROTA_SPEED)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, RIGID_FACE_AXIAL_SPEED)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int,    RIGID_FACE_PROP_ID)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, RIGID_FACE_ROTA_ORIGIN_COORD)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, RIGID_FACE_ROTA_AXIAL_DIR)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, RIGID_FACE_ROTA_GLOBAL_VELOCITY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, RIGID_FACE_BEGIN_TIME)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, RIGID_FACE_END_TIME)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, int, RIGID_FACE_FLAG)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, Vector, RIGID_FACE_COMPUTE_MOVEMENT)

  //SLS DEM-FEM
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, SHEAR_STRESS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, NON_DIMENSIONAL_VOLUME_WEAR)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, IMPACT_WEAR)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, SEVERITY_OF_WEAR)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, BRINELL_HARDNESS)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, bool, COMPUTE_WEAR)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, IMPACT_WEAR_SEVERITY)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, WALL_COHESION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, WALL_INITIAL_COHESION)

  //BOUNDING BOX
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, BOUNDING_BOX_START_TIME)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, BOUNDING_BOX_STOP_TIME)

  //OPTIMIZATION
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, TOTAL_CONTACT_DISTANCES)

  // *************** Thermal only BEGIN *************
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, HEATFLUX)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, THERMAL_CONDUCTIVITY)
  // *************** Thermal only END ***************

  //CONTROL MODULE
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, TARGET_STRESS)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, REACTION_STRESS)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, SMOOTHED_REACTION_STRESS)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, ELASTIC_REACTION_STRESS)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, SMOOTHED_ELASTIC_REACTION_STRESS)
  KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_APPLICATION, LOADING_VELOCITY)

  // for DEM-FEM 2D
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, bool, IMPOSED_Z_STRAIN_OPTION)
  KRATOS_DEFINE_APPLICATION_VARIABLE(DEM_APPLICATION, double, IMPOSED_Z_STRAIN_VALUE)

class DEMFlags
  {
  public:
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, HAS_ROTATION);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, IS_SINTERING);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, HAS_ROLLING_FRICTION);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, STICKY);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, HAS_CRITICAL_TIME);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, FIXED_VEL_X);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, FIXED_VEL_Y);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, FIXED_VEL_Z);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, FIXED_ANG_VEL_X);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, FIXED_ANG_VEL_Y);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, FIXED_ANG_VEL_Z);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, BELONGS_TO_A_CLUSTER);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, HAS_STRESS_TENSOR);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, COPIED_STRESS_TENSOR);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, COPIED_STRESS_TENSOR2);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, PRINT_STRESS_TENSOR);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, CUMULATIVE_ZONE);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, IS_SAND_PRODUCTION);
    KRATOS_DEFINE_LOCAL_APPLICATION_FLAG(DEM_APPLICATION, POLYHEDRON_SKIN);
  };
}

#endif	/* KRATOS_DEM_APPLICATION_VARIABLES_H */
