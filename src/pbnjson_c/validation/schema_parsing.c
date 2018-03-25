// Copyright (c) 2009-2018 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#include "schema_parsing.h"
#include "validator.h"
#include "parser_context.h"
#include "combined_types_validator.h"
#include "combined_validator.h"
#include "generic_validator.h"
#include "feature.h"
#include "uri_resolver.h"
#include "definitions.h"
#include "uri_scope.h"
#include <stdio.h>
#include <assert.h>


static void _release_feature(gpointer data)
{
	feature_unref((Feature *) data);
}

static void _release_validator(gpointer data)
{
	validator_unref((Validator *) data);
}

static Validator* ref(Validator *v)
{
	SchemaParsing *s = (SchemaParsing *) v;
	++s->ref_count;
	return v;
}

static void unref(Validator *v)
{
	SchemaParsing *s = (SchemaParsing *) v;
	if (--s->ref_count)
		return;

	g_slist_free_full(s->features, _release_feature);
	validator_unref(s->type_validator);
	definitions_unref(s->definitions);
	g_slist_free_full(s->validator_combinators, _release_validator);
	validator_unref(s->extends);
	g_free(s->id);
	g_free(s);
}

static void _visit(Validator *v,
                   VisitorEnterFunc enter_func, VisitorExitFunc exit_func,
                   void *ctxt)
{
	SchemaParsing *s = (SchemaParsing *) v;

	validator_visit((Validator *)s->definitions, enter_func, exit_func, ctxt);

	if (s->type_validator)
	{
		enter_func(NULL, s->type_validator, ctxt);
		validator_visit(s->type_validator, enter_func, exit_func, ctxt);
		Validator *new_v = NULL;
		exit_func(NULL, s->type_validator, ctxt, &new_v);
		if (new_v)
		{
			validator_unref(s->type_validator);
			s->type_validator = new_v;
		}
	}

	if (s->extends)
	{
		enter_func(NULL, s->extends, ctxt);
		validator_visit(s->extends, enter_func, exit_func, ctxt);
		Validator *new_v = NULL;
		exit_func(NULL, s->extends, ctxt, &new_v);
		if (new_v)
		{
			validator_unref(s->extends);
			s->extends = new_v;
		}
	}

	GSList *it = s->validator_combinators;
	while(it)
	{
		Validator *item = it->data;
		if (!item)
			continue;
		enter_func(NULL, item, ctxt);
		validator_visit(item, enter_func, exit_func, ctxt);
		Validator *new_v = NULL;
		exit_func(NULL, item, ctxt, &new_v);
		if (new_v)
		{
			validator_unref(item);
			it->data = new_v;
		}
		it = g_slist_next(it);
	}
}

static void _combine(char const *key, Validator *v, void *ctxt, Validator **new_v)
{
	SchemaParsing *s = (SchemaParsing *) v;
	CombinedValidator *vcomb = NULL;

	if (!s->type_validator &&
	    !s->validator_combinators &&
	    !s->extends)
	{
		s->type_validator = GENERIC_VALIDATOR;
		return;
	}

	GSList *it = s->validator_combinators;
	while (it)
	{
		// If there was no type validator, consider the first element as one.
		if (!s->type_validator)
		{
			s->type_validator = it->data;
		}
		else if (!vcomb)
		{
			// This is the second validator (counting from type validator),
			// all they should be combined.
			vcomb = all_of_validator_new();
			// Do not need to suppress errors, because it is not explicitly created allOf validator
			combined_validator_collect_errors(vcomb);
			combined_validator_add_value(vcomb, s->type_validator);
			s->type_validator = &vcomb->base;
			combined_validator_add_value(vcomb, it->data);
		}
		else
			combined_validator_add_value(vcomb, it->data);

		GSList *old = it;
		it = g_slist_next(it);
		g_slist_free_1(old);
	}
	s->validator_combinators = NULL;

	if (s->extends)
	{
		vcomb = all_of_validator_new();
		combined_validator_add_value(vcomb, s->extends);
		s->extends = NULL;
		if (s->type_validator)
			combined_validator_add_value(vcomb, s->type_validator);
		s->type_validator = &vcomb->base;
	}
}

static void _finalize_parse(char const *key, Validator *v, void *ctxt, Validator **new_v)
{
	SchemaParsing *s = (SchemaParsing *) v;
	*new_v = validator_ref(s->type_validator);
}

