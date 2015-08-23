#define _CRT_SECURE_NO_DEPRECATE
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "spl.h"

int is_inset(const char *set, const char ch)
{
    int n;
    for ( n = 0; set[n]; n ++ ) {
        if ( set[n] == ch ) return ch;
    }

    return 0;
}

struct keywords {
    const char *key_words;
    token_t token;
} key_words[] = {
    {"import", TOKEN_KEY_IMPORT}, 
    {"function", TOKEN_KEY_FUNCTION}, 
    {"return", TOKEN_KEY_RETURN}, 
    {"if", TOKEN_KEY_IF}, 
    {"elif", TOKEN_KEY_ELIF}, 
    {"else", TOKEN_KEY_ELSE}, 
    {"for", TOKEN_KEY_FOR}, 
    {"while", TOKEN_KEY_WHILE}, 
    {"continue", TOKEN_KEY_CONTINUE}, 
    {"break", TOKEN_KEY_BREAK}, 
    {"", TOKEN_INVALID}
};

int is_keywords(const struct keywords *set, const char *ch, token_t *t)
{
    int n;

    for ( n = 0; set[n].key_words; n ++ ) {
        if ( 0 == strcmp(set[n].key_words, ch) ) {
            if ( t ) *t = set[n].token;
            return 1;
        }
    }

    return 0;
}

