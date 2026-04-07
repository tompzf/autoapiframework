
Meta Model
==========

.. table::  Meta Model: 

   +----------+--------------+-------------------------------------------------+----------------------------------------------------------------------------------+----------------------------+
   | **Type** | **Property / | **Description**                                 | **Reason for considering it**                                                    | **Example / Notes**        |
   |          | Attribute**  |                                                 |                                                                                  |                            |
   +----------+--------------+-------------------------------------------------+----------------------------------------------------------------------------------+----------------------------+
   | Data     | datatype     | Type of the value transported by the interface. | Required for schema generation, validation, and language bindings.               | float, uint8, boolean      |
   +----------+--------------+-------------------------------------------------+----------------------------------------------------------------------------------+----------------------------+
   | Data     | description  | Functional meaning of the signal.               | Prevents ambiguous interpretation across suppliers, functions, and applications. | Vehicle longitudinal speed |
   +----------+--------------+-------------------------------------------------+----------------------------------------------------------------------------------+----------------------------+
   | Data     | unit         | Engineering unit of the signal.                 | Essential for safe interpretation, comparison, and conversion.                   | km/h (SI preferred)        |
   +----------+--------------+-------------------------------------------------+----------------------------------------------------------------------------------+----------------------------+
   | Data     | min          | Engineering minimum representable value.        | Helps bound legal encoding and generated validation checks.                      | 0                          |
   +----------+--------------+-------------------------------------------------+----------------------------------------------------------------------------------+----------------------------+
   | Data     | max          | Engineering maximum representable value.        | Helps bound legal encoding and generated validation checks.                      | 300                        |
   +----------+--------------+-------------------------------------------------+----------------------------------------------------------------------------------+----------------------------+


.. literalinclude:: autoapiframework_metadata_V1.yaml
   :language: yaml
   :linenos:
