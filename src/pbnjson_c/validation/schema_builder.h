/****************************************************************
 * @@@LICENSE
 *
 * Copyright (c) 2014 LG Electronics, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * LICENSE@@@
 ****************************************************************/

/**
 *  @file schema_builder.h
 */

#ifndef __SCHEMA_BUILDER_H
#define __SCHEMA_BUILDER_H

#include <stdlib.h>
#ifndef NDEBUG
#include <stdio.h>
#endif

#include "parser_api.h"
#include "parser_context.h"
#include "validator.h"

/* Prototypes of the interface to the generated parser. */
void *JsonSchemaParserAlloc(void *(*mallocProc)(size_t));
void JsonSchemaParserFree(
	void *p,                    /* The parser to be deleted */
	void (*freeProc)(void*)     /* Function used to reclaim memory */
);
void JsonSchemaParser(
	void *yyp,                   /* The parser */
	int yymajor,                 /* The major token code number */
	TokenParam yyminor,          /* The value for the token */
	ParserContext *
);
#ifndef NDEBUG
void JsonSchemaParserTrace(FILE *f, char *p);
#endif // NDEBUG


/* context passed to all builder functions
 * TODO: remove YajlContext name
 */
typedef struct
{
	void *parser;
	ParserContext parser_ctxt;
} YajlContext, jschema_builder;

/* builder functions interface */
static void jschema_builder_init(jschema_builder *builder)
{
	*builder = (jschema_builder)
	{
		.parser = JsonSchemaParserAlloc(malloc),
		.parser_ctxt = {
			.validator = NULL,
			.error = SEC_OK,
		},
	};
}

static void jschema_builder_destroy(jschema_builder *builder)
{
	// Forget about SchemaParsing then.
	validator_unref(builder->parser_ctxt.validator);

	JsonSchemaParserFree(builder->parser, free);
}

static bool jschema_builder_is_ok(jschema_builder *builder)
{ return (builder->parser_ctxt.error == SEC_OK); }

static int jschema_builder_error_code(jschema_builder *builder)
{ return builder->parser_ctxt.error; }

static const char *jschema_builder_error_str(jschema_builder *builder)
{ return SchemaGetErrorMessage(builder->parser_ctxt.error); }

jschema_builder *jschema_builder_create();
void jschema_builder_free(jschema_builder *builder);

Validator* jschema_builder_finish(jschema_builder *builder, UriResolver *uri_resolveri, char const *root_scope);

#endif
