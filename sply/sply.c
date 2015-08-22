#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include "../spl/spl.h"
int gen_base = TOKEN_EXT + 1;
struct sply_unit {
    // 非终结符值
    int value;
    // 非终结符
    char *symbol;

    // 非终结符
    struct sply_unit *next_leaf;
    // 产生式
    struct sply_unit *next_branch;
    // 下一个非终结符
    struct sply_unit *next_tree;
};

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
        printf("+ %s\n", spl->symbol);
        branch = spl->next_branch;
        while ( branch ) {
            printf("   |-: %s", branch->symbol);
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
                            value = 255;
                        } else if ( strlen(symbol) == 1 ) {
                            value = symbol[0];
                        } else {
                            printf("无效的字符!\n");
                        }
                    }

                    if ( branch == NULL ) {
                        branch = sply_append_branch(&this_tree->next_branch, value, symbol);
                    } else {
                        sply_append_leaf(&branch->next_leaf, value, symbol);
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
                    printf("提前定义\n");
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

    return 0;
}