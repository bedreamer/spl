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

// 产生树0 T_BODY
int br_0[]={0x8000003, 0x8000002, 0};
int br_1[]={0x8000004, 0x8000002, 0};

// 产生树1 T_IMPORT_LIST
int br_2[]={0x8101, 0x1008, 0x8000003, 0};
int br_3[]={0xfffffffd, 0};

// 产生树2 T_CONTEXT_LIST
int br_4[]={0x8000005, 0x8000004, 0};
int br_5[]={0x8000006, 0x3D, 0x8000006, 0x8000004, 0};
int br_6[]={0xfffffffd, 0};

// 产生树3 T_FUNC_DEF_EXPR
int br_7[]={0x8102, 0x1008, 0x28, 0x8000006, 0x29, 0x800000D, 0};

// 产生树4 T_VALUE_EXPR
int br_8[]={0x8000007, 0};
int br_9[]={0x8000008, 0};
int br_A[]={0x8000009, 0};
int br_B[]={0x800000a, 0};
int br_C[]={0x800000b, 0};
int br_D[]={0x800000c, 0};
int br_E[]={0x8000006, 0};

// 产生树5 T_CONST_EXPR
int br_F[]={0x1003, 0};
int br_10[]={0x1004, 0};
int br_11[]={0x1005, 0};
int br_12[]={0x1006, 0};

// 产生树6 T_VAR_EXPR
int br_13[]={0x1008, 0};

// 产生树7 T_CACULATE_EXPR
int br_14[]={0x8000006, 0x2B, 0x8000006, 0};
int br_15[]={0x8000006, 0x2D, 0x8000006, 0};
int br_16[]={0x8000006, 0x2A, 0x8000006, 0};
int br_17[]={0x8000006, 0x2F, 0x8000006, 0};
int br_18[]={0x8000006, 0x26, 0x8000006, 0};
int br_19[]={0x8000006, 0x5E, 0x8000006, 0};
int br_1A[]={0x8000006, 0x7C, 0x8000006, 0};
int br_1B[]={0x8000006, 0x2C, 0x8000006, 0};
int br_1C[]={0x8000006, 0x2113, 0x8000006, 0};
int br_1D[]={0x8000006, 0x2115, 0x8000006, 0};
int br_1E[]={0x8000006, 0x3D, 0x8000006, 0};
int br_1F[]={0x28, 0x8000006, 0x29, 0};
int br_20[]={0x7e, 0x8000006, 0};
int br_21[]={0x21, 0x8000006, 0};
int br_22[]={0x8000006, 0x2101, 0};
int br_23[]={0x8000006, 0x2103, 0};

// 产生树8 T_LOGIC_EXPR
int br_24[]={0x8000006, 0x100B, 0x8000006, 0};
int br_25[]={0x8000006, 0x100C, 0x8000006, 0};

// 产生树9 T_CMP_EXPR
int br_26[]={0x8000006, 0x403E, 0x8000006, 0};
int br_27[]={0x8000006, 0x403E, 0x8000006, 0};
int br_28[]={0x8000006, 0x403C, 0x8000006, 0};
int br_29[]={0x8000006, 0x4101, 0x8000006, 0};
int br_2A[]={0x8000006, 0x4102, 0x8000006, 0};
int br_2B[]={0x8000006, 0x4103, 0x8000006, 0};
int br_2C[]={0x8000006, 0x4104, 0x8000006, 0};

// 产生树10 T_CALL_EXPR
int br_2D[]={0x8000008, 0x28, 0x8000006, 0x29, 0};

// 产生树11 T_CODE_BLOCK_EXPR
int br_2E[]={0x7b, 0x800000D, 0x7D, 0};
int br_2F[]={0x8104, 0x28, 0x8000006, 0x29, 0x800000E, 0x800000F, 0x800000D, 0};
int br_30[]={0x800000c, 0x800000D, 0};
int br_31[]={0x8000006, 0x3D, 0x8000006, 0x800000D, 0};
int br_32[]={0x8000010, 0x800000D, 0};
int br_33[]={0xfffffffd, 0};

// 产生树12 T_ELIF_BRANCHES_EXPR
int br_34[]={0x8105, 0x28, 0x8000006, 0x29, 0x800000D, 0x800000E, 0};
int br_35[]={0xfffffffd, 0};

// 产生树13 T_ELSE_BRANCHES_EXPR
int br_36[]={0x8106, 0x800000D, 0};
int br_37[]={0xfffffffd, 0};

// 产生树14 T_LOOP_EXPR
int br_38[]={0x8108, 0x28, 0x8000006, 0x29, 0x800000D, 0};

// 树0:T_BODY 的分支
int *tree0_br[]={br_0, br_1, NULL};
// 树1:T_IMPORT_LIST 的分支
int *tree1_br[]={br_2, br_3, NULL};
// 树2:T_CONTEXT_LIST 的分支
int *tree2_br[]={br_4, br_5, br_6, NULL};
// 树3:T_FUNC_DEF_EXPR 的分支
int *tree3_br[]={br_7, NULL};
// 树4:T_VALUE_EXPR 的分支
int *tree4_br[]={br_8, br_9, br_A, br_B, br_C, br_D, br_E, NULL};
// 树5:T_CONST_EXPR 的分支
int *tree5_br[]={br_F, br_10, br_11, br_12, NULL};
// 树6:T_VAR_EXPR 的分支
int *tree6_br[]={br_13, NULL};
// 树7:T_CACULATE_EXPR 的分支
int *tree7_br[]={br_14, br_15, br_16, br_17, br_18, br_19, br_1A, br_1B, br_1C, br_1D, br_1E, br_1F, br_20, br_21, br_22
, br_23, NULL};
// 树8:T_LOGIC_EXPR 的分支
int *tree8_br[]={br_24, br_25, NULL};
// 树9:T_CMP_EXPR 的分支
int *tree9_br[]={br_26, br_27, br_28, br_29, br_2A, br_2B, br_2C, NULL};
// 树10:T_CALL_EXPR 的分支
int *tree10_br[]={br_2D, NULL};
// 树11:T_CODE_BLOCK_EXPR 的分支
int *tree11_br[]={br_2E, br_2F, br_30, br_31, br_32, br_33, NULL};
// 树12:T_ELIF_BRANCHES_EXPR 的分支
int *tree12_br[]={br_34, br_35, NULL};
// 树13:T_ELSE_BRANCHES_EXPR 的分支
int *tree13_br[]={br_36, br_37, NULL};
// 树14:T_LOOP_EXPR 的分支
int *tree14_br[]={br_38, NULL};
//产生式
struct tree_struct {
    int value;
    int **branches;
}trees[]={
    {0x8000002, tree0_br},
    {0x8000003, tree1_br},
    {0x8000004, tree2_br},
    {0x8000005, tree3_br},
    {0x8000006, tree4_br},
    {0x8000007, tree5_br},
    {0x8000008, tree6_br},
    {0x8000009, tree7_br},
    {0x800000a, tree8_br},
    {0x800000b, tree9_br},
    {0x800000c, tree10_br},
    {0x800000d, tree11_br},
    {0x800000e, tree12_br},
    {0x800000f, tree13_br},
    {0x8000010, tree14_br},
    {0, NULL}
};

int match(token_t *p, int nr)
{
    return PUSH_ACCES;
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
            printf("// 产生树%d %s\n", tree_nr++, spl->symbol);
            branch = spl->next_branch;
            while ( branch ) {
                printf("int br_%X[]={0x%x, ", branch_nr ++, branch->value);
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

        // 分支列表
        spl = tree;
        tree_nr = 0;
        branch_nr = 0;
        while ( spl ) {
            branch = spl->next_branch;
            printf("// 树%d:%s 的分支\n" ,tree_nr, spl->symbol);
            printf("int *tree%d_br[]={" ,tree_nr ++);
            while ( branch ) {
                printf("br_%X, ", branch_nr ++);
                branch = branch->next_branch;
            }
            printf("NULL};\n");
            spl = spl->next_tree;
        }

        // define tree
        spl = tree;
        branch_nr = 0;
        tree_nr = 0;
        printf("//产生式\nstruct tree_struct {\n");
        printf("\tint value;\n");
        printf("\tint **branches;\n}trees[]={\n");
        while ( spl ) {
            printf("\t{0x%x, tree%d_br},\n", spl->value, tree_nr ++);
            spl = spl->next_tree;
        }
        printf("\t{0, NULL}\n};\n");
    } while (0);

    do {
        FILE *fp = fopen("spl.txt", "r+");
        char buff[1024] = {0};
        token_t t;
        token_t stack[1024] = {0};
        int top = 0;
        int i,j;
        struct tree_struct *tree;

        while ( (t = sql_get_token(fp, buff, 1024)) != TOKEN_INVALID ) {
            if ( t & TOKEN_BLANK ) continue;
            printf("push 0x%x\n", t);
            stack[ top ++ ] = t;
            
            for ( i = 0; i < top; i ++ ) {

            }
            top ++;
        }
    } while(0);

    return 0;
}
