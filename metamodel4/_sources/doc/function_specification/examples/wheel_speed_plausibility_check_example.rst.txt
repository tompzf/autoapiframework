
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
   #   Thomas Pfleiderer - Function specification added
   # *******************************************************************************

WheelSpeedPlausibilityCheck Error and Supervision Example
===========================================================

This example shows function-specific error interfaces (``errorInterfaces``) and full runnable execution supervision (``alive``, ``deadline`` and
``logical`` supervision types, plus the per-runnable ``executionResult``) as defined in the meta model's ``Scheduling`` and ``Error`` interface types.

.. literalinclude:: wheel_speed_plausibility_check.afs.yaml
   :language: yaml
   :linenos:
