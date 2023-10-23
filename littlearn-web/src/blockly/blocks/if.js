import * as Blockly from 'blockly/core';

// Create a custom block called 'if' that has a body.
const ifStatement = {
  'type': 'if',
  'message0': 'if %1',
  'args0': [
    {
      'type': 'input_value',
      'name': 'CONDITION',
      'check': 'Number',
    },
  ],
  'message1': '%1',
  'args1': [
    {
      'type': 'input_statement',
      'name': 'DO',
    },
  ],
  'previousStatement': null,
  'nextStatement': null,
  'colour': 160,
  'tooltip': '',
  'helpUrl': '',
};

// Create the block definitions for the JSON-only blocks.
export const ifBlock = Blockly.common.createBlockDefinitionsFromJsonArray(
  [ifStatement]
);
