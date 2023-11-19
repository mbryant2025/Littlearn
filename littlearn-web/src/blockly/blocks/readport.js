
import * as Blockly from 'blockly/core';

const button = {
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

const swtch = {
  'type': 'switch',
  'message0': 'get switch on port %1',
  'args0': [
    {
      'type': 'input_value',
      'name': 'TEXT',
      'check': 'Number',
    },
  ],
  'output': 'Number',
  'tooltip': 'Outputs 1 if switch is on, 0 otherwise',
  'helpUrl': '',
  'colour': 127,
};

const motion = {
  'type': 'motion',
  'message0': 'get motion sensor on port %1',
  'args0': [
    {
      'type': 'input_value',
      'name': 'TEXT',
      'check': 'Number',
    },
  ],
  'output': 'Number',
  'tooltip': 'Outputs 1 if motion is detected, 0 otherwise',
  'helpUrl': '',
  'colour': 127,
};

const beam = {
  'type': 'beam',
  'message0': 'get beam break sensor on port %1',
  'args0': [
    {
      'type': 'input_value',
      'name': 'TEXT',
      'check': 'Number',
    },
  ],
  'output': 'Number',
  'tooltip': 'Outputs 1 if motion is detected, 0 otherwise',
  'helpUrl': '',
  'colour': 127,
};

export const readport = Blockly.common.createBlockDefinitionsFromJsonArray(
    [button, swtch, motion, beam]);
