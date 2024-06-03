#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod() */
#include <math.h>

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int lept_parse_true(lept_context* c, lept_value* v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}

static int lept_parse_false(lept_context* c, lept_value* v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}

static int lept_parse_null(lept_context* c, lept_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
}
static int  is_number(lept_context* c, int pointer){
    if(c->json[pointer] >= '0' && c->json[pointer] <= '9'){
        return 1;
    }
    return 0;

}
static int lept_parse_number(lept_context* c, lept_value* v) {
    char* end;
    int pointer=0;
    /* \TODO validate number */
    if (c->json[pointer] == '-'){
        pointer ++;
    }
    if (c->json[pointer] == '0'){

        pointer ++;

        if (is_number(c,pointer) == 1){
            return LEPT_PARSE_ROOT_NOT_SINGULAR;
            }
        
        if (c->json[pointer] == '.') pointer ++;
        
        while(is_number(c,pointer) == 1 ){

            pointer ++;

        }

    }
    else if( is_number(c,pointer) == 1 ){
        pointer ++;
        if (c->json[pointer] == '.' ){
            pointer++;
            if (is_number(c,pointer) != 1){
            return LEPT_PARSE_INVALID_VALUE;
            }
          
        }

        

        while(is_number(c,pointer) == 1 ){

            pointer ++;

        }



    }

    if (c->json[pointer]=='e' || c->json[pointer]=='E' )   pointer ++;
    

    if ((pointer !=0 && c->json[pointer]=='+' )|| c->json[pointer]=='-')    pointer ++;
    
    
    while(is_number(c,pointer) == 1 ) pointer ++;
    
  
    v->n = strtod(c->json, &end);
    if(v->n ==-INFINITY|| v->n == INFINITY) return LEPT_PARSE_NUMBER_TOO_BIG;
    if (c->json == end)
        return LEPT_PARSE_INVALID_VALUE;
    if(pointer==0)  return LEPT_PARSE_INVALID_VALUE;
    if(c->json+pointer!=end){
        return LEPT_PARSE_ROOT_NOT_SINGULAR;
    }
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 't':  return lept_parse_true(c, v);
        case 'f':  return lept_parse_false(c, v);
        case 'n':  return lept_parse_null(c, v);
        default:   return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}
