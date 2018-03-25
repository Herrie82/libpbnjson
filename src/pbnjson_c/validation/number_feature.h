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

#include "feature.h"
#include "number.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Validator Validator;

/** @brief Number feature application function.
 *
 * @param[in] v Validator to apply the feature to.
 * @param[in] n Number to apply
 * @return Validator with feature applied
 */
typedef Validator* (*NumberFeatureFunc)(Validator *v, Number *n);


/** @brief Number feature class */
typedef struct _NumberFeature
{
	Feature base;                   /**< Base class */
	Number value;                   /**< Number value */
	NumberFeatureFunc apply_func;   /**< Application function */
} NumberFeature;


/** @brief Constructor.
 *
 * @param[in] str Pointer to the text to get number from
 * @param[in] len Count of bytes of the number text
 * @param[in] apply_func Application function to use
 * @return An allocated and initialized NumberFeature
 */
NumberFeature* number_feature_new(char const *str, size_t len,
                                  NumberFeatureFunc apply_func);

/** @brief Increase reference counter. */
NumberFeature* number_feature_ref(NumberFeature *n);

/** @brief Decrease reference counter. Once it drops to zero, the object is destructed. */
void number_feature_unref(NumberFeature *n);

#ifdef __cplusplus
}
#endif
