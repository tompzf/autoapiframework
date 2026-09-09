
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

Summary
=======

What we have so far:

1. Meta Model
-------------

2. Component Specification
--------------------------

Contains the definition of a single executable element:

- interface
- inputs/outputs
- signals
- parameters
- constraints
- implementation-independent behavior
- metadata

Conceptually: Component = "Single function definition"

   Examples:

   | VehicleSpeed
   | WheelSpeedFusion
   | BatterySOC
   | NavigationRoutePlanner

3. Runtime Specification (not yet defined)
------------------------------------------

Contains the system composition:

- instantiated components
- dependencies
- execution sequence
- data flow
- event flow
- deployment hints

Conceptually: Runtime = "How do the components work together?"

  Examples:

  runtime includes

  components:

  - VehicleSpeed
  - WheelSpeedFusion

  connections:

  - from: WheelSpeedFusion.Speed
  - to: VehicleSpeed.SpeedInput

  dependencies:

  - VehicleSpeed depends_on WheelSpeedFusion

4. Naming
---------

``*.acs``    Automotive Component Specification files

``*.ars``    Automotive Runtime Specification files

