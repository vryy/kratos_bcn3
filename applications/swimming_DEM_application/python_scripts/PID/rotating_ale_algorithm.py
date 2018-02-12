from KratosMultiphysics import *
from KratosMultiphysics.DEMApplication import *
from KratosMultiphysics.SwimmingDEMApplication import *
from DEM_procedures import KratosPrint as Say
import pre_calculated_fluid_algorithm
BaseAlgorithm = pre_calculated_fluid_algorithm.Algorithm
import numpy as np
import math

class Algorithm(BaseAlgorithm):
    def __init__(self, varying_parameters = Parameters("{}")):
        BaseAlgorithm.__init__(self, varying_parameters)
        self.SetRotator()

    def SetRotator(self):
        self.rotator = MeshRotationUtility(self.pp.CFD_DEM)

    def SetBetaParameters(self):
        BaseAlgorithm.SetBetaParameters(self)
        self.pp.CFD_DEM.AddEmptyValue("ALE_option").SetBool(True)
        Add = self.pp.CFD_DEM.AddEmptyValue
        Add("steps_per_average_step").SetInt(1)
        Add("initial_averaging_time").SetDouble(0.0)
        Add("averaging_has_already_been_done").SetBool(False)
        Add("stationary_start_time").SetDouble(0.0)

    def UpdateALEMeshMovement(self, time):
        if self.pp.CFD_DEM["ALE_option"].GetBool():
            self.rotator.RotateMesh(self.fluid_model_part, time)
            self.projection_module.UpdateDatabase(self.h_min)

    def AssessStationarity(self):
        # BaseAlgorithm.AssessStationarity(self)
        if self.time > self.pp.CFD_DEM["stationary_start_time"].GetDouble():
           self.stationarity = True

        if self.stationarity:
            self.rotator.SetStationaryField(self.fluid_model_part, self.time)

    def SetFluidLoader(self):
        import hdf5_io_tools_PID
        import average_field

        rotation_axis_initial_point = self.pp.CFD_DEM["frame_rotation_axis_initial_point"].GetVector()
        rotation_axis_final_point = self.pp.CFD_DEM["frame_rotation_axis_final_point"].GetVector()
        angular_velocity_module = self.pp.CFD_DEM["angular_velocity_magnitude"].GetDouble()
        dataset_name = 'stationary_field'
        original_file_name = self.pp.CFD_DEM["prerun_fluid_file_name"].GetString()
        initial_averaging_time = self.pp.CFD_DEM["initial_averaging_time"].GetDouble()
        steps_per_average_step = self.pp.CFD_DEM["steps_per_average_step"].GetInt()

        averager = average_field.Averager(rotation_axis_initial_point = rotation_axis_initial_point,
                                          rotation_axis_final_point = rotation_axis_final_point,
                                          angular_velocity_module = angular_velocity_module,
                                          dataset_name = dataset_name,
                                          original_file_name = original_file_name,
                                          original_file_path = self.main_path,
                                          initial_time = initial_averaging_time,
                                          steps_per_average_step = steps_per_average_step)

        self.fluid_loader = hdf5_io_tools_PID.FluidHDF5LoaderPID(self.all_model_parts.Get('FluidPart'),
                                                                 self.all_model_parts.Get('SpheresPart'),
                                                                 self.pp,
                                                                 self.main_path,
                                                                 averager)

    def FluidSolve(self, time='None', solve_system=True):
        if not self.pp.CFD_DEM["averaging_has_already_been_done"].GetBool():
            self.fluid_loader.averager.PerformAverage(reference_time = time)
            self.pp.CFD_DEM["averaging_has_already_been_done"].SetBool(True)

        BaseAlgorithm.FluidSolve(self, time, solve_system)

        if not solve_system:
            self.rotator.RotateFluidVelocities(time)
