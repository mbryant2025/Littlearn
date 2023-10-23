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
        'name': 'Logic',
        'categorystyle': 'logic_category',
        'contents': [
          {
            'kind': 'block',
            'type': 'controls_if',
          },
          {
            'kind': 'block',
            'type': 'logic_compare',
          },
          {
            'kind': 'block',
            'type': 'logic_operation',
          },
          {
            'kind': 'block',
            'type': 'logic_negate',
          },
          {
            'kind': 'block',
            'type': 'logic_boolean',
          },
        ],
      },
      {
        'kind': 'category',
        'name': 'Loops',
        'categorystyle': 'loop_category',
        'contents': [
          {
            'kind': 'block',
            'type': 'controls_repeat_ext',
            'inputs': {
              'TIMES': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 10,
                  },
                },
              },
            },
          },
          {
            'kind': 'block',
            'type': 'controls_whileUntil',
          },
          {
            'kind': 'block',
            'type': 'controls_for',
            'inputs': {
              'FROM': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 1,
                  },
                },
              },
              'TO': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 10,
                  },
                },
              },
              'BY': {
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
            'type': 'controls_flow_statements',
          },
        ],
      },
      {
        'kind': 'category',
        'name': 'Math',
        'categorystyle': 'math_category',
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
            'type': 'math_arithmetic',
            'inputs': {
              'A': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 1,
                  },
                },
              },
              'B': {
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
            'type': 'math_number_property',
            'inputs': {
              'NUMBER_TO_CHECK': {
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
          {
            'kind': 'block',
            'type': 'math_random_int',
            'inputs': {
              'FROM': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 1,
                  },
                },
              },
              'TO': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 100,
                  },
                },
              },
            },
          },
        ],
      },
      {
        'kind': 'sep',
      },
      {
        'kind': 'category',
        'name': 'Variables',
        'categorystyle': 'variable_category',
        'custom': 'VARIABLE',
      },
      {
        'kind': 'sep',
      },
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
                    'NUM': 9,
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
            'type': 'wait',
            'inputs': {
              'TEXT': {
                'shadow': {
                  'type': 'math_number',
                  'fields': {
                    'NUM': 9,
                  },
                },
              },
            },
          },
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
        ],
      },
    ],
  };
  