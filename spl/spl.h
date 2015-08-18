#ifndef _SPL_INCLUDED_H_
#define _SPL_INCLUDED_H_

typedef int token_t;
typedef enum {
    TOKEN_INVALID         = 0xFFFFFFFF,
    TOKEN_BEGIN           = 0xFFFFFFFE,
    TOKEN_E               = 0xFFFFFFFD,
    TOKEN_GEN             = 0xFFFFFFFC,
    TOKEN_EXT             = 0x08000000,
    TOKEN_NULL            = 0x00000000,
    TOKEN_BLANK           = 0x00010000,
    TOKEN_TYPE            = 0x00001000,
    TOKEN_DEC             = TOKEN_TYPE + 3,
    TOKEN_HEX             = TOKEN_TYPE + 4,
    TOKEN_DOUBLE          = TOKEN_TYPE + 5,
    TOKEN_STRING          = TOKEN_TYPE + 6,
    TOKEN_FUNC            = TOKEN_TYPE + 7,
    TOKEN_EXPR            = TOKEN_TYPE + 8,
    TOKEN_LOGIC_NOT       = TOKEN_TYPE + 10,
    TOKEN_LOGIC_AND       = TOKEN_TYPE + 11,
    TOKEN_LOGIC_OR        = TOKEN_TYPE + 12,
    TOKEN_OP              = 0x00002000,
    TOKEN_OP_PLUSPLUS     = TOKEN_OP | (256 + 1),
    TOKEN_OP_PLUSEQ       = TOKEN_OP | (256 + 2),
    TOKEN_OP_SUBSUB       = TOKEN_OP | (256 + 3),
    TOKEN_OP_SUBEQ        = TOKEN_OP | (256 + 4),
    TOKEN_OP_MULEQ        = TOKEN_OP | (256 + 5),
    TOKEN_OP_DIVEQ        = TOKEN_OP | (256 + 6),
    TOKEN_OP_MODEQ        = TOKEN_OP | (256 + 7),
    TOKEN_OP_OREQ         = TOKEN_OP | (256 + 8),
    TOKEN_OP_ANDEQ        = TOKEN_OP | (256 + 9),
    TOKEN_OP_XOREQ        = TOKEN_OP | (256 + 10),
    TOKEN_OP_LSHIFT       = TOKEN_OP | (256 + 19),
    TOKEN_OP_LSHIFTEQ     = TOKEN_OP | (256 + 20),
    TOKEN_OP_RSHIFT       = TOKEN_OP | (256 + 21), // 
    TOKEN_OP_RSHIFTEQ     = TOKEN_OP | (256 + 22), // >>=
    TOKEN_OP_ASSUME       = TOKEN_OP | (256 + 23), // =
    TOKEN_CMP             = 0x00004000,
    TOKEN_CMP_BG          = TOKEN_CMP | '>',
    TOKEN_CMP_LT          = TOKEN_CMP | '<',
    TOKEN_CMP_EQ          = TOKEN_CMP | (256 + 1),
    TOKEN_CMP_BE          = TOKEN_CMP | (256 + 2),
    TOKEN_CMP_LE          = TOKEN_CMP | (256 + 3),
    TOKEN_CMP_NE          = TOKEN_CMP | (256 + 4),
    TOKEN_KEY             = 0x00008000,
    TOKEN_KEY_IMPORT      = TOKEN_KEY | (256 + 1),
    TOKEN_KEY_FUNCTION    = TOKEN_KEY | (256 + 2),
    TOKEN_KEY_RETURN      = TOKEN_KEY | (256 + 3),
    TOKEN_KEY_IF          = TOKEN_KEY | (256 + 4),
    TOKEN_KEY_ELIF        = TOKEN_KEY | (256 + 5),
    TOKEN_KEY_ELSE        = TOKEN_KEY | (256 + 6),
    TOKEN_KEY_FOR         = TOKEN_KEY | (256 + 7),
    TOKEN_KEY_WHILE       = TOKEN_KEY | (256 + 8),
    TOKEN_KEY_CONTINUE    = TOKEN_KEY | (256 + 7),
    TOKEN_KEY_BREAK       = TOKEN_KEY | (256 + 8),
};

// 符号表节点
struct spl_symbol {
    char *symbol;
    char *symbol_type;

    union {
        char __char_value;
        wchar_t __wchar_value;
        int __int_value;
        long long __longlong_value;
        double __double_value;
    }symbol_value;

    struct spl_symbol *left, *right;
};


// 流式协议语言结构
struct spl_struct {
    FILE *fp, *tmp;
    char file[256];

    // 函数符号
    struct spl_symbol *function_symbols;
    // 全局变量符号
    struct spl_symbol *var_symbols;

    // 库函数符号
    struct spl_symbol *lib_function_symbols;
    // 库变量符号
    struct spl_symbol *lib_var_symbols;
};



#endif // _SPL_INCLUDED_H_