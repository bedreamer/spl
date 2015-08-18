#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include "../spl/spl.h"
struct token_struct {
    int token;
    char name[64];

    int gen_nr;
    struct token_struct *gen;

    struct token_struct *next;
};
#define __const_size__ sizeof(struct token_struct)
int token_base = TOKEN_EXT + 1;
int gen_nr = 0; // 产生式个数

struct __BUILDIN_TOKENS {
	const char *token_name;
	unsigned int token;
}__buidin_tokens[]={
	{"TOKEN_INVALID"        , 0xFFFFFFFF},
	{"TOKEN_BEGIN"           , 0xFFFFFFFE},
	{"TOKEN_E"               , 0xFFFFFFFD},
	{"TOKEN_EXT"             , 0x08000000},
	{"TOKEN_NULL"            , 0x00000000},
	{"TOKEN_BLANK"           , 0x00010000},
	{"TOKEN_TYPE"            , 0x00001000},
	{"TOKEN_DEC"             , TOKEN_TYPE + 3},
	{"TOKEN_HEX"             , TOKEN_TYPE + 4},
	{"TOKEN_DOUBLE"          , TOKEN_TYPE + 5},
	{"TOKEN_STRING"          , TOKEN_TYPE + 6},
	{"TOKEN_FUNC"            , TOKEN_TYPE + 7},
	{"TOKEN_EXPR"            , TOKEN_TYPE + 8},
	{"TOKEN_LOGIC_NOT"       , TOKEN_TYPE + 10},
	{"TOKEN_LOGIC_AND"       , TOKEN_TYPE + 11},
	{"TOKEN_LOGIC_OR"        , TOKEN_TYPE + 12},
	{"TOKEN_OP"              , 0x00002000},
	{"TOKEN_OP_PLUSPLUS"     , TOKEN_OP | (256 + 1)},
	{"TOKEN_OP_PLUSEQ"       , TOKEN_OP | (256 + 2)},
	{"TOKEN_OP_SUBSUB"       , TOKEN_OP | (256 + 3)},
	{"TOKEN_OP_SUBEQ"        , TOKEN_OP | (256 + 4)},
	{"TOKEN_OP_MULEQ"        , TOKEN_OP | (256 + 5)},
	{"TOKEN_OP_DIVEQ"        , TOKEN_OP | (256 + 6)},
	{"TOKEN_OP_MODEQ"        , TOKEN_OP | (256 + 7)},
	{"TOKEN_OP_OREQ"         , TOKEN_OP | (256 + 8)},
	{"TOKEN_OP_ANDEQ"        , TOKEN_OP | (256 + 9)},
	{"TOKEN_OP_XOREQ"        , TOKEN_OP | (256 + 10)},
	{"TOKEN_OP_LSHIFT"       , TOKEN_OP | (256 + 19)},
	{"TOKEN_OP_LSHIFTEQ"     , TOKEN_OP | (256 + 20)},
	{"TOKEN_OP_RSHIFT"       , TOKEN_OP | (256 + 21)}, // 
	{"TOKEN_OP_RSHIFTEQ"     , TOKEN_OP | (256 + 22)}, // >>=
	{"TOKEN_OP_ASSUME"       , TOKEN_OP | (256 + 23)}, // =
	{"TOKEN_CMP"             , 0x00004000},
	{"TOKEN_CMP_BG"          , TOKEN_CMP | '>'},
	{"TOKEN_CMP_LT"          , TOKEN_CMP | '<'},
	{"TOKEN_CMP_EQ"          , TOKEN_CMP | (256 + 1)},
	{"TOKEN_CMP_BE"          , TOKEN_CMP | (256 + 2)},
	{"TOKEN_CMP_LE"          , TOKEN_CMP | (256 + 3)},
	{"TOKEN_CMP_NE"          , TOKEN_CMP | (256 + 4)},
	{"TOKEN_KEY"             , 0x00008000},
	{"TOKEN_KEY_IMPORT"      , TOKEN_KEY | (256 + 1)},
	{"TOKEN_KEY_FUNCTION"    , TOKEN_KEY | (256 + 2)},
	{"TOKEN_KEY_RETURN"      , TOKEN_KEY | (256 + 3)},
	{"TOKEN_KEY_IF"          , TOKEN_KEY | (256 + 4)},
	{"TOKEN_KEY_ELIF"        , TOKEN_KEY | (256 + 5)},
	{"TOKEN_KEY_ELSE"        , TOKEN_KEY | (256 + 6)},
	{"TOKEN_KEY_FOR"         , TOKEN_KEY | (256 + 7)},
	{"TOKEN_KEY_WHILE"       , TOKEN_KEY | (256 + 8)},
	{"TOKEN_KEY_CONTINUE"    , TOKEN_KEY | (256 + 7)},
	{"TOKEN_KEY_BREAK"       , TOKEN_KEY | (256 + 8)},
	{"", 0}
};

