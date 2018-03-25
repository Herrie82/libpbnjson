// Copyright (c) 2013-2018 LG Electronics, Inc.
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

#include <gtest/gtest.h>
#include <pbnjson.hpp>

using namespace pbnjson;
using namespace std;

TEST(TestJValue, Constructors)
{
	EXPECT_TRUE(JValue().isNull());
	EXPECT_TRUE(JValue(1).isNumber());
	EXPECT_TRUE(JValue("1").isString());
	EXPECT_TRUE(JValue(true).isBoolean());
	EXPECT_TRUE(Object().isObject());
	EXPECT_TRUE(JObject().isObject());
	EXPECT_TRUE(Array().isArray());
	EXPECT_TRUE(JArray().isArray());
	EXPECT_TRUE(JValue(nullptr).isNull());
}

TEST(TestJValue, Assignment)
{
	JValue valueNull = JValue();
	JValue string = JValue("test");
	JValue value = JValue(1);
	value = valueNull;
	EXPECT_TRUE(value == valueNull);
	value = string;
	EXPECT_TRUE(value == string);
	valueNull = string;
	EXPECT_TRUE(valueNull == value);
}

TEST(TestJValue, StringComparison)
{
	EXPECT_TRUE(JValue("t") == "t");
	EXPECT_TRUE(JValue("test") == "test");
	EXPECT_TRUE(JValue("test1") != "test");
	EXPECT_TRUE(JValue(1) != "1");
	EXPECT_TRUE(JValue(true) != "true");
	EXPECT_TRUE(Object() != "");
	EXPECT_TRUE(Array() != "");
}

TEST(TestJValue, BooleanComparison)
{
	EXPECT_TRUE(JValue(true) == true);
	EXPECT_TRUE(JValue(false) == false);
	EXPECT_TRUE(JValue(0) != false);
	EXPECT_TRUE(JValue("") != false);
	EXPECT_TRUE(Object() != false);
	EXPECT_TRUE(Array() != false);
}

TEST(TestJValue, NumberComparison)
{
	double d(0);
	int64_t int64t(1);
	int32_t int32t(2);

	EXPECT_TRUE(JValue(0) == d);
	EXPECT_TRUE(JValue(1) == int64t);
	EXPECT_TRUE(JValue(2) == int32t);
	EXPECT_TRUE(JValue(3) != d);
	EXPECT_TRUE(JValue(3) != int64t);
	EXPECT_TRUE(JValue(3) != int32t);
	EXPECT_TRUE(JValue(false) != d);
	EXPECT_TRUE(JValue(true) != int64t);
	EXPECT_TRUE(JValue(true) != int32t);
	EXPECT_TRUE(JValue("0") != d);
	EXPECT_TRUE(JValue("1") != int64t);
	EXPECT_TRUE(JValue("2") != int32t);
	EXPECT_TRUE(Object() != d);
	EXPECT_TRUE(Object() != int64t);
	EXPECT_TRUE(Object() != int32t);
	EXPECT_TRUE(Array() != d);
	EXPECT_TRUE(Array() != int64t);
	EXPECT_TRUE(Array() != int32t);
}

TEST(TestJValue, CompareEmpty)
{
	EXPECT_TRUE(JValue() == JValue());
}

TEST(TestJValue, CompareNumbers)
{
	EXPECT_FALSE(JValue(1) == JValue(2));
	EXPECT_TRUE(JValue(42) == JValue(42));
	EXPECT_FALSE(JValue(0.0) == JValue(1000000));
	EXPECT_TRUE(JValue(1.0) == JValue(1));
	EXPECT_FALSE(JValue(-1) == JValue(1));
}

TEST(TestJValue, CompareBooleans)
{
	EXPECT_TRUE(JValue(true) == JValue(true));
	EXPECT_TRUE(JValue(false) == JValue(false));
	EXPECT_FALSE(JValue(true) == JValue(false));
	EXPECT_FALSE(JValue(false) == JValue(true));
}

TEST(TestJValue, CompareStrings)
{
	EXPECT_FALSE(JValue("first") == JValue("second"));
	EXPECT_FALSE(JValue("") == JValue("second"));
	EXPECT_TRUE(JValue("") == JValue(""));
	EXPECT_TRUE(JValue("first") == JValue("first"));
}

TEST(TestJValue, CompareArrays)
{
	EXPECT_TRUE(Array() == Array());

	EXPECT_TRUE(Array() << "a1" == Array() << "a1");
	EXPECT_FALSE(Array() << "a1" == Array() << "a2");
	EXPECT_FALSE(Array() << "a1" == Array() << "a1" << "a1");
	EXPECT_FALSE(Array() << "a1" << "a2" == Array() << "a2" << "a1");
	EXPECT_TRUE(Array() << "a1" << "a2" == Array() << "a1" << "a2");
	EXPECT_FALSE(Array() << "a1" << "a2" == Array() << "a1" << "a1");
	EXPECT_FALSE(Array() << "a2" << "a1" == Array() << "a1" << "a1");

	EXPECT_FALSE(Array() << 1 << 2 == Array() << 2 << 1);
	EXPECT_TRUE(Array() << 1 << 2 == Array() << 1 << 2);

	EXPECT_FALSE(Array() << true << false == Array() << false << true);
	EXPECT_TRUE(Array() << true << false == Array() << true << false);

	EXPECT_TRUE(Array() << Object() == Array() << Object());
	EXPECT_FALSE(Array() << (Object() << JValue::KeyValue("id", 1))
	             == Array() << (Object() << JValue::KeyValue("id", 2)));
	EXPECT_TRUE(Array() << (Object() << JValue::KeyValue("id", 1))
	            == Array() << (Object() << JValue::KeyValue("id", 1)));
	EXPECT_FALSE(Array() << (Object() << JValue::KeyValue("id", 1))
	             == Array() << (Object() << JValue::KeyValue("id", 1))
	                        << (Object() << JValue::KeyValue("name", "John")));
}

TEST(TestJValue, CompareObjects)
{
	EXPECT_TRUE(Object() == Object());
	EXPECT_FALSE(Object() << JValue::KeyValue("id", 1) ==
	             Object() << JValue::KeyValue("id", 2));
	EXPECT_FALSE(Object() << JValue::KeyValue("id", 1) ==
	             Object() << JValue::KeyValue("id1", 1));
	EXPECT_TRUE(Object() << JValue::KeyValue("id", 1) ==
	            Object() << JValue::KeyValue("id", 1));
	EXPECT_FALSE(Object() << JValue::KeyValue("id", 1) ==
	             Object() << JValue::KeyValue("id", 1)
	                      << JValue::KeyValue("name", "John"));
	EXPECT_TRUE(Object() << JValue::KeyValue("id", 1)
	                     << JValue::KeyValue("name", "John")
	            ==
	            Object() << JValue::KeyValue("id", 1)
	                     << JValue::KeyValue("name", "John"));
	EXPECT_FALSE(Object() << JValue::KeyValue("id", 1)
	                      << JValue::KeyValue("name", "John")
	             ==
	             Object() << JValue::KeyValue("id", 1)
	                      << JValue::KeyValue("name1", "John"));
	EXPECT_FALSE(Object() << JValue::KeyValue("name", "John")
	             ==
	             Object() << JValue::KeyValue("name1", "John"));
	EXPECT_FALSE(Object() << JValue::KeyValue("name", "John")
	                      << JValue::KeyValue("bool", false)
	             ==
	             Object() << JValue::KeyValue("name", "John")
	                      << JValue::KeyValue("bool", true));
	EXPECT_TRUE(Object() << JValue::KeyValue("name", "John")
	                     << JValue::KeyValue("bool", false)
	            ==
	            Object() << JValue::KeyValue("name", "John")
	                     << JValue::KeyValue("bool", false));
	EXPECT_FALSE(Object() << JValue::KeyValue("name", "John")
	                      << JValue::KeyValue("bool", false)
	             ==
	             Object() << JValue::KeyValue("name", "John")
	                      << JValue::KeyValue("bool", JValue()));
	EXPECT_TRUE(Object() << JValue::KeyValue("array", Array())
	            ==
	            Object() << JValue::KeyValue("array", Array()));
	EXPECT_FALSE(Object() << JValue::KeyValue("array", Array() << 1)
	             ==
	             Object() << JValue::KeyValue("array", Array() << 2));
	EXPECT_TRUE(Object() << JValue::KeyValue("array", Array() << 1)
	            ==
	            Object() << JValue::KeyValue("array", Array() << 1));
	EXPECT_FALSE(Object() << JValue::KeyValue("array", Array() << 1)
	             ==
	             Object() << JValue::KeyValue("array", Array() << 2)
	                      << JValue::KeyValue("array2", Array() << 1));
}

TEST(TestJValue, CompareNumberAndBoolean)
{
	EXPECT_TRUE(JValue(1) != JValue(true));
	EXPECT_TRUE(JValue(0) != JValue(false));
	EXPECT_TRUE(JValue(false) != JValue(0));
}

TEST(TestJValue, CompareNumberAndString)
{
	EXPECT_TRUE(JValue(1) != JValue("1"));
	EXPECT_TRUE(JValue(3) != JValue("3"));
	EXPECT_TRUE(JValue("0") != JValue(0));
}

TEST(TestJValue, CompareNumberAndArray)
{
	EXPECT_TRUE(JValue(0) != Array());
	EXPECT_TRUE(JValue(0) != (Array() << 0));
	EXPECT_TRUE((Array() << 0) != JValue(0));
}

TEST(TestJValue, CompareNumberAndObject)
{
	EXPECT_TRUE(JValue(0) != Object());
	EXPECT_TRUE(JValue(0) != (Object() << JValue::KeyValue("number",0)));
	EXPECT_TRUE((Object() << JValue::KeyValue("number",0)) != JValue(0));
}

TEST(TestJValue, CompareBooleanAndString)
{
	EXPECT_TRUE(JValue(true) != JValue("true"));
	EXPECT_TRUE(JValue(false) != JValue("false"));
	EXPECT_TRUE(JValue("false") != JValue(false));
}

TEST(TestJValue, CompareBooleanAndArray)
{
	EXPECT_TRUE(JValue(false) != Array());
	EXPECT_TRUE(JValue(true) != (Array() << true));
	EXPECT_TRUE((Array() << true) != JValue(true));
}

TEST(TestJValue, CompareBooleanAndObject)
{
	EXPECT_TRUE(JValue(false) != Object());
	EXPECT_TRUE(JValue(true) != (Object() << JValue::KeyValue("boolean",true)));
	EXPECT_TRUE((Object() << JValue::KeyValue("boolean",true)) != JValue(true));
}

TEST(TestJValue, CompareStringAndArray)
{
	EXPECT_TRUE(JValue("") != Array());
	EXPECT_TRUE(JValue("test") != (Array() << "test"));
	EXPECT_TRUE((Array() << "test") != JValue("test"));
}

TEST(TestJValue, CompareStringAndObject)
{
	EXPECT_TRUE(JValue("") != Object());
	EXPECT_TRUE(JValue("test") != (Object() << JValue::KeyValue("string","test")));
	EXPECT_TRUE((Object() << JValue::KeyValue("string","test")) != JValue("test"));
}

TEST(TestJValue, CompareArrayAndObject)
{
	EXPECT_TRUE(Array() != Object());
	EXPECT_TRUE((Array() << 1) != (Object() << JValue::KeyValue("number",1)));
	EXPECT_TRUE((Object() << JValue::KeyValue("string","test")) != (Array() << "test"));
}

TEST(TestJValue, BracketsForSimpleTypes)
{
	JValue valueNull = JValue();
	JValue number = JValue(1);
	JValue boolean = JValue(true);
	JValue string = JValue("test");

	EXPECT_TRUE(valueNull[0] == JValue());
	EXPECT_TRUE(number[0] == JValue());
	EXPECT_TRUE(boolean[0] == JValue());
	EXPECT_TRUE(string[0] == JValue());
}

TEST(TestJValue, BracketsForArrays)
{
	JValue array = Array();
	array.append("1");
	EXPECT_TRUE(array[0] == std::string("1"));
	array.append(3);
	EXPECT_TRUE(array[1] == 3);
	EXPECT_TRUE(array[2] == JValue());
	array.append(JValue());
	EXPECT_TRUE(array[2] == JValue());
	EXPECT_TRUE(array["key"] == JValue());
}

