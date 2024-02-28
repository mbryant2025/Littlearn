import React, { useEffect } from 'react';
import * as Blockly from 'blockly';
import { load, save } from '../blockly/serialization';
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
import { forBlock } from '../blockly/generators/codeGen';
import { javascriptGenerator } from 'blockly/javascript';
import { toolbox } from '../blockly/toolbox';
import './styles/Blockly.css';
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

    // Load the workspace from local storage
    load(workspace);

    // Every time the workspace changes state, save the changes to storage.
    workspace.addChangeListener((e) => {
      if (e.isUiEvent) return;
      save(workspace);
    });

    // Whenever the workspace changes meaningfully, run the code again.
    workspace.addChangeListener((e) => {
      // Don't run the code when the workspace finishes loading; we're
      // already running it once when the application starts.
      // Don't run the code during drags; we might have invalid state.
      if (e.isUiEvent || e.type === Blockly.Events.FINISHED_LOADING ||
        workspace.isDragging()) {
        return;
      }
      runCode();
    });

    // Add event listener for the 'CREATE_INT_VARIABLE' button
    workspace.registerButtonCallback('CREATE_INT_VARIABLE', function () {
      console.log('Create int variable button clicked!');
      const variableName = prompt('Enter the variable name:');
      if (variableName) {
        // Create a new variable using Blockly's createVariable function
        workspace.createVariable(variableName, 'Number'); // 'Number' indicates the type of the variable
      }
      //print workspace variables
      console.log(workspace.getAllVariables());
    });

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
    <div className="block-chop">
      <div id="blocklyDiv" style={{ height: '100%', width: '100%' }}></div>
      <div id="blocklyToolboxDiv" style={{ display: 'none' }}></div>
    </div>
  );
};

export default BlocklyComponent;
