Blueprint project (VSS + VAF)
=============================

This blueprint project illustrates the combined usage of the VSS editing tool (as contributed by ZF Group) and the Application Framework (as contributed by Vector Informatik GmbH).

Overview
--------

An overview of the sample application for this project is given in the figure below. 

|ZF scenario part 1|

VSS catalogue
-------------

The signal definitions partly originate from the original VSS catalogue and are marked in gray. New, and therefore custom signals for this application, are added to the same structure by using the `vss-gui-tool <https://github.com/eclipse-autoapiframework/vss-gui-tool>`__, which is part of this Eclipse project. Those are marked in blue and purple.

<<<<<<< HEAD
The outcome of this step is filed in JSON format and located in model folder of this blueprint project (see `vss-sample-zf.json <../../../../examples/bp-vss-vaf/model/vss/vss-sample-zf.json>`__).
=======
The outcome of this step is filed in JSON format and located in model folder of this blueprint project (see `vss-sample-zf.json <./model/vss/vss-sample-zf.json>`__).
>>>>>>> 4440b85 (Add documentation including all readme files from other repos)

From here, the `application-framework <https://github.com/eclipse-autoapiframework/application-framework>`__ for design, implementation, and test of this application.

VAF interface project
---------------------

<<<<<<< HEAD
This file is consumed as input artifact by the VAF interface project. Based on the catalogue, the following interfaces are defined in the Config as Code file `interfaces.py <../../../../examples/bp-vss-vaf/Interfaces/interfaces.py>`__: 
=======
This file is consumed as input artifact by the VAF interface project. Based on the catalogue, the following interfaces are defined in the Config as Code file `interfaces.py <./Interfaces/interfaces.py>`__: 
>>>>>>> 4440b85 (Add documentation including all readme files from other repos)

- SpeedIf 
- SpeedCalculationIf 
- HazardIf 
- HazardDetectionIf

Finally, the complete interface definition gets exported to the VAF model format in JSON.

Application modules
-------------------

The model is used as input for the application module projects. For this project, there are two of them to realize the functionality: 

<<<<<<< HEAD
- `VehicleSpeedCalculation <../../../../examples/bp-vss-vaf/VehicleSpeedCalculation/>`__ 
- `SpeedHazardDetection <../../../../examples/bp-vss-vaf/SpeedHazardDetection/>`__
=======
- `VehicleSpeedCalculation <./VehicleSpeedCalculation/>`__ 
- `SpeedHazardDetection <./SpeedHazardDetection/>`__
>>>>>>> 4440b85 (Add documentation including all readme files from other repos)

Those projects hold the actual implementation of the business logic, which is specified in the figure below. Also unit tests with Google Test are possible and supported by the Application Framework as part of this project. 

|ZF scenario part 2|

<<<<<<< HEAD
On top, a third application module (`TestModule <../../../../examples/bp-vss-vaf/TestModule/>`__) is created. It is used later to connect to the open ends of the arrows as illustrated above and by that allow testing.
=======
On top, a third application module (`TestModule <./TestModule/>`__) is created. It is used later to connect to the open ends of the arrows as illustrated above and by that allow testing.
>>>>>>> 4440b85 (Add documentation including all readme files from other repos)

Integration project
-------------------

<<<<<<< HEAD
The final project is the `Integration project <../../../../examples/bp-vss-vaf/IntegrationProject/>`__. This is where the above application modules get instantiated and connected among each other. This project provides two flavors. 

- Integration of all modules in one executable. See `integration_project.py <../../../../examples/bp-vss-vaf/IntegrationProject/model/vaf/integration_project.py>`__ for the details. 

- Integration of `VehicleSpeedCalculation <../../../../examples/bp-vss-vaf/VehicleSpeedCalculation/>`__ and `SpeedHazardDetection <../../../../examples/bp-vss-vaf/SpeedHazardDetection/>`__ in one executable and `TestModule <../../../../examples/bp-vss-vaf/TestModule/>`__ in a separate one. The connection between the them is realized via Vector SIL Kit. See `integration_project_silkit.py <../../../../examples/bp-vss-vaf/IntegrationProject/model/vaf/integration_project_silkit.py>`__ for the details.

To switch between the two variants, modify the file `model.py:11-12 <../../../../examples/bp-vss-vaf/IntegrationProject/model/vaf/model.py>`__ accordingly and trigger the following two commands afterwards to re-generate and re-build the application: 
=======
The final project is the `Integration project <./IntegrationProject/>`__. This is where the above application modules get instantiated and connected among each other. This project provides two flavors. 

- Integration of all modules in one executable. See `integration_project.py <./IntegrationProject/model/vaf/integration_project.py>`__ for the details. 

- Integration of `VehicleSpeedCalculation <./VehicleSpeedCalculation/>`__ and `SpeedHazardDetection <./SpeedHazardDetection/>`__ in one executable and `TestModule <./TestModule/>`__ in a separate one. The connection between the them is realized via Vector SIL Kit. See `integration_project_silkit.py <./IntegrationProject/model/vaf/integration_project_silkit.py>`__ for the details.

To switch between the two variants, modify the file `model.py:11-12 <./IntegrationProject/model/vaf/model.py>`__ accordingly and trigger the following two commands afterwards to re-generate and re-build the application: 
>>>>>>> 4440b85 (Add documentation including all readme files from other repos)

- vaf model generate 
- vaf make install

.. |ZF scenario part 1| image:: figures/zf-scenario-part1.png
.. |ZF scenario part 2| image:: figures/zf-scenario-part2.png