TEST(TestJValue, BracketsForObjects)
{
	JValue object = Object();
	object.put("key1", "1");
	EXPECT_TRUE(object["key1"] == std::string("1"));
	object.put("key2", 3);
	EXPECT_TRUE(object["key2"] == 3);
	EXPECT_TRUE(object["key3"] == JValue());
	object.put("key3", JValue());
	EXPECT_TRUE(object["key3"] == JValue());
	EXPECT_TRUE(object[0] == JValue());
}

TEST(TestJValue, CastToBool)
{
	JValue object = Object();
	object.put("key1", "1");
	object.put("key2", 3);
	object.put("key3", JValue());
	EXPECT_TRUE((bool)object["key3"]);
	EXPECT_FALSE((bool)object["nokey"]);
	EXPECT_TRUE((bool)object["key2"]);
}

TEST(TestJValue, CopyConstructor)
{
	JValue v1(Object());
	v1.put("key1", "val1");
	JValue v2(v1);
	v2.put("key2", "val2");

	EXPECT_TRUE(v1 == v2);
}

TEST(TestJValue, MoveConstructor)
{
	{
	JValue v1;
	EXPECT_TRUE(v1.isNull());
	JValue v2(move(v1));
	EXPECT_TRUE(v2.isNull());
	}

	{
	JValue v1(42);
	EXPECT_TRUE(v1.isNumber());
	JValue v2(move(v1));
	EXPECT_TRUE(v2.isNumber());
	}

	{
	JValue v1("input");
	EXPECT_TRUE(v1.isString());
	JValue v2(move(v1));
	EXPECT_TRUE(v2.isString());
	}

	{
	JValue v1(Object());
	EXPECT_TRUE(v1.isObject());
	v1.put("answer", 42);
	JValue v2(move(v1));
	EXPECT_TRUE(v2.isObject());
	EXPECT_TRUE(v2["answer"] == 42);
	}

	{
	JObject jobj;
	JValue jval { move(jobj) };
	EXPECT_TRUE(jval.isObject());
	}

	{
	JArray jarr;
	JValue jval { move(jarr) };
	EXPECT_TRUE(jval.isArray());
	}
}

TEST(TestJValue, MoveAssignment)
{
	JValue valueNull = JValue();
	JValue string = JValue("test");
	JValue number = JValue(42);
	JObject jobj { {"k","v"} };
	JArray jarr { 1, 2, 3 };

	JValue value;

	value = move(number);
	EXPECT_TRUE(value.isNumber());

	value = move(string);
	EXPECT_TRUE(value.isString());

	value = move(valueNull);
	EXPECT_TRUE(value.isNull());

	value = move(jobj);
	EXPECT_TRUE(value.isObject());
	EXPECT_TRUE(value.objectSize() == 1);

	value = move(jarr);
	EXPECT_TRUE(value.isArray());
	EXPECT_TRUE(value.arraySize() == 3);
}

TEST(TestJValue, Duplicate)
{
	JValue v1(Object());
	v1.put("key1", "val1");
	JValue v2(v1.duplicate());
	v2.put("key2", "val2");

	EXPECT_TRUE(v1 != v2);
	EXPECT_TRUE(v1.hasKey("key1"));
	EXPECT_FALSE(v1.hasKey("key2"));
	EXPECT_TRUE(v2.hasKey("key1"));
	EXPECT_TRUE(v2.hasKey("key2"));
}

TEST(TestJValue, DuplicateArray)
{
	auto a = Array() << true << 42 << "hello";
	auto b = a.duplicate();

	EXPECT_TRUE(a == b);
}

TEST(TestJValue, IteratorAdvance)
{
	JValue obj = Object();
	EXPECT_TRUE(obj.objectSize() == 0);
	for (int i = 0; i != 10; ++i)
	{
		char key = 'a' + i;
		obj.put(string(&key, &key + 1), JValue(i));
	}
	ASSERT_EQ(10, obj.objectSize());

	auto it1 = obj.children().begin();
	for (int i = 1; i != 5; ++i)
	{
		JValue::KeyValue kv1 = *++it1;
		JValue::KeyValue kv2 = *(obj.children().begin() + i);
		EXPECT_TRUE(kv1 == kv2);
	}
}

