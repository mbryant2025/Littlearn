import React, { useEffect } from 'react';
import * as Blockly from 'blockly';
import { printBlock } from '../blockly/blocks/print';
import { waitBlock } from '../blockly/blocks/wait';
import { ifBlock } from '../blockly/blocks/if';
import { binaryOpBlock } from '../blockly/blocks/binaryOp';
import { whileBlock } from '../blockly/blocks/while';
import { sevenseg } from '../blockly/blocks/sevenseg';
import { readport } from "../blockly/blocks/readport";
import { writeport } from "../blockly/blocks/writeport";
import { intVarDeclarationBlock, useVariableBlockDef, assignVariableBlockDef } from '../blockly/blocks/intVars';
import { floatVarDeclarationBlock } from '../blockly/blocks/floatVars';
import { forBlock } from '../blockly/generators/javascript';
import { javascriptGenerator } from 'blockly/javascript';
import { toolbox } from '../blockly/toolbox';

import { useGeneratedCode } from '../GeneratedCodeContext';


const BlocklyComponent: React.FC = () => {

  const { setGeneratedCode } = useGeneratedCode();


  useEffect(() => {
    // Register the custom blocks and generator
    Blockly.common.defineBlocks(printBlock);
    Blockly.common.defineBlocks(waitBlock);
    Blockly.common.defineBlocks(ifBlock);
    Blockly.common.defineBlocks(binaryOpBlock);
    Blockly.common.defineBlocks(whileBlock);
    Blockly.common.defineBlocks(intVarDeclarationBlock);
    Blockly.common.defineBlocks(useVariableBlockDef);
    Blockly.common.defineBlocks(sevenseg);
    Blockly.common.defineBlocks(readport);
    Blockly.common.defineBlocks(writeport);
    Blockly.common.defineBlocks(floatVarDeclarationBlock);
    Blockly.common.defineBlocks(assignVariableBlockDef);

    Object.assign(javascriptGenerator.forBlock, forBlock);

    const workspace = Blockly.inject('blocklyDiv', { toolbox: toolbox });

    // This function resets the code and output divs, shows the
    // generated code from the workspace, and evals the code.
    // In a real application, you probably shouldn't use `eval`.
    const runCode = () => {
      let code = javascriptGenerator.workspaceToCode(workspace);

      // Hacky workaround for the existing JS implementation
      // If there are any variables, delete the first two lines

      if (code.includes('var ')) {
        code = code.split('\n').slice(3).join('\n');
      }

      //set the code in the generatedCode context
      setGeneratedCode(code.replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/\n/g, '<br>'));

    };

    runCode();

    // Whenever the workspace changes meaningfully, run the code again.
    workspace.addChangeListener(() => {
      runCode();
    });
  }, [setGeneratedCode]);


  return (
    <div>
      <div id="blocklyDiv" style={{ height: '480px', width: '100%' }}></div>
      <div id="blocklyToolboxDiv" style={{ display: 'none' }}></div>
      {/* <div id="generatedCode" style={{ position: 'absolute', top: '0px', left: '500px' }}></div> */}
    </div>
  );
};

export default BlocklyComponent;