token_t __spl_get_token(FILE *fp, char *text, size_t maxlen)
{
    char *terminal_chars = "[]{}(),./?;:\n\t";
    char *expre_begin = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM_";
    size_t ret, terminated = 0;
    char ch;
    int len = 0;
    int row = 0, line = 0;

    if ( !fp ) return TOKEN_INVALID;
    while ( ! terminated && !feof(fp) ) {
        ret = fread(&ch, 1, 1, fp);
        if ( ! ret ) break;

        // 字符串
        if ( ch == '\"' ) {
            int done = 0;
            while ( !done ) {
                ret = fread(&ch, 1, 1, fp);
                if ( ! ret ) break;

                // 字符串终结
                if ( ch == '\"' ) {
                    done = 1;
                    text[ len ] = '\0';
                    return TOKEN_STRING;
                }

                // 转义
                if ( ch == '\\' ) {
                    ret = fread(&ch, 1, 1, fp);
                    if ( ! ret ) break;
                    row ++;
                    switch ( ch ) {
                    case '\"':
                        text[ len ++ ] = '\"';
                        break;
                    case 'n':
                        text[ len ++ ] = '\n';
                        break;
                    case 'r':
                        text[ len ++ ] = '\r';
                        break;
                    case 't':
                        text[ len ++ ] = '\t';
                        break;
                    case '\\':
                        text[ len ++ ] = '\\';
                        break;
                    case '\r':
                        break;
                    case '\n':
                        row = 0; line ++;
                        break;
                    default:
                        row ++;
                        printf("WRN: invalid means convert with %c @ line %d, row %d\n", ch, line, row);
                        break;
                    }
                    continue;
                }

                // 非法终结
                if ( ch == '\r' || ch == '\n' ) {
                    return TOKEN_INVALID | TOKEN_STRING;
                }
                text[ len ++ ] = ch;
            }
        }
    
        // 操作符，比较，逻辑运算符
        switch (ch) {
        case '+':
            do {
                text[ len ++ ] = ch;
                ret = fread(&ch, 1, 1, fp);
                if ( ! ret ) break;
                if ( ch == '+' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    return TOKEN_OP_PLUSPLUS;
                } else if ( ch == '=' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    return TOKEN_OP_PLUSEQ;
                } else {
                    long offset = ftell(fp);
                    fseek(fp, offset -1 , SEEK_SET);
                    text[ len  ] = 0;
                    return '+';
                }
            } while (0);
        break;
        case '-':
           do {
                text[ len ++ ] = ch;
                ret = fread(&ch, 1, 1, fp);
                if ( ! ret ) break;
                if ( ch == '-' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    return TOKEN_OP_SUBSUB;
                } else if ( ch == '=' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    return TOKEN_OP_SUBEQ;
                } else {
                    long offset = ftell(fp);
                    fseek(fp, offset -1 , SEEK_SET);
                    text[ len  ] = 0;
                    return '-';
                }
            } while (0);
        break;
        case '*':
           do {
                text[ len ++ ] = ch;
                ret = fread(&ch, 1, 1, fp);
                if ( ! ret ) break;
                if ( ch == '=' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    return TOKEN_OP_MULEQ;
                } else {
                    long offset = ftell(fp);
                    fseek(fp, offset -1 , SEEK_SET);
                    text[ len  ] = 0;
                    return '*';
                }
            } while (0);
        break;
        case '/':
           do {
                text[ len ++ ] = ch;
                ret = fread(&ch, 1, 1, fp);
                if ( ! ret ) break;
                if ( ch == '=' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    return TOKEN_OP_DIVEQ;
                } else {
                    long offset = ftell(fp);
                    fseek(fp, offset -1 , SEEK_SET);
                    text[ len  ] = 0;
                    return '/';
                }
            } while (0);
        break;
        case '%':
           do {
                text[ len ++ ] = ch;
                ret = fread(&ch, 1, 1, fp);
                if ( ! ret ) break;
                if ( ch == '=' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    return TOKEN_OP_MODEQ;
                } else {
                    long offset = ftell(fp);
                    fseek(fp, offset -1 , SEEK_SET);
                    text[ len  ] = 0;
                    return '%';
                }
            } while (0);
        break;
        case '&':
           do {
                text[ len ++ ] = ch;
                ret = fread(&ch, 1, 1, fp);
                if ( ! ret ) break;
                if ( ch == '&' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    return TOKEN_LOGIC_AND;
                } else if ( ch == '=' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    return TOKEN_OP_ANDEQ;
                } else {
                    long offset = ftell(fp);
                    fseek(fp, offset -1 , SEEK_SET);
                    text[ len  ] = 0;
                    return '&';
                }
            } while (0);
        break;
        case '|':
           do {
                text[ len ++ ] = ch;
                ret = fread(&ch, 1, 1, fp);
                if ( ! ret ) break;
                if ( ch == '|' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    return TOKEN_LOGIC_OR;
                } else if ( ch == '=' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    return TOKEN_OP_OREQ;
                } else {
                    long offset = ftell(fp);
                    fseek(fp, offset -1 , SEEK_SET);
                    text[ len  ] = 0;
                    return '|';
                }
            } while (0);
        break;
        case '^':
           do {
                text[ len ++ ] = ch;
                ret = fread(&ch, 1, 1, fp);
                if ( ! ret ) break;
                if ( ch == '=' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    return TOKEN_OP_XOREQ;
                } else {
                    long offset = ftell(fp);
                    fseek(fp, offset -1 , SEEK_SET);
                    text[ len  ] = 0;
                    return '^';
                }
            } while (0);
        break;
        case '>':
           do {
                text[ len ++ ] = ch;
                ret = fread(&ch, 1, 1, fp);
                if ( ! ret ) break;
                if ( ch == '=' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    return TOKEN_CMP_BE;
                } if ( ch == '>' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    ret = fread(&ch, 1, 1, fp);
                    if ( ! ret ) break;
                    if ( ch == '=' ) {
                        text[ len ++ ] = ch;
                        text[ len  ] = 0;
                        return TOKEN_OP_RSHIFTEQ;
                    } else {
                        long offset = ftell(fp);
                        fseek(fp, offset -1 , SEEK_SET);
                        text[ len  ] = 0;
                        return TOKEN_OP_RSHIFT;
                    }
                } else {
                    long offset = ftell(fp);
                    fseek(fp, offset -1 , SEEK_SET);
                    text[ len  ] = 0;
                    return TOKEN_CMP_BG;
                }
            } while (0);
        break;
        case '<':
           do {
                text[ len ++ ] = ch;
                ret = fread(&ch, 1, 1, fp);
                if ( ! ret ) break;
                if ( ch == '=' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    return TOKEN_CMP_LE;
                } if ( ch == '<' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    ret = fread(&ch, 1, 1, fp);
                    if ( ! ret ) break;
                    if ( ch == '=' ) {
                        text[ len ++ ] = ch;
                        text[ len  ] = 0;
                        return TOKEN_OP_LSHIFTEQ;
                    } else {
                        long offset = ftell(fp);
                        fseek(fp, offset -1 , SEEK_SET);
                        text[ len  ] = 0;
                        return TOKEN_OP_LSHIFT;
                    }
                } else {
                    long offset = ftell(fp);
                    fseek(fp, offset -1 , SEEK_SET);
                    text[ len  ] = 0;
                    return TOKEN_CMP_LT;
                }
            } while (0);
        break;
        case '!':
           do {
                text[ len ++ ] = ch;
                ret = fread(&ch, 1, 1, fp);
                if ( ! ret ) break;
                if ( ch == '=' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    return TOKEN_CMP_NE;
                } else {
                    long offset = ftell(fp);
                    fseek(fp, offset -1 , SEEK_SET);
                    text[ len  ] = 0;
                    return TOKEN_LOGIC_NOT;
                }
            } while (0);
        break;
        case '=':
           do {
                text[ len ++ ] = ch;
                ret = fread(&ch, 1, 1, fp);
                if ( ! ret ) break;
                if ( ch == '=' ) {
                    text[ len ++ ] = ch;
                    text[ len  ] = 0;
                    return TOKEN_CMP_EQ;
                } else {
                    long offset = ftell(fp);
                    fseek(fp, offset -1 , SEEK_SET);
                    text[ len  ] = 0;
                    return TOKEN_OP_ASSUME;
                }
            } while (0);
        break;
        case '(':case ')':case '[':case ']':case ':':
        case '{':case '}':case ';':case ',':case '.':
            text[ len ++ ] = ch;
            text[ len ] = '\0';
            return (ch);
        break;
        }

        
        // 表达式
        if ( is_inset(expre_begin, ch) ) {
            char *expre_all = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM_1234567890";
            text[ len ++ ] = ch;
            text[ len ] = '\0';

            while (1) {
                token_t t;
                ret = fread(&ch, 1, 1, fp);
                if ( ! ret ) break;

                text[ len ++ ] = ch;
                text[ len ] = '\0';

                if ( ! is_inset(expre_all, ch ) ) {
                    long offset = ftell(fp);
                    text[ --len  ] = '\0';
                    fseek(fp, offset - 1, SEEK_SET);
                    return TOKEN_EXPR;
                } else {
                    if ( is_keywords(key_words, text, &t) ) {
                        return t;
                    }
                    continue;
                }
            }
        }

        
        // digit
        text[ len ++ ] = ch;
        text[ len ] = '\0';
        do {
            char *base16_set = "1234567890abcdefABCDEF"; // 0[xX][0-9a-fA-F]+
            char *base10_set = "0123456789";             // [1-9][0-9]+

            if ( ch == '0' ) {
                ret = fread(&ch, 1, 1, fp);
                if ( ! ret ) break;

                text[ len ++ ] = ch;
                text[ len ] = '\0';
                if ( is_inset("xX", ch) ) {
                    do {
                        ret = fread(&ch, 1, 1, fp);
                        if ( ! ret ) break;
                        text[ len ++ ] = ch;
                        text[ len ] = '\0';
                        if ( is_inset(base16_set, ch) ) {
                            continue;
                        } else {
                            if ( len > 2 ) {
                                long offset = ftell(fp);
                                text[ --len  ] = '\0';
                                fseek(fp, offset - 1, SEEK_SET);
                                return TOKEN_HEX;
                            } else break;
                        }
                    } while ( 1 );
                } else if ( ch == '.' ) {
                    do {
                        ret = fread(&ch, 1, 1, fp);
                        if ( ! ret ) break;
                        text[ len ++ ] = ch;
                        text[ len ] = '\0';
                        if ( is_inset(base10_set, ch) ) {
                            continue;
                        } else {
                            if ( len > 2 ) {
                                long offset = ftell(fp);
                                text[ --len  ] = '\0';
                                fseek(fp, offset - 1, SEEK_SET);
                                return TOKEN_DOUBLE;
                            } else break;
                        }
                    } while ( 1 );
                } else break; // not a hex, float
            } else if ( is_inset("123456789", ch) ) {
                do {
                        ret = fread(&ch, 1, 1, fp);
                        if ( ! ret ) break;
                        text[ len ++ ] = ch;
                        text[ len ] = '\0';
                        if ( is_inset(base10_set, ch) ) {
                            continue;
                        } else if ('.' == ch ) {
                            do {
                                ret = fread(&ch, 1, 1, fp);
                                if ( ! ret ) break;
                                text[ len ++ ] = ch;
                                text[ len ] = '\0';
                                if ( is_inset(base10_set, ch) ) {
                                    continue;
                                } else {
                                    if ( len > 2 ) {
                                        long offset = ftell(fp);
                                        text[ --len  ] = '\0';
                                        fseek(fp, offset - 1, SEEK_SET);
                                        return TOKEN_DOUBLE;
                                    } else break;
                                }
                            } while ( 1 );
                        } else {
                            long offset = ftell(fp);
                            text[ --len  ] = '\0';
                            fseek(fp, offset - 1, SEEK_SET);
                            return TOKEN_DEC;
                        }
                } while ( 1 );
            } else {
                // not a digit            
                break;
            }
        } while (0);

        switch ( ch ) {
        case ' ':case '\t':case '\r':case '\n':
            return TOKEN_BLANK | ch;
        break;
        }

        break;
    }
    return TOKEN_INVALID;
}