TEST(TestJValue, GetType)
{
	EXPECT_EQ(JV_NULL, JValue().getType());
	EXPECT_EQ(JV_NUM, JValue(1).getType());
	EXPECT_EQ(JV_STR, JValue("1").getType());
	EXPECT_EQ(JV_BOOL, JValue(true).getType());
	EXPECT_EQ(JV_OBJECT, Object().getType());
	EXPECT_EQ(JV_ARRAY, Array().getType());
}

TEST(TestJValue, AsNumber)
{
	JDomParser parser(nullptr);
	parser.parse(R"({"value": -1.5})", JSchema::AllSchema(), nullptr);
	EXPECT_EQ(-1.5, parser.getDom()["value"].asNumber<double>());
	parser.parse(R"({"value": -0.3})", JSchema::AllSchema(), nullptr);
	EXPECT_EQ(-0.3, parser.getDom()["value"].asNumber<double>());
}

TEST(TestJValue, Stringify)
{
	{
		JValue json = Array();
		EXPECT_EQ("[]", json.stringify());
		json << JValue() << JValue(1) << JValue("test");
		EXPECT_EQ("[null,1,\"test\"]", json.stringify());
	}
	{
		JValue json = JValue(int32_t(1));
		EXPECT_EQ("1", json.stringify());
	}
	{
		JValue json = JValue(true);
		EXPECT_EQ("true", json.stringify());
	}
	{
		JValue json = JValue();
		EXPECT_EQ("null", json.stringify());
	}
	{
		JValue json = JValue("");
		EXPECT_EQ("\"\"", json.stringify());
	}
	{
		JValue json = JValue("test");
		EXPECT_EQ("\"test\"", json.stringify());
	}
	{
		JValue json = JValue(42323.0234234);
		EXPECT_EQ("42323.0234234", json.stringify());
	}
	{
		JValue json = JValue(int64_t(4292496729600));
		EXPECT_EQ("4292496729600", json.stringify());
	}
	{
		JValue json = Object();
		EXPECT_EQ("{}", json.stringify());
		//Error situation. It is not legal to put JValue, that is not Object or Array, to the Object.
		json << JValue(int32_t(1));
		EXPECT_EQ("{}", json.stringify());
	}
	{
		JValue json;
		EXPECT_EQ("null", json.stringify());
	}
}

TEST(TestJValue, Prettify)
{
	const char* INPUT = ( R"json(
		{
			"name": "Alisha"
		}
	)json");

	JValue json = JDomParser::fromString(INPUT);
	EXPECT_TRUE(json.isValid());

	std::string json_str = json.stringify("  ");
	std::string res2 = "{\n  \"name\": \"Alisha\"\n}\n";
	EXPECT_TRUE(json_str.compare(res2) == 0);

	json_str = json.stringify("\t");
	std::string res3 = "{\n\t\"name\": \"Alisha\"\n}\n";
	EXPECT_TRUE(json_str.compare(res3) == 0);

	json_str = json.stringify(".."); // not acceptable, using "  " instead
	EXPECT_TRUE(json_str.compare(res2) == 0);

}

TEST(TestJValue, GetError)
{
	JValue json = JDomParser::fromFile("file-file-blabla.bla");
	EXPECT_FALSE(json);

	std::string err_msg("Invalid parameters error. Can't open file: file-file-blabla.bla");

	EXPECT_TRUE(err_msg.compare(json.errorString()) == 0);
}

TEST(TestJValue, BraceEnclosedInitializerList)
{
	JValue jstr { "asd" };
	EXPECT_TRUE(jstr.isString());

	JValue jobj { {"asd", 42} };
	EXPECT_TRUE(jobj.isObject());

	JValue obj = JObject { { "k1", 12 }, { "k2", true } };
	EXPECT_TRUE(obj.objectSize() == 2);
	EXPECT_TRUE(obj["k1"] == 12);
	EXPECT_TRUE(obj["k2"].asBool()); // gtest need explicit conversion to bool

	JArray arr {"string", 13};
	EXPECT_TRUE(arr.arraySize() == 2);
	EXPECT_TRUE(arr[0] == "string");
	EXPECT_TRUE(arr[1] == 13);

	JObject v {
		{ "asd", 12 },
		{ "qwe", JObject { {"zxc", 345} } },
		{ "qaz", JArray {"wsx", 3.14, true, nullptr, 42} }
	};
	EXPECT_TRUE(v.objectSize() == 3);
	EXPECT_TRUE(v["qwe"].objectSize() == 1);
	EXPECT_TRUE(v["qaz"].arraySize() == 5);
	EXPECT_TRUE(v["qaz"][3] == JValue());
	EXPECT_TRUE(v["qaz"][1] == 3.14);
}

TEST(TestJValue, NumberAsNumber)
{
	int64_t i64val = 0;
	int32_t i32val = 0;
	double fval = 0;

	EXPECT_EQ(CONV_OK, JValue(3.141592).asNumber(fval));
	EXPECT_DOUBLE_EQ(3.141592, fval);

	EXPECT_EQ(CONV_PRECISION_LOSS, JValue(3.141592).asNumber(i32val));
	EXPECT_EQ(3, i32val);

	EXPECT_EQ(CONV_OK, JValue(std::numeric_limits<int64_t>::max()).asNumber(i64val));
	EXPECT_EQ(std::numeric_limits<int64_t>::max(), i64val);

	EXPECT_TRUE(CONV_HAS_POSITIVE_OVERFLOW(JValue(std::numeric_limits<int64_t>::max()).asNumber(i32val)));
	EXPECT_EQ(std::numeric_limits<int32_t>::max(), i32val);

	EXPECT_TRUE(CONV_HAS_NEGATIVE_OVERFLOW(JValue(std::numeric_limits<int64_t>::min()).asNumber(i32val)));
	EXPECT_EQ(std::numeric_limits<int32_t>::min(), i32val);

	EXPECT_EQ(CONV_PRECISION_LOSS, JValue(std::numeric_limits<int64_t>::max()).asNumber(fval));
	EXPECT_EQ(double(std::numeric_limits<int64_t>::max()), fval);

	EXPECT_TRUE(CONV_HAS_NEGATIVE_OVERFLOW(JValue(std::numeric_limits<int64_t>::min()).asNumber(i32val)));
	EXPECT_EQ(std::numeric_limits<int32_t>::min(), i32val);
}

TEST(TestJValue, NumberAsNumberString)
{
	std::string sval;

	EXPECT_EQ(CONV_NOT_A_RAW_NUM, JValue(3.141592).asNumber(sval));

	EXPECT_EQ(CONV_NOT_A_RAW_NUM, JValue(42).asNumber(sval));
}

TEST(TestJValue, NumericStringAsNumber)
{
	double fval;
	int64_t ival;

	EXPECT_EQ(CONV_OK, JValue(NumericString("3.141592")).asNumber(fval));
	EXPECT_DOUBLE_EQ(3.141592, fval);

	EXPECT_EQ(CONV_PRECISION_LOSS, JValue(NumericString("3.141592")).asNumber(ival));
	EXPECT_EQ(3, ival);

	EXPECT_EQ(CONV_OK, JValue(NumericString("42")).asNumber(fval));
	EXPECT_DOUBLE_EQ(42, fval);

	EXPECT_EQ(CONV_OK, JValue(NumericString("42")).asNumber(ival));
	EXPECT_EQ(42, fval);

	EXPECT_TRUE(CONV_HAS_POSITIVE_OVERFLOW(JValue(NumericString("1e1000000")).asNumber(fval)));
	EXPECT_DOUBLE_EQ(std::numeric_limits<decltype(fval)>::max(), fval);

	EXPECT_TRUE(CONV_HAS_POSITIVE_OVERFLOW(JValue(NumericString("1e1000000")).asNumber(ival)));
	EXPECT_EQ(std::numeric_limits<decltype(ival)>::max(), ival);
}

TEST(TestJValue, NumericStringAsNumberString)
{
	std::string sval;

	EXPECT_EQ(CONV_OK, JValue(NumericString("3.141592")).asNumber(sval));
	EXPECT_EQ(std::string("3.141592"), sval);

	EXPECT_EQ(CONV_OK, JValue(NumericString("42")).asNumber(sval));
	EXPECT_EQ(std::string("42"), sval);
}

TEST(TestJValue, NumberEquality)
{
	JValue a(NumericString("4.2e-4"));
	JValue b(NumericString("0.00042"));

	EXPECT_EQ( a, b );
	EXPECT_EQ( b, a );
}
