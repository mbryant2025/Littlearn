/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */

import {Order} from 'blockly/javascript';
import * as Blockly from 'blockly/core';


// Export all the code generators for our custom blocks,
// but don't register them with Blockly yet.
// This file has no side effects!
export const forBlock = Object.create(null);

forBlock['add_text'] = function (block, generator) {
  const text = generator.valueToCode(block, 'TEXT', Order.NONE) || "''";
  const color =
    generator.valueToCode(block, 'COLOR', Order.ATOMIC) || "'#ffffff'";

  const addText = generator.provideFunction_(
      'addText',
      `function ${generator.FUNCTION_NAME_PLACEHOLDER_}(text, color) {

  // Add text to the output area.
  const outputDiv = document.getElementById('output');
  const textEl = document.createElement('p');
  textEl.innerText = text;
  textEl.style.color = color;
  outputDiv.appendChild(textEl);
}`
  );
  // Generate the function call for this block.
  const code = `${addText}(${text}, ${color});\n`;
  return code;
};

forBlock['print'] = function (block, generator) {
  const text = generator.valueToCode(block, 'TEXT', Order.NONE) || "''";
  // Generate the function call for this block.
  const code = `print(${text});\n`;
  return code;
};

forBlock['wait'] = function (block, generator) {
  const text = generator.valueToCode(block, 'TEXT', Order.NONE) || "''";
  // Generate the function call for this block.
  const code = `wait(${text});\n`;
  return code;
};

forBlock['set_int'] = function (block, generator) {
  const text = generator.valueToCode(block, 'TEXT', Order.NONE) || "''";
  const num = generator.valueToCode(block, 'NUM', Order.NONE) || "''";
  // Generate the function call for this block.
  const code = `int (${text}) = (${num});\n`;
  return code;
};

forBlock['sevenseg'] = function (block, generator) {
  const text = generator.valueToCode(block, 'TEXT', Order.NONE) || "''";
  // Generate the function call for this block.
  const code = `print_seven_segment(${text});\n`;
  return code;
};

forBlock['button'] = function (block, generator) {
  const text = generator.valueToCode(block, 'TEXT', Order.NONE) || "''";
  // Generate the function call for this block.
  const code = `read_port(${text})`;
  return [code, 0];
};

forBlock['switch'] = function (block, generator) {
  const text = generator.valueToCode(block, 'TEXT', Order.NONE) || "''";
  // Generate the function call for this block.
  const code = `read_port(${text})`;
  return [code, 0];
};

forBlock['motion'] = function (block, generator) {
  const text = generator.valueToCode(block, 'TEXT', Order.NONE) || "''";
  // Generate the function call for this block.
  const code = `read_port(${text})`;
  return [code, 0];
};

forBlock['fan'] = function (block, generator) {
  let port = generator.valueToCode(block, 'PORT', Order.NONE);
  let value = generator.valueToCode(block, 'VALUE', Order.NONE);
  // Generate the function call for this block.
  const code = `write_port(${port}, ${value});\n`;
  return code;
};

forBlock['LED'] = function (block, generator) {
  let port = generator.valueToCode(block, 'PORT', Order.NONE);
  let value = generator.valueToCode(block, 'VALUE', Order.NONE);
  // Generate the function call for this block.
  const code = `write_port(${port}, ${value};\n)`;
  return code;
};

forBlock['if'] = function (block, generator) {
  // Generate code for the 'if' block
  var condition = generator.valueToCode(block, 'CONDITION', Order.NONE);

  // If condition is blank, set it to 0
  if (!condition) {
    condition = '0';
  }

  var doBody = generator.statementToCode(block, 'DO');

  var code = 'if (' + condition + ') {\n' + doBody + '}';
  
  return code;
};

forBlock['binaryOp'] = function (block, generator) {
  const operator = block.getFieldValue('OPERATOR');
  
  let valueA = generator.valueToCode(block, 'A', Order.NONE);
  let valueB = generator.valueToCode(block, 'B', Order.NONE);

  // If either value is blank, set it to 0

  if (!valueA) {
    valueA = '0';
  }

  if (!valueB) {
    valueB = '0';
  }

  let code = '';

  // Create the code based on the selected operator
  switch (operator) {
    case '+':
      code = `${valueA} + ${valueB}`;
      break;
    case '-':
      code = `${valueA} - ${valueB}`;
      break;
    case '*':
      code = `${valueA} * ${valueB}`;
      break;
    case '/':
      code = `${valueA} / ${valueB}`;
      break;
    case '%':
      code = `${valueA} % ${valueB}`;
      break;
    case '>':
      code = `${valueA} > ${valueB}`;
      break;
    case '<':
      code = `${valueA} < ${valueB}`;
      break;
    default:
      throw new Error('Unknown operator: ' + operator);
  }

  return [code, Order.ATOMIC];
};

forBlock['while'] = function (block, generator) {
  // Generate code for the 'if' block
  var condition = generator.valueToCode(block, 'CONDITION', Order.NONE);

  // If condition is blank, set it to 0
  if (!condition) {
    condition = '0';
  }

  var doBody = generator.statementToCode(block, 'DO');

  var code = 'while (' + condition + ') {\n' + doBody + '}';
  
  return code;
};

forBlock['int_var_declaration'] = function (block, generator) {
  // Get the variable name from the 'VAR' field
  const variableName = generator.nameDB_.getName(
      block.getFieldValue('VAR'), Blockly.VARIABLE_CATEGORY_NAME);

  // Get the initial value (if provided)
  const initialValue = generator.valueToCode(block, 'VALUE', Order.ASSIGNMENT) || '0';

  // Generate the code to declare the integer variable
  const code = `int ${variableName} = ${initialValue};\n`;

  return code;
};

forBlock['use_variable'] = function (block, generator) {
  // Get the variable name from the 'VAR' field
  const variableName = generator.nameDB_.getName(
    block.getFieldValue('VAR'), Blockly.VARIABLE_CATEGORY_NAME);

  // Generate the code to use the variable
  const code = `${variableName}`;

  return [code, Order.ATOMIC];
};
