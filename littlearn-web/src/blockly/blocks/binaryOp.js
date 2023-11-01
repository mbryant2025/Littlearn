import * as Blockly from 'blockly/core';

// Create a custom block for performing binary operations.
const binaryOp = {
  'type': 'binaryOp',
  'message0': '%1 %2 %3',
  'args0': [
    {
      'type': 'input_value',
      'name': 'A',
      'check': 'Number',
    },
    {
      'type': 'field_dropdown',
      'name': 'OPERATOR',
      'options': [
        ['+', '+'],
        ['-', '-'],
        ['*', '*'],
        ['/', '/'],
        ['%', '%'],
        ['>', '>'],
        ['<', '<'],

      ],
    },
    {
      'type': 'input_value',
      'name': 'B',
      'check': 'Number',
    },
  ],
  'inputsInline': true,
  'output': 'Number',
  //blockly math color
  'colour': 230,
  'tooltip': '',
  'helpUrl': '',
};

// Create the block definitions for the JSON-only blocks.
export const binaryOpBlock = Blockly.common.createBlockDefinitionsFromJsonArray([binaryOp]);
