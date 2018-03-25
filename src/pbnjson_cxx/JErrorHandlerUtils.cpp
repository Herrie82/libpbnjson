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

#define PBNJSON_USE_DEPRECATED_API
#include "JErrorHandlerUtils.h"
#include "../pbnjson_c/validation/error_code.h"

namespace pbnjson {

JErrorHandler::SchemaError ErrorToSchemaError(int type)
{
	switch(type)
	{
	case VEC_MISSING_REQUIRED_KEY:
		return JErrorHandler::ERR_SCHEMA_MISSING_REQUIRED_KEY;

	case VEC_NOT_NULL:
	case VEC_NOT_BOOLEAN:
	case VEC_NOT_STRING:
	case VEC_NOT_NUMBER:
	case VEC_NOT_INTEGER_NUMBER:
	case VEC_NOT_ARRAY:
	case VEC_NOT_OBJECT:
	case VEC_TYPE_NOT_ALLOWED:
		return JErrorHandler::ERR_SCHEMA_UNEXPECTED_TYPE;

	default:
		break;
	}

	return JErrorHandler::ERR_SCHEMA_GENERIC;
}

}
