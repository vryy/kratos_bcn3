from __future__ import print_function, absolute_import, division #makes KratosMultiphysics backward compatible with python 2.6 and 2.7
from KratosMultiphysics import _ImportApplication
from KratosMeshMovingApplication import *
application = KratosMeshMovingApplication()
application_name = "KratosMeshMovingApplication"

_ImportApplication(application, application_name)
