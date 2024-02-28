
import * as Blockly from 'blockly/core';

// Create a custom block called 'add_text' that adds
// text to the output div on the sample app.
// This is just an example and you should replace this with your
// own custom blocks.
const block = {
  'type': 'sevenseg',
  'message0': 'set 7-segment display to %1',
  'args0': [
    {
      'type': 'input_value',
      'name': 'TEXT',
      'check': 'Number',
    },
  ],
  'previousStatement': null,
  'nextStatement': null,
  'tooltip': '',
  'helpUrl': '',
  'colour': 127,
};

// Create the block definitions for the JSON-only blocks.
// This does not register their definitions with Blockly.
// This file has no side effects!
export const sevenseg = Blockly.common.createBlockDefinitionsFromJsonArray(
    [block]);
