/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */

/*
This toolbox contains nearly every single built-in block that Blockly offers,
in addition to the custom block 'add_text' this sample app adds.
You probably don't need every single block, and should consider either rewriting
your toolbox from scratch, or carefully choosing whether you need each block
listed here.
*/

export const toolbox = {
    'kind': 'categoryToolbox',
    'contents': [
      {
        'kind': 'category',
        'name': 'I/O',
        'categorystyle': 'text_category',
        'contents': [
          {
            'kind': 'block',
            'type': 'print',
            'inputs': {
              'TEXT': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 0,
                  },
                },
              },
            },
          },
          {
            'kind': 'block',
            'type': 'sevenseg',
            'inputs': {
              'TEXT': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 1234,
                  },
                },
              },
            },
          },
          {
            'kind': 'block',
            'type': 'button',
            'inputs': {
              'TEXT': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 1,
                  },
                },
              },
            },
          },
          {
            'kind': 'block',
            'type': 'switch',
            'inputs': {
              'TEXT': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 2,
                  },
                },
              },
            },
          },
          {
            'kind': 'block',
            'type': 'motion',
            'inputs': {
              'TEXT': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 3,
                  },
                },
              },
            },
          },
          {
            'kind': 'block',
            'type': 'fan',
            'inputs': {
              'PORT': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 0,
                  },
                },
              },
              'VALUE': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 0,
                  },
                },
              },
            },
          },
          {
            'kind': 'block',
            'type': 'LED',
            'inputs': {
              'PORT': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 0,
                  },
                },
              },
              'VALUE': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 0,
                  },
                },
              },
            },
          },
        ],
      },
      {
        'kind': 'category',
        'name': 'Control',
        'categorystyle': 'text_category',
        'contents': [
          {
            'kind': 'block',
            'type': 'if',
            'inputs': {
              'CONDITION': {
                'type': 'math_number',
                'fields': {
                  'NUM': 9,
                },
              },
              'DO': {
                'type': 'statement',
              },
            },
          },
          {
            'kind': 'block',
            'type': 'while',
            'inputs': {
              'CONDITION': {
                'type': 'math_number',
                'fields': {
                  'NUM': 9,
                },
              },
              'DO': {
                'type': 'statement',
              },
            },
          },
          {
            'kind': 'block',
            'type': 'wait',
            'inputs': {
              'TEXT': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 500,
                  },
                },
              },
            },
          },
        ],
      },
      {
        'kind': 'category',
        'name': 'Math',
        'categorystyle': 'text_category',
        'contents': [
          {
            'kind': 'block',
            'type': 'math_number',
            'fields': {
              'NUM': 123,
            },
          },
          {
            'kind': 'block',
            'type': 'binaryOp',
          },
          {
            'kind': 'block',
            'type': 'math_modulo',
            'inputs': {
              'DIVIDEND': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 64,
                  },
                },
              },
              'DIVISOR': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 10,
                  },
                },
              },
            },
          },
         
        ],
      },
      {
        'kind': 'category',
        'name': 'Variables',
        'categorystyle': 'text_category',
        'contents': [
          {
            'kind': 'button',
            'text': 'Create New Variable',
            'callbackKey': 'CREATE_INT_VARIABLE',
          },
          {
            'kind': 'block',
            'type': 'int_var_declaration',
          },
          {
            'kind': 'block',
            'type': 'float_var_declaration',
          },
          {
            'kind': 'block',
            'type': 'use_variable', // Add the 'use_variable' block to the toolbox
          },
          {
            'kind': 'block',
            'type': 'assign_variable',
          },
        ]
      }
    ],
  };
  