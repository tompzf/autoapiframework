
..
   # *******************************************************************************
   # Copyright (c) 2026 Contributors to the Eclipse Foundation
   #
   # See the NOTICE file(s) distributed with this work for additional
   # information regarding copyright ownership.
   #
   # This program and the accompanying materials are made available under the
   # terms of the Apache License Version 2.0 which is available at
   # https://www.apache.org/licenses/LICENSE-2.0
   #
   # SPDX-License-Identifier: Apache-2.0
   #
   # Contributors:
   #   Thomas Pfleiderer - Meta model added
   # *******************************************************************************

Current status of the project

   
Component Specification File
============================  


A omponent Specification File is characterized by a set of attributes and parameters. Attributes describe the signal itself (e.g., its properties and characteristics), 
while parameters provide additional configuration and implementation-specific information. Together, they define all information required for API generation and integration.


.. table::  Meta Model Data Type: 

   +-------------------+----------+----------------------------------------------------------------+------------------------------------------------------------------------------------+----------------------------+--------------+
   | **Property /      | **Req.** | **Description**                                                | **Reason for                                                                       | **Example / Notes**        | **Covered by |
   | Attribute**       |          |                                                                | considering it**                                                                   |                            | COVESA VSS** |
   +-------------------+----------+----------------------------------------------------------------+------------------------------------------------------------------------------------+----------------------------+--------------+
   | name              | Yes      | Unique interface or node name.                                 | Needed for generated API identifiers, documentation, and traceability.             | Vehicle.Speed              | yes          |
   +-------------------+----------+----------------------------------------------------------------+------------------------------------------------------------------------------------+----------------------------+--------------+
   | path              | Yes      | Canonical VSS path of the data interface.                      | Allows stable addressing, path-based subscriptions, and standard API generation.   | Vehicle.Speed              | yes          |
   +-------------------+----------+----------------------------------------------------------------+------------------------------------------------------------------------------------+----------------------------+--------------+
   | dataType          | Yes      | Type of the value transported by the interface.                | Required for schema generation, validation, and language bindings.                 | float, uint8, boolean      | yes          |
   +-------------------+----------+----------------------------------------------------------------+------------------------------------------------------------------------------------+----------------------------+--------------+
   | description       | Yes      | Functional meaning of the signal.                              | Prevents ambiguous interpretation across suppliers, functions, and applications.   | Vehicle longitudinal speed | yes          |
   +-------------------+----------+----------------------------------------------------------------+------------------------------------------------------------------------------------+----------------------------+--------------+
   | unit              | Yes      | Engineering unit of the signal.                                | Essential for safe interpretation, comparison, and conversion.                     | "km/h (SI preferred)"      | yes          |
   +-------------------+----------+----------------------------------------------------------------+------------------------------------------------------------------------------------+----------------------------+--------------+
   | min               | Yes      | Engineering minimum representable value.                       | Helps bound legal encoding and generated validation checks.                        | 0                          | yes          |
   +-------------------+----------+----------------------------------------------------------------+------------------------------------------------------------------------------------+----------------------------+--------------+
   | max               | Yes      | Engineering maximum representable value.                       | Helps bound legal encoding and generated validation checks.                        | 300                        | yes          |
   +-------------------+----------+----------------------------------------------------------------+------------------------------------------------------------------------------------+----------------------------+--------------+
   | defaultValue      | Yes      | Default or initialization value if defined.                    | Useful for initialization, simulation, and fallback behavior.                      | 0                          | No           |
   +-------------------+----------+----------------------------------------------------------------+------------------------------------------------------------------------------------+----------------------------+--------------+
   | resolution        | Yes      | Smallest measurable quantization value. Meant for scaling etc. | Important for scaling, display, and control interpretation.                        | 0.1                        | No           |
   +-------------------+----------+----------------------------------------------------------------+------------------------------------------------------------------------------------+----------------------------+--------------+
   | precision         | Yes      | Number of meaningful digits or precision of value.             | Useful for UI formatting and downstream numeric interpretation.                    | 0.01                       | No           |
   +-------------------+----------+----------------------------------------------------------------+------------------------------------------------------------------------------------+----------------------------+--------------+
   | ASIL              | Yes      | ASIL classification when the data is safety-relevant.          | Safety-relevant data must preserve safety classification for downstream design,    | QM / A / B / C / D         | No           |
   |                   |          |                                                                | verification, and decomposition.                                                   |                            |              |
   +-------------------+----------+----------------------------------------------------------------+------------------------------------------------------------------------------------+----------------------------+--------------+
   | minUpdatePeriodMs | Yes      | Minimum interval between updates.                              | Prevents uncontrolled publication rates and helps scheduling analysis.             | 10                         | No           |
   +-------------------+----------+----------------------------------------------------------------+------------------------------------------------------------------------------------+----------------------------+--------------+
   | qualityCode       | Yes      | Runtime quality status of the value.  [1]_                     | Applications need qualifier information, not only the raw value.                   | Invalid / Valid  / & more  | No           |
   +-------------------+----------+----------------------------------------------------------------+------------------------------------------------------------------------------------+----------------------------+--------------+
   | accuracy          | Yes      | Expected measurement or estimation accuracy.                   | Important for control, fusion, analytics, and UI confidence.                       | ±0.2 km/h                  | No           |
   +-------------------+----------+----------------------------------------------------------------+------------------------------------------------------------------------------------+----------------------------+--------------+

.. [1] **Remark:** It is not a static info and hence shall be transformed into a separate signal as it gets updated runtime e.g., Vehicle.Speed.Value -> For signal value and Vehicle.Speed.Qualifier -> For quality code

Component Specification File
----------------------------

Version 0.0.1

.. literalinclude:: autoapiframework_component_specification_file_V01.acs.yaml
   :language: yaml
   :linenos: