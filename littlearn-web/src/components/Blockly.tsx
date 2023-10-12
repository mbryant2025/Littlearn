import React, { useEffect } from 'react';
import * as Blockly from 'blockly';
import { blocks } from '../blockly/blocks/text';
import { forBlock } from '../blockly/generators/javascript';
import { javascriptGenerator } from 'blockly/javascript';
import { toolbox } from '../blockly/toolbox';

const BlocklyComponent: React.FC = () => {
  useEffect(() => {
    // Register the custom blocks and generator
    Blockly.common.defineBlocks(blocks); // Use your blocks array
    Object.assign(javascriptGenerator.forBlock, forBlock);
    console.log(blocks);

    const workspace = Blockly.inject('blocklyDiv', {toolbox: toolbox});

    // This function resets the code and output divs, shows the
    // generated code from the workspace, and evals the code.
    // In a real application, you probably shouldn't use `eval`.
    const runCode = () => {
      const code = javascriptGenerator.workspaceToCode(workspace);
      console.log(code);

      // Add your code execution logic here
    };

    runCode();

    // Whenever the workspace changes meaningfully, run the code again.
    workspace.addChangeListener(() => {
      runCode();
    });
  }, []);

  return (
    <div>
      <div id="blocklyDiv" style={{ height: '480px', width: '600px' }}></div>
      <div id="blocklyToolboxDiv" style={{ display: 'none' }}></div>
      <div id="generatedCode"></div>
      <div id="output"></div>
    </div>
  );
};

export default BlocklyComponent;
