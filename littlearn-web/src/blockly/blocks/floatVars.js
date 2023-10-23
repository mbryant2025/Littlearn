import * as Blockly from 'blockly/core';

// Create a custom block for declaring an integer variable in code.
const floatVarDeclaration = {
  'type': 'float_var_declaration',
  'message0': 'Declare float variable %1 = %2',
  'args0': [
    {
      'type': 'field_variable',
      'name': 'VAR',
      'variable': 'my_variable',
    },
    {
      'type': 'input_value',
      'name': 'VALUE',
      'check': 'Number',
    },
  ],
  'previousStatement': null,
  'nextStatement': null,
  'colour': 160,
  'tooltip': '',
  'helpUrl': '',
};

// Create the block definitions for the JSON-only blocks.
export const floatVarDeclarationBlock = Blockly.common.createBlockDefinitionsFromJsonArray([floatVarDeclaration]);
