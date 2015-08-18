spl
=========

流式协议语言(STREAM PROTOCOL LANGUAGE), 是进行流式协议解析，生成，监控
为一体的解释型语言。

词法
=========
  Number
   2base  [01]+b
   8base  0[1-7]+o
   10base [1-9]+[0-9]*
   16base 0[xX][0-9a-fA-F]+
Operator
   +
   -
   *
   /
   %
   &
   !
   =
   |
   ^
   ++
   +=
   --
   -=
   *=
   /=
Compare
   >
   <
   ==
   >=
   <=
   !=
Terminal
   {}[]()
Logic
   &&
   ||
Express
   [_a-zA-Z]+[0-9a-zA-Z_]*
String
   \".*\"
Comment
   \/\/.?
Key Words
   function
   return
   if
   elif
   else
   for
   while

[TOKEN_EXPR TOKEN_KEY_BLANK [, TOKEN_KEY_BLANK TOKEN_EXPR]?]?