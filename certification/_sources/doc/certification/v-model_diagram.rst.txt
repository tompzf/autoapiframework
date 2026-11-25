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
   #   Thomas Pfleiderer - Function designer added
   #   Thomas Pfleiderer - support 4.1, Support YAML comments, Drag & Drop, Enums in afs files added
   # *******************************************************************************

V-Model diagram
===============

A simplified V-Model representation of the Eclipse Automotive API Framework workflow could look like this:

 .. code-block:: text

                 SAFETY REQUIREMENTS
                         |
                         v
              +----------------------+
              |  AFS Metamodel       |
              |  Specification       |
              +----------------------+
                         |
                         v
              +----------------------+
              |  AFS Definition      |
              |  (*.afs)             |
              +----------------------+
                         |
                         v
              +----------------------+
              |  AFS Validation      |
              |  (Pydantic Rules)    |
              +----------------------+
                         |
                         v
              +----------------------+
              |  API Generation      |
              |  (Jinja2 Templates)  |
              +----------------------+
                         |
                         v
              +----------------------+
              | Generated Function   |
              | API Code             |
              +----------------------+
                         |
                         v
              +----------------------+
              | Integration into     |
              | Vehicle Function     |
              +----------------------+

------------------------------------------------

 .. code-block:: text

              +----------------------+
              | Generated Code       |
              | Verification Tests   |
              +----------------------+
                         ^
                         |
              +----------------------+
              | Generator Regression |
              | Tests                |
              +----------------------+
                         ^
                         |
              +----------------------+
              | Validation Rule      |
              | Tests                |
              +----------------------+
                         ^
                         |
              +----------------------+
              | Metamodel Reviews &  |
              | Requirement Reviews  |
              +----------------------+


Or in a more ISO 26262-oriented V-model:


 .. code-block:: text

        LEFT SIDE (Specification)        RIGHT SIDE (Verification)

        Safety Requirements   -----------------> Requirement Verification
               |
               v
        AFS Metamodel         -----------------> Metamodel Review
               |
               v
        AFS File (*.afs)      -----------------> AFS Validation Results
               |                                 (Pydantic)
               |
               v
        Generation Rules
        (Jinja2 Templates)    -----------------> Generator Regression Tests
               |
               v
        Generated API Code    -----------------> API Verification /
               |                                 Static Analysis /
               |                                 Unit Tests
               |
               v
        Vehicle Function      -----------------> Integration Tests
               |
               v
        Safety-related ECU    -----------------> System Tests


**Tool qualification view**

 .. code-block:: text

                    Safety Requirements
                             |
                             v
                       AFS Model
                             |
                      [Validation]
                        Pydantic
                             |
                             v
                      Validated AFS
                             |
                     [Code Generator]
                           Jinja2
                             |
                             v
                     Generated API
                             |
                       Verification
                    (Unit/Integration Tests)

Tool Confidence Analysis (ISO 26262-8)
######################################

- AFS Editor (C++)
- Pydantic
- Jinja2
- Generation Templates

This version maps directly to the ISO 26262 argument that the AFS model is the specification artifact, the Pydantic layer performs model verification, Jinja2 performs transformation/code generation, and independent verification activities provide confidence in the generated API and the tools used.
