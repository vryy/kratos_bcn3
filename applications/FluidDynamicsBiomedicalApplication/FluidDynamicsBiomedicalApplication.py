import KratosMultiphysics.FluidDynamicsApplication
from KratosFluidDynamicsBiomedicalApplication import *

from KratosMultiphysics import _ImportApplication
application = KratosFluidDynamicsBiomedicalApplication()
application_name = "KratosFluidDynamicsBiomedicalApplication"

_ImportApplication(application, application_name)