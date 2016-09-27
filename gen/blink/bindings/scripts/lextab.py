# lextab.py. This file automatically created by PLY (version 3.4). Don't edit!
_tabversion   = '3.4'
_lextokens    = {'EXCEPTION': 1, 'SHORT': 1, 'CONST': 1, 'SETLIKE': 1, 'MAPLIKE': 1, 'VOID': 1, 'float': 1, 'NAN': 1, 'STATIC': 1, 'REGEXP': 1, 'DATE': 1, 'NULL': 1, 'TRUE': 1, 'SETTER': 1, 'DOMSTRING': 1, 'CREATOR': 1, 'FALSE': 1, 'REQUIRED': 1, 'UNSIGNED': 1, 'LONG': 1, 'READONLY': 1, 'ELLIPSIS': 1, 'SERIALIZER': 1, 'TYPEDEF': 1, 'OBJECT': 1, 'INFINITY': 1, 'string': 1, 'DICTIONARY': 1, 'SEQUENCE': 1, 'LEGACYITERABLE': 1, 'ENUM': 1, 'OCTET': 1, 'INHERIT': 1, 'LEGACYCALLER': 1, 'GETTER': 1, 'integer': 1, 'BYTE': 1, 'OPTIONAL': 1, 'ITERABLE': 1, 'IMPLEMENTS': 1, 'BYTESTRING': 1, 'PARTIAL': 1, 'STRINGIFIER': 1, 'DOUBLE': 1, 'FLOAT': 1, 'OR': 1, 'CALLBACK': 1, 'BOOLEAN': 1, 'PROMISE': 1, 'ATTRIBUTE': 1, 'INTERFACE': 1, 'UNRESTRICTED': 1, 'identifier': 1, 'ANY': 1, 'DELETER': 1}
_lexreflags   = 0
_lexliterals  = '"*.(){}[],;:=+-/~|&^?<>'
_lexstateinfo = {'INITIAL': 'inclusive'}
_lexstatere   = {'INITIAL': [('(?P<t_COMMENT>(/\\*(.|\\n)*?\\*/)|(//.*(\\n[ \\t]*//.*)*))|(?P<t_ELLIPSIS>\\.\\.\\.)|(?P<t_float>-?(([0-9]+\\.[0-9]*|[0-9]*\\.[0-9]+)([Ee][+-]?[0-9]+)?|[0-9]+[Ee][+-]?[0-9]+))|(?P<t_integer>-?([1-9][0-9]*|0[Xx][0-9A-Fa-f]+|0[0-7]*))|(?P<t_LINE_END>\\n+)|(?P<t_string>"[^"]*")|(?P<t_KEYWORD_OR_SYMBOL>_?[A-Za-z][A-Za-z_0-9]*)', [None, ('t_COMMENT', 'COMMENT'), None, None, None, None, ('t_ELLIPSIS', 'ELLIPSIS'), ('t_float', 'float'), None, None, None, ('t_integer', 'integer'), None, ('t_LINE_END', 'LINE_END'), ('t_string', 'string'), ('t_KEYWORD_OR_SYMBOL', 'KEYWORD_OR_SYMBOL')])]}
_lexstateignore = {'INITIAL': ' \t'}
_lexstateerrorf = {'INITIAL': 't_ANY_error'}
