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

#include <JSchemaFragment.h>

#include <pbnjson.h>

namespace pbnjson {

jschema_ref JSchemaFragment::createSchema(const std::string &fragment)
{
	raw_buffer schemaStr;
	schemaStr.m_str = fragment.c_str();
	schemaStr.m_len = fragment.length();

	return jschema_parse(schemaStr, JSCHEMA_DOM_NOOPT, NULL);
}

JSchemaFragment::JSchemaFragment(const std::string& fragment)
	: JSchema(createSchema(fragment))
{
}

JSchemaFragment::JSchemaFragment(const JSchemaFragment& other)
	: JSchema(other)
{
}

JSchemaFragment::~JSchemaFragment()
{
}

}

