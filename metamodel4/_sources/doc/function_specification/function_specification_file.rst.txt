
..
   # *******************************************************************************
   # Copyright (c) 2026 ZF Friedrichshafen AG
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

Current status of the project:

.. figure:: ../meta_model/figures/time_line.png
   :alt: time line of the project
   
Function Specification File
===========================  


A Function Specification File is characterized by a set of attributes and parameters. Attributes describe the signal itself (e.g., its properties and characteristics), 
while parameters provide additional configuration and implementation-specific information. Together, they define all information required for API generation and integration.

A Function Specification File always describes exactly **one** function: its ``dataInterfaces``, its ``parameters`` and the scheduling of
its own runnables. It does not describe how this function is combined with other functions, how their signals are connected to each other,
or in which order several functions execute relative to each other on a system. That system-level composition of multiple Function
Specification Files is described by the :doc:`Runtime Specification File </doc/runtime_specification/runtime_specification_file>`.

About the naming
----------------

In our documentation, we deliberately use the term Function (for example, Function Specification File and Function Adapter) because other terms, such as Component, 
Application, or Module, often carry pre-existing meanings in different frameworks and toolchains.

In this context, a Function represents a self-contained piece of functionality, typically implemented as a wrapper or adapter on top of middleware and algorithmic code 
(for example, MATLAB-generated code). The term is intentionally generic and can be used to describe a wide range of algorithmic purposes, including:

- Vehicle Function
- Diagnostic Function 
- Control Function 
- Analysis Function 
- Calculation Function 
- Signal Processing
- Data Processing 
- Estimation Function 
- Monitoring Function 
- Simulation Function 

Therefore, the term Function should be understood as a generic abstraction for algorithmic functionality rather than as a specific software architectural concept.

Content Of Function Specification File
--------------------------------------

.. table::  Function: 

   +----------------+----------+----------------------------------------------------------+-----------------------------------------------------+
   | **Property /   | **Req.** | **Description**                                          | **Example / Notes**                                 |
   | Attribute**    |          |                                                          |                                                     |
   +----------------+----------+----------------------------------------------------------+-----------------------------------------------------+
   | dataInterfaces | Yes      | Collection of required signals                           | - namePath: Vehicle.Speed                           |
   |                |          |                                                          |                                                     |
   |                |          |                                                          |   direction: input                                  |
   |                |          |                                                          |                                                     |
   |                |          |                                                          |   dataType: float                                   |
   |                |          |                                                          |                                                     |
   |                |          |                                                          |   unit: km/h                                        |
   |                |          |                                                          |                                                     |
   |                |          |                                                          |   etc.                                              |
   +----------------+----------+----------------------------------------------------------+-----------------------------------------------------+
   | parameters     | Yes      | Collection of parameters for calibration & configuration | - namePath: Vehicle.Chassis.HazardRequestDurationMs |
   |                |          |                                                          |                                                     |
   |                |          |                                                          |   dataType: uint32                                  |
   |                |          |                                                          |                                                     |
   |                |          |                                                          |   defaultValue: 3000                                |
   |                |          |                                                          |                                                     |
   |                |          |                                                          |   unit: ms                                          |
   |                |          |                                                          |                                                     |
   |                |          |                                                          |   etc.                                              |
   +----------------+----------+----------------------------------------------------------+-----------------------------------------------------+
   | errorInterfaces| Optional | Collection of function-specific diagnostic interfaces,   | - name: <Function>.<ErrorName>.ErrorStatus          |
   |                |          | separate from the generic executionResult reported per   |                                                     |
   |                |          | runnable in scheduling [2]_                              |   severity: degraded                                |
   |                |          |                                                          |                                                     |
   |                |          |                                                          |   maturationTimeMs: 100                             |
   |                |          |                                                          |                                                     |
   |                |          |                                                          |   resetCondition: ...                               |
   |                |          |                                                          |                                                     |
   |                |          |                                                          |   etc.                                              |
   +----------------+----------+----------------------------------------------------------+-----------------------------------------------------+
   | scheduling     | Yes      | Scheduling information for runnable/function entry point | - functionName: SpeedHazardDetection.Init           |
   |                |          | [1]_                                                     |                                                     |   
   |                |          |                                                          |   runType: init                                     |
   |                |          |                                                          |                                                     |   
   |                |          |                                                          |   cycleTimeMs: 0                                    |
   |                |          |                                                          |                                                     |   
   |                |          |                                                          |   ASIL: B                                           |
   |                |          |                                                          |                                                     |   
   |                |          |                                                          |   previousRunnableRef: System.Startup               |
   +----------------+----------+----------------------------------------------------------+-----------------------------------------------------+
   | supervision    | Yes [2]_ | Per-runnable execution supervision configured within     | - required: true                                    |
   |                |          | scheduling: alive, deadline and/or logical checks.       |                                                     |
   |                |          |                                                          |   type: [alive, deadline]                           |
   |                |          |                                                          |                                                     |
   |                |          |                                                          |   alive:                                            |
   |                |          |                                                          |                                                     |
   |                |          |                                                          |     referenceCycleMs: 10                            |
   |                |          |                                                          |                                                     |
   |                |          |                                                          |   deadline:                                         |
   |                |          |                                                          |                                                     |
   |                |          |                                                          |     maxExecutionTimeMs: 15                          |
   +----------------+----------+----------------------------------------------------------+-----------------------------------------------------+

.. [1] **Remark:** There may be dependencies to other functions not included in this specification file. This is allowed. The whole picture is defined in the runtime specification, see :doc:`Software Architecture </doc/runtime_specification/runtime_specification_file>`.

.. [2] **Remark:** ``supervision`` is a mandatory object within each ``scheduling`` entry, but only requires the ``required: false`` case; ``type``/``alive``/``deadline``/``logical`` are only needed when ``required: true``. See the :doc:`error/supervision example </doc/function_specification/examples/wheel_speed_plausibility_check_example>` for a fully populated ``errorInterfaces`` and ``supervision`` configuration.





Template Version 0.0.1

.. literalinclude:: autoapiframework_function_specification_template_V01.afs.yaml
   :language: yaml
   :linenos:

What needs to be added for the SpeedHazardDetection example:

.. literalinclude:: autoapiframework_function_specification_example.afs.yaml
   :language: yaml
   :linenos:


