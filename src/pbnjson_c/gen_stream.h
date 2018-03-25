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

#ifndef GEN_STREAM_H_
#define GEN_STREAM_H_

#include <stdint.h>
#include "jschema_types.h"
#include "japi.h"
#include "jgen_types.h"

enum TopLevelType {
	TOP_None,
	TOP_Object,
	TOP_Array,
};

typedef enum TopLevelType TopLevelType;

PJSON_LOCAL JStreamRef jstreamInternal(TopLevelType type, const char *indent);

#endif /* GEN_STREAM_H_ */
