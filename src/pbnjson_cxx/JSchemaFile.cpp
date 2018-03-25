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
#include <JSchemaFile.h>
#include <pbnjson.h>

#include <cstdio>
#include <cassert>

#include "../pbnjson_c/liblog.h"
#include "JErrorHandlerUtils.h"
#include "../pbnjson_c/jschema_types_internal.h"
#include "../pbnjson_c/validation/error_code.h"
#include "JSchemaResolverWrapper.h"

namespace pbnjson {

namespace {

bool OnErrorParser(void *ctxt, JSAXContextRef parseCtxt)
{
	JErrorHandler *errorHandler = static_cast<JErrorHandler *>(ctxt);
	if (errorHandler)
		errorHandler->syntax(NULL, JErrorHandler::ERR_SYNTAX_GENERIC, "error parsing");
	return false;
}

bool OnErrorSchema(void *ctxt, JSAXContextRef parseCtxt)
{
	JErrorHandler *errorHandler = static_cast<JErrorHandler *>(ctxt);
	if (errorHandler)
		errorHandler->schema(NULL, ErrorToSchemaError(parseCtxt->m_error_code), ValidationGetErrorMessage(parseCtxt->m_error_code));
	return false;
}

bool OnErrorUnknown(void *ctxt, JSAXContextRef parseCtxt)
{
	JErrorHandler *errorHandler = static_cast<JErrorHandler *>(ctxt);
	if (errorHandler)
		errorHandler->misc(NULL, "unknown error parsing");
	return false;
}

} //namespace;

jschema_ref
JSchemaFile::createSchemaMap(const std::string &path, const std::string& rootScope, JErrorHandler *errorHandler, JSchemaResolverRef resolver)
{
	JErrorCallbacks error_callbacks = { 0 };
	error_callbacks.m_parser = OnErrorParser;
	error_callbacks.m_schema = OnErrorSchema;
	error_callbacks.m_unknown = OnErrorUnknown;
	error_callbacks.m_ctxt = errorHandler;

	return jschema_parse_file_resolve(path.c_str(), rootScope.c_str(), &error_callbacks, resolver);
}

JSchemaFile::JSchemaFile(const std::string& path)
	: JSchema(createSchemaMap(path, path, NULL, NULL))
{
}

JSchemaFile::JSchemaFile(const std::string& path, const std::string& rootScope, JErrorHandler *errorHandler, JResolver *resolver)
{
	JSchemaResolverWrapper resolverWrapper(resolver);
	JSchemaResolver schemaresolver;
	schemaresolver.m_resolve = &(resolverWrapper.sax_schema_resolver);
	schemaresolver.m_userCtxt = &resolverWrapper;
	schemaresolver.m_inRecursion = 0;

	set(createSchemaMap(path, rootScope, errorHandler, &schemaresolver));
}

JSchemaFile::JSchemaFile(const JSchemaFile& other)
	: JSchema(other)
{
}

JSchemaFile::~JSchemaFile()
{
}

}
