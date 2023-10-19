
import * as Blockly from 'blockly/core';

// Create a custom block called 'add_text' that adds
// text to the output div on the sample app.
// This is just an example and you should replace this with your
// own custom blocks.
const print = {
  'type': 'set_int',
  'message0': 'int %1 in %2',
  'args0': [
    {
      'type': 'input_value',
      'name': 'TEXT',
      'check': 'String',
    },
    {
        'type': 'input_value',
        'name': 'NUM',
        'check': 'Number',
    },
  ],
  'colour': 90,
  'inputsInline': true,
};

// Create the block definitions for the JSON-only blocks.
// This does not register their definitions with Blockly.
// This file has no side effects!
export const blocks4 = Blockly.common.createBlockDefinitionsFromJsonArray(
    [print]);
