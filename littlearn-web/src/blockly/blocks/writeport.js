
import * as Blockly from 'blockly/core';

// Create a custom block called 'add_text' that adds
// text to the output div on the sample app.
// This is just an example and you should replace this with your
// own custom blocks.
const block = {
  'type': 'fan',
  'message0': 'turn fan on port %1 to %2',
  'args0': [
    {
      'type': 'input_value',
      'name': 'PORT',
      'check': 'Number',
    },
    {
      'type': 'input_value',
      'name': 'VALUE',
      'check': 'Number',
    },
  ],
  'inputsInline': true,
  'previousStatement': null,
  'nextStatement': null,
  'colour': 127,
  'tooltip': 'Turn a fan on or off',
  'helpUrl': '',
};

const block1 = {
  'type': 'LED',
  'message0': 'turn LED panel on port %1 to %2',
  'args0': [
    {
      'type': 'input_value',
      'name': 'PORT',
      'check': 'Number',
    },
    {
      'type': 'input_value',
      'name': 'VALUE',
      'check': 'Number',
    },
  ],
  'inputsInline': true,
  'previousStatement': null,
  'nextStatement': null,
  'colour': 127,
  'tooltip': 'Turn a LED panel on or off',
  'helpUrl': '',
};

// Create the block definitions for the JSON-only blocks.
// This does not register their definitions with Blockly.
// This file has no side effects!
export const writeport = Blockly.common.createBlockDefinitionsFromJsonArray(
    [block, block1]);