unsigned int __search_buildin_token(const char *name)
{
	int i = 0;
	for ( i = 0; __buidin_tokens[i].token_name; i ++ ) {
		if ( 0 == strcmp(__buidin_tokens[i].token_name, name) ) 
			return __buidin_tokens[i].token;
	}
	return TOKEN_INVALID;
}

struct token_struct * __append(struct token_struct **pts, int token, const char *name)
{
    struct token_struct *thiz = NULL;

    thiz = (struct token_struct *)malloc(__const_size__);
    if ( thiz ) {
        memset(thiz, 0, __const_size__);
        thiz->token = token;
        strncpy(thiz->name, name, 64);
        if ( *pts == NULL ) {
            *pts = thiz;
        } else {
            struct token_struct *pos = *pts;
            while ( pos->next ) pos = pos->next;
            pos->next = thiz;
        }
    } else {
        printf("内存分配失败need %d bytes, errno: %d!\n", __const_size__, errno);
    }
    return thiz;
}

struct token_struct *__search_token(struct token_struct *pts, int token)
{
    while ( pts ) {
        if ( pts->token == token ) return pts;
        //if ( pts->gen ) return __search_token(pts->gen, token);
        return __search_token(pts->next, token);
    }
    return NULL;
}

struct token_struct *__search_name(struct token_struct *pts, const char *name)
{
    while ( pts ) {
        if ( 0 == strcmp(pts->name, name ) ) return pts;
        //if ( pts->gen ) return __search_name(pts->gen, name);
        return __search_name(pts->next, name);
    }
    return NULL;
}

int main(int argc, const char *argv[])
{
    char token[256];
    char gen_token[256];
    char *to = token;
    int mode = 0;
    int len = 0;
    int gen = 0;
    int member = 0;
    struct token_struct *father = NULL;
    struct token_struct *thiz = NULL;
    struct token_struct *copy = NULL;

    FILE *fp = fopen(argv[1], "r+");
    if ( !fp ) return -1;

    while ( ! feof(fp) ) {
        char ch;
        int nr = fread(&ch, 1, 1, fp);
        if ( nr <= 0 ) break;

        switch ( ch ) {
        case ':':
            if ( len ) {
                copy = __search_name(father, token);
                if ( copy ) {
                    thiz = copy;
                    printf("先导终结符: %X-%s\n", copy->token, token);
                } else {
                    thiz = __append(&father, token_base ++, token);
                    printf("终结符: %X-%s\n", token_base-1, token);
                }
            }
            gen = 0;
            to = token;
            len = 0;
            member = 0;
        break;
        case '#':
            member = 0;
            gen ++;
            gen_nr ++;
            do {
                char genname[64];
                sprintf(genname, "gen_%d", gen_nr);

                thiz = __append(&thiz->gen, TOKEN_GEN + gen_nr, genname);
            } while (0);
            printf("\t产生式 %d: \n", gen);
            to = token;
            len = 0;
            member = 0;
        break;
        case ' ':
        case '\'':
        case '\t':
        case '\n':
        case '\r':
            if ( len ) {
                member ++;
                printf("\t\t成员%d: %s", member, token);
                if ( token[0] == 'T' && token[1] == '_' ) {
                    copy = __search_name(father, token);
                    if ( ! copy ) {
                        printf(" [*]");
                        __append(&father, token_base ++, token);
                        __append(&thiz->next, token_base, token);
                        thiz->gen_nr ++;
                    }
                } else if ( token[0] == 'T' && token[1] == 'O' &&
                    token[2] == 'K' && token[3] == 'E' &&
                    token[4] == 'N' && token[5] == '_' ) {
                    unsigned int t;
                    // 多元内建
                    t = __search_buildin_token(token);
                    __append(&thiz->next, t, token);
                    thiz->gen_nr ++;
                } else {
                    // 单字符
                    __append(&thiz->next, token[0], token);
                    thiz->gen_nr ++;
                }
                printf("\n");
            }
            len = 0;
            to = token;
        break;
        default:
            *to ++ = ch;
            *to = '\0';
            len ++;
        break;
        }
    }

    do {
        struct token_struct *gen = father;

        while ( gen ) {
            printf("#define %-20s  0x%08X\n", gen->name, gen->token);
            gen = gen->next;
        }

        gen = father;
        while ( gen ) {
            if ( gen->gen_nr ) {
                printf("unsigned int gen_%s[] = {}\n",
                    gen->name);
            }
            gen = gen->next;
        }

    } while (0);

    return 0;
}