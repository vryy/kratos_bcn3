# ==============================================================================
#  KratosOptimizationApplication
#
#  License:         BSD License
#                   license: OptimizationApplication/license.txt
#
#  Main authors:    Reza Najian Asl, https://github.com/RezaNajian
#
# ==============================================================================

# Making KratosMultiphysics backward compatible with python 2.6 and 2.7
from __future__ import print_function, absolute_import, division

# Kratos Core and Apps
import KratosMultiphysics as KM
import KratosMultiphysics.OptimizationApplication as KO

# additional imports


## Purely for backward compatibility, should be removed soon.
def CreateOptimizer(optimization_settings):
    return Optimizer(optimization_settings)

def Create(optimization_settings):
    return Optimizer(optimization_settings)

# ==============================================================================
class Optimizer:
    # --------------------------------------------------------------------------
    def __init__(self, optimization_settings):
        self._ValidateSettings(optimization_settings)
        self.optimization_settings = optimization_settings
        
        


    def _ValidateSettings(self, optimization_settings):
        self._ValidateTopLevelSettings(optimization_settings)
        self._ValidateModelPartsSettingsRecursively(optimization_settings["model_parts"])
        self._ValidateControlsSettingsRecursively(optimization_settings["controls"])
        self._ValidateAnalysisSettingsRecursively(optimization_settings["analysis"])
        self._ValidateAlgorithmsSettingsRecursively(optimization_settings["algorithms"])

    # ------------------------------------------------------------------------------
    def _ValidateTopLevelSettings(self, optimization_settings):
        default_settings = KM.Parameters("""
        {
            "model_parts" : [ ],
            "controls" : [ ],
            "analysis" : [ ],
            "algorithms" : [ ]
        }""")

        for key in default_settings.keys():
            if not optimization_settings.Has(key):
                raise RuntimeError("Optimizer: Required setting '{}' missing in 'optimization_settings'!".format(key))

        optimization_settings.ValidateAndAssignDefaults(default_settings)

    # ------------------------------------------------------------------------------
    def _ValidateModelPartsSettingsRecursively(self, model_parts_settings):
        default_settings = KM.Parameters("""
        {
            "domain_size"           : 3,
            "model_part_name"       : "MODEL_PART_NAME",
            "model_import_settings"              : {
                "input_type"     : "mdpa",
                "input_filename" : "MODEL_PART_FILENAME"
            }
        }""")
        for itr in range(model_parts_settings.size()):
            model_parts_settings[itr].ValidateAndAssignDefaults(default_settings)

    # ------------------------------------------------------------------------------
    def _ValidateControlsSettingsRecursively(self, controls_settings):
        default_settings = KM.Parameters("""
        {
            "name"       : "CONTROL_NAME",
            "type"       : "CONTROL_TYPE",
            "settings"              : {}
        }""")
        for itr in range(controls_settings.size()):
            controls_settings[itr].ValidateAndAssignDefaults(default_settings)
    # ------------------------------------------------------------------------------
    def _ValidateAnalysisSettingsRecursively(self, analysis_settings):
        default_settings = KM.Parameters("""
        {
            "name"       : "ANALYSIS_NAME",
            "type"       : "ANALYSIS_TYPE",
            "settings"              : {}
        }""")
        for itr in range(analysis_settings.size()):
            analysis_settings[itr].ValidateAndAssignDefaults(default_settings) 
    # ------------------------------------------------------------------------------
    def _ValidateAlgorithmsSettingsRecursively(self, algorithms_settings):
        default_settings = KM.Parameters("""
        {
            "name"       : "ALGORITHM_NAME",
            "type"       : "ALGORITHM_TYPE",
            "settings"              : {}
        }""")
        if algorithms_settings.size() > 1 :
            raise NameError("OptimizationApplication only handles single algorithm, check your optimization settings ")

        for itr in range(algorithms_settings.size()):
            algorithms_settings[itr].ValidateAndAssignDefaults(default_settings)             
