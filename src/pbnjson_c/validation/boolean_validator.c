// @@@LICENSE
//
//      Copyright (c) 2009-2013 LG Electronics, Inc.
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
// LICENSE@@@

#include "boolean_validator.h"
#include "validation_event.h"
#include "validation_state.h"

static void _release(Validator *v)
{
	BooleanValidator *b = (BooleanValidator *) v;
	g_free(b);
}

static bool _check(Validator *v, ValidationEvent const *e, ValidationState *s, void *c)
{
	BooleanValidator *b = (BooleanValidator *) v;

	if (e->type != EV_BOOL)
	{
		validation_state_notify_error(s, VEC_NOT_BOOLEAN, c);
		validation_state_pop_validator(s);
		return false;
	}

	if (b->value_expected && b->value != e->value.boolean)
	{
		validation_state_notify_error(s, VEC_UNEXPECTED_VALUE, c);
		validation_state_pop_validator(s);
		return false;
	}

	validation_state_pop_validator(s);
	return true;
}

static ValidatorVtable boolean_vtable =
{
	.release = _release,
	.check = _check,
};

static BooleanValidator BOOLEAN_VALIDATOR_IMPL =
{
	.base = {
		.ref_count = 42,   // Let it live forever
		.vtable = &boolean_vtable
	},
	.value_expected = false,
	.value = false,
};

Validator *BOOLEAN_VALIDATOR = &BOOLEAN_VALIDATOR_IMPL.base;

BooleanValidator *boolean_validator_new(void)
{
	BooleanValidator *b = g_new0(BooleanValidator, 1);
	if (!b)
		return NULL;
	validator_init(&b->base, &boolean_vtable);
	return b;
}

BooleanValidator *boolean_validator_new_with_value(bool value)
{
	BooleanValidator *b = boolean_validator_new();
	if (!b)
		return NULL;
	b->value_expected = true;
	b->value = value;
	return b;
}

BooleanValidator *boolean_validator_ref(BooleanValidator *b)
{
	if (b)
		validator_ref(&b->base);
	return b;
}

void boolean_validator_unref(BooleanValidator *b)
{
	if (b)
		validator_unref(&b->base);
}