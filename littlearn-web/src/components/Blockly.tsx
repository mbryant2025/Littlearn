import React, { useEffect } from 'react';
import * as Blockly from 'blockly/core';
import { blocks } from '../blockly/blocks/text'; // Import your blocks
import { forBlock } from '../blockly/generators/javascript';
import { javascriptGenerator } from 'blockly/javascript';
import { save, load } from '../blockly/serialization';
import { toolbox } from '../blockly/toolbox';

const toolboxXML = `
  <xml id="toolbox" style="display: none">
    <block type="add_text"></block>
    <!-- Add other custom blocks to the toolbox as needed -->
  </xml>
`;

const BlocklyComponent: React.FC = () => {
  useEffect(() => {
    // Register the custom blocks and generator
    Blockly.defineBlocksWithJsonArray(blocks); // Use your blocks array
    Object.assign(javascriptGenerator.forBlock, forBlock);

    const workspace = Blockly.inject('blocklyDiv', {
      toolbox: toolboxXML,
    });

    // This function resets the code and output divs, shows the
    // generated code from the workspace, and evals the code.
    // In a real application, you probably shouldn't use `eval`.
    const runCode = () => {
      const code = javascriptGenerator.workspaceToCode(workspace);
      console.log(code);

      // Add your code execution logic here
    };

    // Load the initial state from storage and run the code.
    load(workspace);
    runCode();

    // Whenever the workspace changes meaningfully, run the code again.
    workspace.addChangeListener(() => {
      runCode();
    });
  }, []);

  return (
    <div>
      <div id="blocklyDiv" style={{ height: '480px', width: '600px' }}></div>
      <div dangerouslySetInnerHTML={{ __html: toolboxXML }} />
      <div id="generatedCode"></div>
      <div id="output"></div>
    </div>
  );
};

export default BlocklyComponent;
