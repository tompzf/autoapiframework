
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

Your naming actually suggests a nice separation of concerns:

1. Component Specification

Contains the definition of a single executable element:

interface
inputs/outputs
signals
parameters
constraints
implementation-independent behavior
metadata

Conceptually:

Component = "What does this thing do?"


Examples:

VehicleSpeed
WheelSpeedFusion
BatterySOC
NavigationRoutePlanner

2. Runtime Specification

Contains the system composition:

instantiated components
dependencies
execution sequence
data flow
event flow
deployment hints

Conceptually:

Runtime = "How do the components work together?"


Examples:

runtime:
  components:
    - VehicleSpeed
    - WheelSpeedFusion

  connections:
    - from: WheelSpeedFusion.Speed
      to: VehicleSpeed.SpeedInput

  dependencies:
    - VehicleSpeed depends_on WheelSpeedFusion


This is very similar to how many modeling approaches distinguish between:

Type Definition      -> Component Specification
Composition Instance -> Runtime Specification

Naming

I would use:

``*.acs``    Automotive Component Specification
``*.ars``    Automotive Runtime Specification


The pair is easy to remember:

vehicle_speed.acs
vehicle_runtime.ars


or

WheelSpeedFusion.acs
ADAS_System.ars


The naming symmetry is attractive:

ACS = Component
ARS = Runtime


Other options:

ACS = Component Specification
ASS = System Specification


but ASS is probably unfortunate.

Or:

ACS = Component Specification
AMS = System Model


but then the relation is less obvious.

My recommendation would be:



``*.acs``  = Automotive Component Specification
``*.ars``  = Automotive Runtime Specification


because the distinction is immediately visible and the files naturally form a hierarchy:

WheelSpeedFusion.acs
VehicleSpeed.acs
ADAS_Runtime.ars


where the .ars file references and connects multiple .acs components. This aligns nicely with your description of "single function/component" versus "multiple components and their relationships."
