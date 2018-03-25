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
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/** @brief JSON boolean validator */
typedef struct _BooleanValidator
{
	Validator base;        /**< @brief Base class */
	unsigned ref_count;    /**< @brief Reference count */
	jvalue_ref def_value;  /**< @brief Default value attached to this validator */
} BooleanValidator;

/** @brief Generic boolean validator. Checks only value type. */
Validator *boolean_validator_instance(void);

/** @brief Constructor */
BooleanValidator *boolean_validator_new(void);

/** @brief Constructor with specific expected value (for enum). */
Validator *boolean_validator_new_with_value(bool value);

#ifdef __cplusplus
}
#endif
