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

#pragma once

#include "validator.h"
#include <stdbool.h>
#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _StringSpan StringSpan;
typedef struct _Validator Validator;
typedef struct _UriResolver UriResolver;
typedef struct _UriScope UriScope;

/** @brief Definitions is standard place for subschemas */
typedef struct _Definitions
{
	Validator base;      /**< @brief Base class. We want definitions to be visitable. */

	unsigned ref_count;  /**< @brief Reference count */
	char *name;          /**< @brief Name of the definition, key in the parent schema. */
	GSList *validators;  /**< @brief List of subschemas with their names. */
} Definitions;

/** @brief Constructor */
Definitions* definitions_new(void);

/** @brief Decrement reference counter. Once it drops to zero, destruct the object. */
void definitions_unref(Definitions *d);

/** @brief Set name for this definitions */
void definitions_set_name(Definitions *d, StringSpan *name);

/** @brief Add a definition
 *
 * @param[in] d This object
 * @param[in] name Name of the subschema in {"definitions": {"name": {...}}}
 * @param[in] v Validator for the subschema in {"definitions": {"name": {...}}}
 */
void definitions_add(Definitions *d, StringSpan *name, Validator *v);

/** @brief collect schemas for all definitions
 *
 * @param[in] d This object
 * @param[in,out] uri_scope Information about current scope and associated resolver
 */
void definitions_collect_schemas(Definitions *d, UriScope *uri_scope);

#ifdef __cplusplus
}
#endif
