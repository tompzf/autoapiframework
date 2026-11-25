
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

Necessary Activities
====================

**1. Creation of the AFS file**

Definition of the AFS metamodel and its semantics.
Authoring and maintenance of AFS specifications.
Change management, versioning, and traceability.
Verification that the AFS file correctly represents the safety requirements.

**2. Validation of the AFS file**

Syntactic validation against the metamodel.
Semantic validation (constraints, consistency rules, naming rules, type compatibility, etc.).
Verification evidence and reporting.
Definition of acceptance criteria.

**3. API code generation**

Specification of the code generator behavior.
Verification that generated code corresponds to the AFS specification.
Regression testing of the generator.
Configuration management of templates and generator versions.
Reproducible generation process.

Tool considerations
###################

**Function Designer (C++ application)**

Used for:

- Reading/writing AFS files.
- Syntax checks.
- Model editing.

Certification aspects:

If the tool output is not independently verified, it may require Tool Qualification according to ISO 26262 Part 8.
If an independent verification step exists, qualification effort may be reduced. **Pydantic** is meant for an independent verification step.
Otherwise, requirements, tests, version control, and defect tracking for the tool itself are needed.

**Pydantic**

Used for:

- Model validation.
- Constraint checking.

Certification aspects:

Normally treated as a supporting software library.
Evidence is needed showing that validation rules are correctly implemented and tested.
Qualification of Pydantic itself is usually not the goal; instead, confidence is achieved through verification of the implemented validation logic.

**Jinja2**

Used for:

- Source code generation.

Certification aspects:

The generated code must be verified.
The templates become safety-relevant artifacts.
Regression tests should demonstrate that specific AFS inputs always produce the expected API code.
Template modifications require impact analysis and re-validation.

Recommended certification approach
##################################

Instead of certifying the individual technologies (C++, Pydantic, Jinja2), certify the workflow and provide evidence that:

The AFS metamodel is specified and versioned.
AFS files are validated against well-defined rules.
The code generator is deterministic and tested.
Generated code is verified.
Complete traceability exists:
Safety Requirements
AFS Model
Generated API
Verification Results
Tool confidence analysis is performed according to ISO 26262 Part 8.

Practical conclusion
####################

The strongest certification argument is usually:

The Function Designer used as an AFS editor, Pydantic, and Jinja2 are not safety products themselves.
They are development tools used within a qualified workflow.
Confidence in the workflow is achieved through tool qualification analysis, comprehensive verification of the AFS validation rules,
regression testing of the code generator, and verification of the generated API code.

This approach is generally easier to justify than attempting to certify the individual tools or libraries themselves.