static void _apply_features(char const *key, Validator *v, void *ctxt)
{
	SchemaParsing *s = (SchemaParsing *) v;

	GSList *head = s->features;
	if (!head)
		return;

	// if there is no validator for features to apply (aka no types was defined in schema)
	// create combined types validator that allow all types
	if (!s->type_validator)
	{
		s->type_validator = (Validator *) combined_types_validator_new();
		combined_types_validator_fill_all_types((CombinedTypesValidator *) s->type_validator);
	}

	while (head)
	{
		s->type_validator = feature_apply((Feature *) head->data, s->type_validator);
		head = g_slist_next(head);
	}
}

static void _collect_uri_enter(char const *key, Validator *v, void *ctxt)
{
	SchemaParsing *s = (SchemaParsing *) v;
	UriScope *uri_scope = (UriScope *) ctxt;

	if (s->id)
		uri_scope_push_uri(uri_scope, s->id);
}

static void _collect_schemas(Validator *v, void *ctxt)
{
	SchemaParsing *s = (SchemaParsing *) v;
	UriScope *uri_scope = (UriScope *) ctxt;

	// collect root schema
	int chars_required = uri_scope_get_document_length(uri_scope);
	char buffer[chars_required];
	char const *document = uri_scope_get_document(uri_scope, buffer, chars_required);
	char const *fragment = uri_scope_get_fragment(uri_scope);

	if (!uri_resolver_add_validator(uri_scope->uri_resolver, document, fragment, s->type_validator))
		fprintf(stderr, "The same fragment %s is added second time!\n", fragment);

	// collect #/definitions/* if present
	Definitions *defs = s->definitions;
	if (defs) definitions_collect_schemas(defs, uri_scope);
}

static void _collect_uri_exit(char const *key, Validator *v, void *ctxt, Validator **v_new)
{
	SchemaParsing *s = (SchemaParsing *) v;
	UriScope *uri_scope = (UriScope *) ctxt;

	if (s->id)
		uri_scope_pop_uri(uri_scope);
}

static void _dump_enter(char const *key, Validator *v, void *ctxt)
{
	if (key)
		fprintf((FILE *) ctxt, "%s:", key);
	fprintf((FILE *) ctxt, "(?");
}

static void _dump_exit(char const *key, Validator *v, void *ctxt, Validator **new_v)
{
	fprintf((FILE *) ctxt, "?)");
}

static ValidatorVtable schema_parsing_vtable =
{
	.ref = ref,
	.unref = unref,
	.visit = _visit,
	.apply_features = _apply_features,
	.combine_validators = _combine,
	.finalize_parse = _finalize_parse,
	.collect_uri_enter = _collect_uri_enter,
	.collect_schemas = _collect_schemas,
	.collect_uri_exit = _collect_uri_exit,
	.dump_enter = _dump_enter,
	.dump_exit = _dump_exit,
};

SchemaParsing* schema_parsing_new(void)
{
	SchemaParsing *s = g_new0(SchemaParsing, 1);
	s->ref_count = 1;
	validator_init(&s->base, &schema_parsing_vtable);
	return s;
}

SchemaParsing* schema_parsing_ref(SchemaParsing *s)
{
	validator_ref(&s->base);
	return s;
}

void schema_parsing_unref(SchemaParsing *s)
{
	validator_unref(&s->base);
}

void schema_parsing_add_feature(SchemaParsing *s, Feature *f)
{
	assert(s);
	// Skip empty features.
	if (!f)
		return;
	s->features = g_slist_prepend(s->features, f);
}

void schema_parsing_set_validator(SchemaParsing *s, Validator *v)
{
	validator_unref(s->type_validator);
	s->type_validator = v;
}

void schema_parsing_set_id(SchemaParsing *s, StringSpan id)
{
	g_free(s->id);
	s->id = g_strndup(id.str, id.str_len);
}

void schema_parsing_set_definitions(SchemaParsing *s, Definitions *d)
{
	definitions_unref(s->definitions);
	s->definitions = d;
}

void schema_parsing_add_combinator(SchemaParsing *s, Validator *v)
{
	assert(s);
	if (v)
		s->validator_combinators = g_slist_prepend(s->validator_combinators, v);
}

void schema_parsing_set_extends(SchemaParsing *s, Validator *extends)
{
	assert(s);
	validator_unref(s->extends);
	s->extends = extends;
}
