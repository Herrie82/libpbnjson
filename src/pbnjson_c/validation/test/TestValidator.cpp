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

#include "../validator.h"
#include "Util.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace std;

ValidatorVtable foo_vtable =
{
};

struct Foo
	: public Validator
{
};

TEST(Validator, Empty)
{
	Foo foo;
	validator_init(&foo, &foo_vtable);
	ASSERT_EQ(&foo_vtable, foo.vtable);
}
