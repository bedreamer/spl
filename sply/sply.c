#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include "../spl/spl.h"
#include "stack.h"
int gen_base = TOKEN_EXT + 1;

struct symbol_item {
    const char *symbol;
    int value;
} token_define[]={
    {"TOKEN_INVALID", 0xFFFFFFFF},
    {"TOKEN_BEGIN", 0xFFFFFFFE},
    {"TOKEN_E", 0xFFFFFFFD},
    {"TOKEN_GEN", 0xFFFFFFFC},
    {"TOKEN_EXT", 0x08000000},
    {"TOKEN_NULL", 0x00000000},
    {"TOKEN_BLANK", 0x00010000},
    {"TOKEN_TYPE", 0x00001000},
    {"TOKEN_DEC", TOKEN_TYPE + 3},
    {"TOKEN_HEX", TOKEN_TYPE + 4},
    {"TOKEN_DOUBLE", TOKEN_TYPE + 5},
    {"TOKEN_STRING", TOKEN_TYPE + 6},
    {"TOKEN_FUNC", TOKEN_TYPE + 7},
    {"TOKEN_EXPR", TOKEN_TYPE + 8},
    {"TOKEN_LOGIC_NOT", TOKEN_TYPE + 10},
    {"TOKEN_LOGIC_AND", TOKEN_TYPE + 11},
    {"TOKEN_LOGIC_OR", TOKEN_TYPE + 12},
    {"TOKEN_OP", 0x00002000},
    {"TOKEN_OP_PLUSPLUS", TOKEN_OP | (256 + 1)},
    {"TOKEN_OP_PLUSEQ", TOKEN_OP | (256 + 2)},
    {"TOKEN_OP_SUBSUB", TOKEN_OP | (256 + 3)},
    {"TOKEN_OP_SUBEQ", TOKEN_OP | (256 + 4)},
    {"TOKEN_OP_MULEQ", TOKEN_OP | (256 + 5)},
    {"TOKEN_OP_DIVEQ", TOKEN_OP | (256 + 6)},
    {"TOKEN_OP_MODEQ", TOKEN_OP | (256 + 7)},
    {"TOKEN_OP_OREQ", TOKEN_OP | (256 + 8)},
    {"TOKEN_OP_ANDEQ", TOKEN_OP | (256 + 9)},
    {"TOKEN_OP_XOREQ", TOKEN_OP | (256 + 10)},
    {"TOKEN_OP_LSHIFT", TOKEN_OP | (256 + 19)},
    {"TOKEN_OP_LSHIFTEQ", TOKEN_OP | (256 + 20)},
    {"TOKEN_OP_RSHIFT", TOKEN_OP | (256 + 21)},
    {"TOKEN_OP_RSHIFTEQ", TOKEN_OP | (256 + 22)},
    {"TOKEN_OP_ASSUME", TOKEN_OP | (256 + 23)},
    {"TOKEN_CMP", 0x00004000},
    {"TOKEN_CMP_BG", TOKEN_CMP | '>'},
    {"TOKEN_CMP_LT", TOKEN_CMP | '<'},
    {"TOKEN_CMP_EQ", TOKEN_CMP | (256 + 1)},
    {"TOKEN_CMP_BE", TOKEN_CMP | (256 + 2)},
    {"TOKEN_CMP_LE", TOKEN_CMP | (256 + 3)},
    {"TOKEN_CMP_NE", TOKEN_CMP | (256 + 4)},
    {"TOKEN_KEY", 0x00008000},
    {"TOKEN_KEY_IMPORT", TOKEN_KEY | (256 + 1)},
    {"TOKEN_KEY_FUNCTION", TOKEN_KEY | (256 + 2)},
    {"TOKEN_KEY_RETURN", TOKEN_KEY | (256 + 3)},
    {"TOKEN_KEY_IF", TOKEN_KEY | (256 + 4)},
    {"TOKEN_KEY_ELIF", TOKEN_KEY | (256 + 5)},
    {"TOKEN_KEY_ELSE", TOKEN_KEY | (256 + 6)},
    {"TOKEN_KEY_FOR", TOKEN_KEY | (256 + 7)},
    {"TOKEN_KEY_WHILE", TOKEN_KEY | (256 + 8)},
    {"TOKEN_KEY_CONTINUE", TOKEN_KEY | (256 + 7)},
    {"", 0}
};


struct sply_unit {
    // 非终结符值
    int value;
    // 非终结符
    char *symbol;

    // 叶子个数
    int leaf_count;
    // 分支个数
    int branched_count;

    // 非终结符
    struct sply_unit *next_leaf;
    // 产生式
    struct sply_unit *next_branch;
    // 下一个非终结符
    struct sply_unit *next_tree;
};
struct symbol_item *search_symbol(const char *symbol)
{
    struct symbol_item *item = token_define;

    for ( ; item->symbol; item ++ ) {
        if ( 0 == strcmp(symbol, item->symbol) ) return item;
    }

