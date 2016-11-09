#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL */

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {/* parse space */
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int lept_parse_null(lept_context* c, lept_value* v) {
    EXPECT(c, 'n');/* if the first character is 'n' */
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;/* skip the next 3 chars */
    v->type = LEPT_NULL;
	/*
	The following 6 lines has been modified by James on 2016/11/09
	*/
	lept_parse_whitespace(&c);/* skip the spaces */
	if (c->json[0] == '\0'){
		return LEPT_PARSE_OK;
	}else{
		return LEPT_PARSE_ROOT_NOT_SINGULAR;
	}
    
}

/*
This function has been added by James on 2016/11/09
*/
static int lept_parse_true(lept_context* c, lept_value* v) {
    EXPECT(c, 't');/* if the first character is 'n' */
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;/* skip the next 3 chars */
    v->type = LEPT_TRUE;
	lept_parse_whitespace(&c);/* skip the spaces */
	if (c->json[0] == '\0'){
		return LEPT_PARSE_OK;
	}else{
		return LEPT_PARSE_ROOT_NOT_SINGULAR;
	}
    
}

/*
This function has been added by James on 2016/11/09
*/
static int lept_parse_false(lept_context* c, lept_value* v) {
    EXPECT(c, 'f');/* if the first character is 'n' */
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4;/* skip the next 3 chars */
    v->type = LEPT_FALSE;
	lept_parse_whitespace(&c);/* skip the spaces */
	if (c->json[0] == '\0'){
		return LEPT_PARSE_OK;
	}else{
		return LEPT_PARSE_ROOT_NOT_SINGULAR;
	}
    
}

static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 'n':  return lept_parse_null(c, v);	/* if starts with 'n' */
        case '\0': return LEPT_PARSE_EXPECT_VALUE;	/* if reach the end of string */
		/*
		The following 2 lines has been modified by James on 2016/11/09
		*/
		case 't' : return lept_parse_true(c,v);
		case 'f' : return lept_parse_false(c,v);
        default:   return LEPT_PARSE_INVALID_VALUE;
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    return lept_parse_value(&c, v);
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}
