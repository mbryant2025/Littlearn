import React, { useEffect } from 'react';
import * as Blockly from 'blockly';
import { printBlock } from '../blockly/blocks/print';
import { waitBlock } from '../blockly/blocks/wait';
import { ifBlock } from '../blockly/blocks/if';
import { binaryOpBlock } from '../blockly/blocks/binaryOp';
import { forBlock } from '../blockly/generators/javascript';
import { javascriptGenerator } from 'blockly/javascript';
import { toolbox } from '../blockly/toolbox';


const BlocklyComponent: React.FC = () => {
  useEffect(() => {
    // Register the custom blocks and generator
    Blockly.common.defineBlocks(printBlock);
    Blockly.common.defineBlocks(waitBlock);
    Blockly.common.defineBlocks(ifBlock);
    Blockly.common.defineBlocks(binaryOpBlock);

    Object.assign(javascriptGenerator.forBlock, forBlock);


    const workspace = Blockly.inject('blocklyDiv', {toolbox: toolbox});

    // This function resets the code and output divs, shows the
    // generated code from the workspace, and evals the code.
    // In a real application, you probably shouldn't use `eval`.
    const runCode = () => {
      const code = javascriptGenerator.workspaceToCode(workspace);
      console.log(code);

      //set the code in the code div
      const element = document.getElementById('generatedCode');
        if (element) {
            //set innerHTML to code while maintaining formatting
            element.innerHTML = code.replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/\n/g, '<br>');
        }

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
      <div id="generatedCode" style={{ position: 'absolute', top: '0px', left: '500px'}}></div>
    </div>
  );
};

export default BlocklyComponent;
