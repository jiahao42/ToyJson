#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod() */
#include <math.h>
#include <errno.h>

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
	double n;
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int lept_parse_literal(lept_context* c, lept_value* v, const char* literal, lept_type type){
	size_t i;
	EXPECT(c,literal[0]);
	for (i = 0; literal[i+1]; i++){
		if(c->json[i] != literal[i+1]){
			return LEPT_PARSE_INVALID_VALUE;
		}
	}
	c->json += i;
	v->type = type;
	return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context* c, lept_value* v) {
    char* end;
	/* in case the number is 'nan' 'NAN' 'inf' 'INF' */
	if(*c->json == 'i' || *c->json == 'I' || *c->json == 'n' || *c->json == 'N'){
		return LEPT_PARSE_INVALID_VALUE;
	}
	/* only '.' can be after the '0' */
	if (*c->json == '0'){
		if (c->json[1] != '\0'){
			if (c->json[1] != '.'){
				return LEPT_PARSE_ROOT_NOT_SINGULAR;
			}
		}
	}
	/* number never start with '.' or '+' */
    if (*c->json == '.' || *c->json == '+'){
		return LEPT_PARSE_INVALID_VALUE;
	}
	/* only digit can be after the '.' */
	size_t i = 0;
	while(c->json[i] != '\0'){
		if(c->json[i] == '.'){
			i++;
			if((c->json[i] - '0') > 9 || (c->json[i] - '0') < 0){
				return LEPT_PARSE_INVALID_VALUE;
			}
			break;
		}
		i++;
	}
	/* in case the number is too big */
	errno = 0;
    v->n = strtod(c->json, &end);
	if (v->n == HUGE_VAL || errno == ERANGE){
		return LEPT_PARSE_NUMBER_TOO_BIG;
	}
    if (c->json == end)/* if the str not change at all, it means there is no number */
        return LEPT_PARSE_INVALID_VALUE;
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 't':  return lept_parse_literal(c,v,"true",LEPT_TRUE);
		case 'f':  return lept_parse_literal(c,v,"false",LEPT_FALSE);
		case 'n':  return lept_parse_literal(c,v,"null",LEPT_NULL);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
		default:   return lept_parse_number(c, v);
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