token_t sql_get_token(FILE *fp, char *text, size_t maxlen)
{
    token_t t = TOKEN_BLANK;

    while ( t == TOKEN_BLANK ) {
        t = __spl_get_token(fp, text, maxlen);
    }
    return t;
}
#if 0
int spl_do_parase(const char *file)
{
    char buff[1024];
    FILE *fp = fopen(file, "r");
    token_t t = 0;

    while ( t != TOKEN_INVALID ) {
        t = __spl_get_token(fp, buff, 1024);
        if ( t == TOKEN_INVALID ) break;
        printf("%08X  '%s'\n", t, buff);

        if ( t & TOKEN_KEY ) {
            switch (t) {
            case  TOKEN_KEY_IMPORT:
                do {
                    t = __spl_get_token(fp, buff, 1024);
                    if ( t == TOKEN_INVALID ) break;
                    if ( t & TOKEN_BLANK ) continue;
                    if ( t != TOKEN_EXPR ) {
                       printf("unexcepted packet <%s>\n", buff);
                       break;                   
                    }
                    printf("DO PACKET IMPORT <%s>\n", buff);
                    break;
                } while ( 1 );
            break;
            case  TOKEN_KEY_FUNCTION:
                do {
                    t = __spl_get_token(fp, buff, 1024);
                    if ( t == TOKEN_INVALID ) break;
                    if ( t & TOKEN_BLANK ) continue;
                    if ( t != TOKEN_EXPR ) {
                       printf("unexcepted packet <%s>\n", buff);
                       break;                   
                    }
                    printf("DEFINE FUNCTION <%s>\n", buff);
                    break;
                } while ( 1 );
            break;
            case  TOKEN_KEY_IF:
            break;
            case  TOKEN_KEY_ELIF:
            break;
            case  TOKEN_KEY_ELSE:
            break;
            case  TOKEN_KEY_FOR:
            break;
            case  TOKEN_KEY_WHILE:
            break;
           }
        } else if ( t == TOKEN_EXPR ) { // 调用/定义
        } else if ( t & TOKEN_BLANK) {
            continue;
        } else {
            //printf("unexcepted token <%s>\n", buff);
        }
    }

    fclose(fp);
    return 0;
}


int main()
{
    spl_do_parase("spl.txt");
    return 0;
}
#endif