
import * as Blockly from 'blockly/core';

// Create a custom block called 'add_text' that adds
// text to the output div on the sample app.
// This is just an example and you should replace this with your
// own custom blocks.
const block = {
  'type': 'button',
  'message0': 'get button on port %1',
  'args0': [
    {
      'type': 'input_value',
      'name': 'TEXT',
      'check': 'Number',
      'value': 0, // default value
    },
  ],
  'output': 'Number',
  'tooltip': 'Outputs 1 if button is pressed, 0 otherwise',
  'helpUrl': '',
  'colour': 127,
};

const block1 = {
  'type': 'switch',
  'message0': 'get switch on port %1',
  'args0': [
    {
      'type': 'input_value',
      'name': 'TEXT',
      'check': 'Number',
    },
  ],
  //make the block output a boolean
  'output': 'Number',
  'tooltip': 'Outputs 1 if switch is on, 0 otherwise',
  'helpUrl': '',
  'colour': 127,
};

const block2 = {
  'type': 'motion',
  'message0': 'get motion sensor on port %1',
  'args0': [
    {
      'type': 'input_value',
      'name': 'TEXT',
      'check': 'Number',
    },
  ],
  //make the block output a boolean
  'output': 'Number',
  'tooltip': 'Outputs 1 if motion is detected, 0 otherwise',
  'helpUrl': '',
  'colour': 127,
};

// Create the block definitions for the JSON-only blocks.
// This does not register their definitions with Blockly.
// This file has no side effects!
export const readport = Blockly.common.createBlockDefinitionsFromJsonArray(
    [block, block1, block2]);