    return NULL;
}
struct sply_unit *sply_append_tree(struct sply_unit **sply, int v, char *symbol)
{
    int malloc_size = sizeof(struct sply_unit) + strlen(symbol) + 1;
    struct sply_unit * thiz = (struct sply_unit*)malloc(malloc_size);
    memset(thiz, 0, malloc_size);
    thiz->value = v;
    thiz->symbol = (char*)(thiz) + sizeof(struct sply_unit);
    strcpy(thiz->symbol, symbol);
    while ( *sply ) {
        sply = &((*sply)->next_tree);
    }
    *sply = thiz;
    return thiz;
}
struct sply_unit *sply_append_branch(struct sply_unit **sply, int v, char *symbol)
{
    int malloc_size = sizeof(struct sply_unit) + strlen(symbol) + 1;
    struct sply_unit * thiz = (struct sply_unit*)malloc(malloc_size);
    memset(thiz, 0, malloc_size);
    thiz->value = v;
    thiz->symbol = (char*)(thiz) + sizeof(struct sply_unit);
    strcpy(thiz->symbol, symbol);
    while ( *sply ) {
        sply = &((*sply)->next_branch);
    }
    *sply = thiz;
    return thiz;
}
struct sply_unit *sply_append_leaf(struct sply_unit **sply, int v, char *symbol)
{
    int malloc_size = sizeof(struct sply_unit) + strlen(symbol) + 1;
    struct sply_unit * thiz = (struct sply_unit*)malloc(malloc_size);
    memset(thiz, 0, malloc_size);
    thiz->value = v;
    thiz->symbol = (char*)(thiz) + sizeof(struct sply_unit);
    strcpy(thiz->symbol, symbol);
    while ( *sply ) {
        sply = &((*sply)->next_leaf);
    }
    *sply = thiz;
    return thiz;
}

struct sply_unit *sply_search_tree(struct sply_unit *sply, const char *symbol)
{
    while ( sply ) {
        if ( 0 == strcmp(symbol, sply->symbol) ) return sply;
        sply = sply->next_tree;
    }

    return NULL;
}

struct sply_unit *sply_search_branch(struct sply_unit *sply, const char *symbol)
{
    while ( sply ) {
        if ( 0 == strcmp(symbol, sply->symbol) ) return sply;
        sply = sply->next_branch;
    }

    return NULL;
}

struct sply_unit *sply_search_leaf(struct sply_unit *sply, const char *symbol)
{
    while ( sply ) {
        if ( 0 == strcmp(symbol, sply->symbol) ) return sply;
        sply = sply->next_leaf;
    }

    return NULL;
}


void printf_tree(struct sply_unit *spl)
{
    struct sply_unit *branch = NULL;
    struct sply_unit *leaf = NULL;
    while ( spl ) {
        printf("+ %s:%d\n", spl->symbol, spl->branched_count);
        branch = spl->next_branch;
        while ( branch ) {
            printf("   |-%d: %s", branch->leaf_count, branch->symbol);
            leaf = branch->next_leaf;
            while ( leaf )
            {
                printf(" %s", leaf->symbol);
                leaf = leaf->next_leaf;
            }
            printf("\n");
            branch = branch->next_branch;
        }
        printf("\n");
        spl = spl->next_tree;
    }
}

int main(int argc, const char *argv[])
{
    char symbol[256];
    char *putin = symbol;
    struct sply_unit *tree = NULL;
    struct sply_unit *this_tree = NULL;
    struct sply_unit *branch = NULL;
    struct sply_unit *leaf = NULL;
    int start_branch = 0;

    FILE *fp = fopen(argv[1], "r+");
    if ( !fp ) return -1;
    memset(symbol, 0, sizeof(symbol));

    while ( ! feof(fp) ) {
        char ch;
        int nr = fread(&ch, 1, 1, fp);
        if ( nr <= 0 ) break;

        switch ( ch ) {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            {
                if ( strlen(symbol) ) {
                    struct sply_unit *me = sply_search_tree(tree, symbol);
                    int value;
                    if ( !me ) {
                        if ( symbol[0] == 'T' && symbol[1] == '_' ) {
                            me = sply_append_tree(&tree, ++gen_base, symbol);
                            value = me->value;
                        } else if (symbol[0] == 'T' && symbol[1] == 'O') {
                            struct symbol_item *item = search_symbol(symbol);
                            if ( !item ) {
                                printf("无效token: %s\n", symbol);
                            } else {
                                value = item->value;
                            }
                        } else if ( strlen(symbol) == 1 ) {
                            value = symbol[0];
                        } else {
                            printf("无效的字符!\n");
                        }
                    } else {
                        value = me->value;
                    }

                    if ( branch == NULL ) {
                        branch = sply_append_branch(&this_tree->next_branch, value, symbol);
                        this_tree->branched_count ++;
                        branch->leaf_count ++;
                    } else {
                        sply_append_leaf(&branch->next_leaf, value, symbol);
                        branch->leaf_count ++;
                    }
                }
            }
            memset(symbol, 0, sizeof(symbol));
            putin = symbol;
        break;
        case '#':
            branch = NULL;
            memset(symbol, 0, sizeof(symbol));
            putin = symbol;
        break;
        case ':':
            {
                struct sply_unit *me = sply_search_tree(tree, symbol);
                if ( me ) {
                    this_tree = me;
                } else {
                    this_tree = sply_append_tree(&tree, ++gen_base, symbol);
                }
                branch = NULL;
            }
            memset(symbol, 0, sizeof(symbol));
            putin = symbol;
        break;
        default:
            *putin ++ = ch;
        break;
        }
    }

    do {
        printf_tree(tree);
    } while (0);

    // gen code.
    do {
        struct sply_unit *branch = NULL;
        struct sply_unit *leaf = NULL;
        struct sply_unit *spl = tree;
        int branch_nr = 0;
        int tree_nr = 0;

        while ( spl ) {
            printf(" tree_0x%X:%0x\n", spl->value, spl->branched_count);
            branch = spl->next_branch;
            while ( branch ) {
                printf("int branch_%X[]={0x%x, ", branch_nr ++, branch->value);
                leaf = branch->next_leaf;
                while ( leaf )
                {
                    printf("0x%X, ", leaf->value);
                    leaf = leaf->next_leaf;
                }
                printf("0};\n");
                branch = branch->next_branch;
            }
            printf("\n");
            spl = spl->next_tree;
        }
    } while (0);

    return 0;
}

int stack_push(struct syntax_node **stack, int t)
{
}

struct syntax_node* stack_pop(struct syntax_node **stack)
{
}
