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

ISO 26262 Work-Product Checklist
================================

To use the Eclipse Automotive API Framework workflow within a safety-related development process, the following work products should be available and maintained.


.. list-table:: ISO 26262 Work-Product Checklist
   :header-rows: 1
   :widths: 20 45 35

   * - **Area**
     - **Work Product**
     - **Purpose**
   * - Method Definition
     - AFS Metamodel Specification
     - Defines the syntax and semantics of the AFS language.
   * - Requirements
     - Requirements for the API Generation Workflow
     - Describes functional and safety requirements for validation and code generation.
   * - Tooling
     - Tool Architecture Description
     - Documents the C++ editor, validation framework, and code generator architecture.
   * - Validation
     - Validation Rule Specification
     - Defines all syntactic and semantic checks implemented using Pydantic.
   * - Verification
     - Validation Test Specification and Results
     - Demonstrates correct operation of all validation rules.
   * - Code Generation
     - Generator Specification
     - Defines the mapping from AFS constructs to generated API code.
   * - Code Generation
     - Jinja2 Template Specification
     - Documents generation templates and generation rules.
   * - Verification
     - Generator Regression Test Suite
     - Verifies that defined AFS inputs generate the expected API outputs.
   * - Verification
     - Generator Test Reports
     - Evidence that the generator performs according to specification.
   * - Configuration Management
     - Tool Version Baselines
     - Identifies approved versions of the editor, Pydantic, Jinja2, and templates.
   * - Configuration Management
     - Change History and Impact Analysis
     - Documents changes and their safety impact.
   * - Traceability
     - Requirements-to-AFS Traceability
     - Links functional and safety requirements to AFS elements.
   * - Traceability
     - AFS-to-Generated-Code Traceability
     - Demonstrates how generated APIs originate from AFS definitions.
   * - Tool Qualification
     - Tool Confidence Analysis (TCL)
     - ISO 26262 Part 8 assessment of tool impact and error detection capability.
   * - Tool Qualification
     - Tool Qualification Plan and Report (if required)
     - Evidence that the tooling can be used with sufficient confidence.
   * - Safety Case
     - Workflow Safety Argumentation
     - Demonstrates that the workflow does not introduce unacceptable systematic faults.

Tool-Specific Evidence
######################

**C++ AFS Editor**

- Software requirements specification
- Design description
- Unit and integration test results
- Verification report
- Tool confidence analysis

**Pydantic Validation Layer**

- Definition of validation rules
- Test cases covering all rules
- Coverage and regression test reports
- Version baseline

**Jinja2 Code Generator**

- Generator specification
- Template reviews
- Regression test suite
- Generated-code verification reports
- Change impact analysis

**Typical Certification Strategy**

A practical ISO 26262 approach is:

1. Treat the AFS file as a safety-related model.
2. Verify the model using independent validation rules implemented in Pydantic.
3. Treat the Jinja2 templates as safety-relevant artifacts.
4. Demonstrate through regression tests that generation is deterministic and correct.
5. Perform Tool Confidence Level (TCL) analysis according to ISO 26262 Part 8.
6. Verify generated API code independently from the generation tool.
7. Maintain end-to-end traceability from safety requirements → AFS model → generated API → verification results.

**Certification Argument**

The Eclipse Automotive API Framework workflow is not certified by itself. Confidence is achieved by maintaining controlled work products, performing a Tool Confidence Analysis according to ISO 26262 Part 8, verifying the AFS validation rules, regression-testing the code generator, and verifying the generated API code. Well-established technologies such as C++, Pydantic, and Jinja2 can be used within such a workflow provided adequate verification evidence, configuration management, and traceability are available.
