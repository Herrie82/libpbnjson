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

#include "generic_validator.h"
#include "validation_state.h"
#include "validation_event.h"
#include <jobject.h>
#include <glib.h>

static Validator* ref(Validator *validator)
{
	GenericValidator *v = (GenericValidator *) validator;
	++v->ref_count;
	return validator;
}

static void unref(Validator *validator)
{
	GenericValidator *v = (GenericValidator *) validator;
	if (--v->ref_count)
		return;
	j_release(&v->def_value);
	g_free(v);
}

static Validator* set_default(Validator *validator, jvalue_ref def_value)
{
	GenericValidator *v = (GenericValidator *) validator;
	j_release(&v->def_value);
	v->def_value = jvalue_copy(def_value);
	return validator;
}

static jvalue_ref get_default(Validator *validator, ValidationState *s)
{
	GenericValidator *v = (GenericValidator *) validator;
	return v->def_value;
}

static Validator* set_default_generic(Validator *v, jvalue_ref def_value)
{
	return set_default(&generic_validator_new()->base, def_value);
}

static bool check(Validator *v, ValidationEvent const *e, ValidationState *s, void *c)
{
	// The generic validator has tricky logic. It should validate a single JSON value.
	// If the value is of simple type, implementaion is trivial. But array and
	// object must be tracked carefully, because they can be nested.
	// We rely on the parser to check the order of nested objects and arrays
	// boundaries.

	int depth = GPOINTER_TO_INT(validation_state_get_context(s));
	if (!depth)
	{
		// If the event isn't a beginning of object or array, consume it,
		// and we're done.
		if (e->type != EV_OBJ_START && e->type != EV_ARR_START)
		{
			validation_state_pop_validator(s);
			return true;
		}
	}

	switch (e->type)
	{
	case EV_OBJ_START:
	case EV_ARR_START:
		validation_state_set_context(s, GINT_TO_POINTER(++depth));
		return true;
	case EV_OBJ_END:
	case EV_ARR_END:
		validation_state_set_context(s, GINT_TO_POINTER(--depth));
		break;
	default:
		return true;
	}

	// If the last array or object has been closed, we're done.
	if (!depth)
	{
		validation_state_pop_validator(s);
		return true;
	}
	// Ignore happily anything else.
	return true;
}

static bool check_inverse(Validator *v, ValidationEvent const *e, ValidationState *s, void *c)
{
	return !check(v, e, s, c);
}

static bool init_state(Validator *v, ValidationState *s)
{
	// We'll store sum of object and array depths of incomming events.
	// We assume, YAJL does basic check of event order, and when
	// this sum drops to 0, all the opened objects and arrays have been
	// properly closed.
	validation_state_push_context(s, GINT_TO_POINTER(0));
	return true;
}

static void cleanup_state(Validator *v, ValidationState *s)
{
	validation_state_pop_context(s);
}

static void dump_enter(char const *key, Validator *v, void *ctxt)
{
	if (key)
		fprintf((FILE *) ctxt, "%s:", key);
	fprintf((FILE *) ctxt, "(*)");
}

static ValidatorVtable generic_vtable =
{
	.ref = ref,
	.unref = unref,
	.check = check,
	.init_state = init_state,
	.cleanup_state = cleanup_state,
	.set_default = set_default,
	.get_default = get_default,
	.dump_enter = dump_enter,
};

static ValidatorVtable generic_static_vtable =
{
	.check = check,
	.init_state = init_state,
	.cleanup_state = cleanup_state,
	.set_default = set_default_generic,
	.dump_enter = dump_enter,
};

static ValidatorVtable inverse_generic_static_vtable =
{
	.check = check_inverse,
	.init_state = init_state,
	.cleanup_state = cleanup_state,
	.dump_enter = dump_enter,
};

GenericValidator *generic_validator_new(void)
{
	GenericValidator *v = g_new0(GenericValidator, 1);
	v->ref_count = 1;
	validator_init(&v->base, &generic_vtable);
	return v;
}

// Static instance may be suitable to cover "additionalProperties" and
// other service cases when default value isn't attached to the validator.
static Validator GENERIC_VALIDATOR_IMPL =
{
	.vtable = &generic_static_vtable,
};

static Validator INVERSE_GENERIC_VALIDATOR_IMPL =
{
	.vtable = &inverse_generic_static_vtable,
};

Validator *GENERIC_VALIDATOR = &GENERIC_VALIDATOR_IMPL;

Validator *generic_validator_instance(void)
{
	return GENERIC_VALIDATOR;
}

Validator *inverse_generic_validator_instance(void)
{
	return &INVERSE_GENERIC_VALIDATOR_IMPL;
}
