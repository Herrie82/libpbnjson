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

#ifndef JOBJECT_TYPES_H_
#define JOBJECT_TYPES_H_

#include <stddef.h>
#include <glib.h>
#include "japi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _JValueType {
	JV_NULL = 0,
	JV_BOOL,
	JV_NUM,
	JV_STR,
	JV_ARRAY,
	JV_OBJECT,
} JValueType;

typedef struct jvalue* jvalue_ref;

typedef struct jsaxparser *jsaxparser_ref;
typedef struct jdomparser *jdomparser_ref;

/**
  * @brief Iterator through JSON DOM object
 */
typedef struct {
	/// Internal structure iterator. Should not be used directly
	GHashTableIter m_iter;
} jobject_iter;

/**
 * @brief The options for the JSON array (currently unspecified). The structure is used for creation of JSON array.
 * Now the structure does not have any flags
 */
typedef struct {

} *jarray_opts;

typedef enum {
	SPLICE_NOCHANGE,    /// the splice doesn't have any affect on the ownership of the elements
	SPLICE_TRANSFER, /// the ownership of the spliced elements gets transferred to the new array (the old array loses ownership)
	SPLICE_COPY, /// the new array retains a copy of the individual elements from the first array
} JSpliceOwnership;

/**
 * @brief A structure to store a string and its length. This allows it to be friendly with Unicode encodings other
 * than UTF-8 but more importantly allows some no-copy optimizations.
 */
typedef struct {
	/// pointer to string
	const char *m_str;
	/// this should be set to the number of characters in m_str not including any terminating nulls.
	size_t m_len;
} raw_buffer;

/**
 * A structure representing a key/value pair in a JSON object.
 */
typedef struct {
	/// string containing name of the key
	jvalue_ref key;
	/// contains JSON value
	jvalue_ref value;
} jobject_key_value;

typedef void (*jdeallocator)(void *buffer);

#ifdef __cplusplus
}
#endif

#endif /* JOBJECT_TYPES_H_ */
// vim: set noet ts=4 sw=4:
