import * as Blockly from 'blockly/core';

// Create a custom block for declaring an integer variable in code.
const intVarDeclaration = {
  'type': 'int_var_declaration',
  'message0': 'Declare int variable %1 = %2',
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
export const intVarDeclarationBlock = Blockly.common.createBlockDefinitionsFromJsonArray([intVarDeclaration]);

const useVariableBlock = {
    'type': 'use_variable',
    'message0': '%1',
    'args0': [
      {
        'type': 'field_variable',
        'name': 'VAR',
        'variable': 'my_variable', // Default variable name
      },
    ],
    'output': 'Number', // Set the output type to 'Number' or the appropriate data type
    'colour': 160,
    'tooltip': '',
    'helpUrl': '',
  };
  
  // Create the block definition for the JSON-only block.
  export const useVariableBlockDef = Blockly.common.createBlockDefinitionsFromJsonArray([useVariableBlock]);

  const assignVariableBlock = {
    'type': 'assign_variable',
    'message0': '%1 = %2',
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
  export const assignVariableBlockDef = Blockly.common.createBlockDefinitionsFromJsonArray([assignVariableBlock]);
