/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#include "TestStatus.h"
#include "pbop/Status.h"
#include "rapidassist/testing.h"

using namespace pbop;

void TestStatus::SetUp()
{
}

void TestStatus::TearDown()
{
}

TEST_F(TestStatus, testGetSetCode)
{
  StatusCode expected_code = pbop::STATUS_CODE_NOT_IMPLEMENTED;
  
  Status s;

  //test default is something else than STATUS_CODE_NOT_IMPLEMENTED
  StatusCode default_code = s.GetCode();
  ASSERT_NE(expected_code, default_code);

  //test setter & getter
  s.SetCode(expected_code);
  StatusCode actual_code = s.GetCode();
  ASSERT_EQ(expected_code, actual_code);
}

TEST_F(TestStatus, testGetSetMessage)
{
  Status s;

  //test default is empty string
  std::string expected_message = "";
  std::string default_message = s.GetDescription();
  ASSERT_EQ(expected_message, default_message);

  //test setter & getter
  expected_message = "foobar";
  s.SetDescription(expected_message);
  std::string actual_message = s.GetDescription();
  ASSERT_EQ(expected_message, actual_message);
}

TEST_F(TestStatus, testSuccess)
{
  Status s;

  //test default is false
  ASSERT_FALSE(s.Success());

  //success code is Success
  s.SetCode(pbop::STATUS_CODE_SUCCESS);
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  //test all other codes are failures.
  for(int i = 1; i <= 10; i++)  
  {
    StatusCode code = static_cast<StatusCode>(i);
    s.SetCode(code);
    ASSERT_FALSE(s.Success());
  }
}

TEST_F(TestStatus, testOperatorEquals)
{
  StatusCode code = pbop::STATUS_CODE_INVALID_ARGUMENT;
  std::string message = ra::testing::GetTestQualifiedName();

  Status s1;

  s1.SetCode(code);
  s1.SetDescription(message);

  //test operator =
  {
    Status s2;
    s2 = s1;
    ASSERT_EQ(s1.GetCode(), s2.GetCode());
    ASSERT_EQ(s1.GetDescription(), s2.GetDescription());
  }

  //test assignment =
  {
    Status s2 = s1;
    ASSERT_EQ(s1.GetCode(), s2.GetCode());
    ASSERT_EQ(s1.GetDescription(), s2.GetDescription());
  }

  //test copy ctor
  {
    Status s2(s1);
    ASSERT_EQ(s1.GetCode(), s2.GetCode());
    ASSERT_EQ(s1.GetDescription(), s2.GetDescription());
  }
}

TEST_F(TestStatus, testOperatorEqualsEquals)
{
  StatusCode code = pbop::STATUS_CODE_INVALID_ARGUMENT;
  std::string message = ra::testing::GetTestQualifiedName();

  Status s1;

  s1.SetCode(code);
  s1.SetDescription(message);

  //test both element are equals
  {
    Status s2 = s1;
    ASSERT_TRUE (s1 == s2);
    ASSERT_FALSE(s1 != s2);
  }

  //test code different
  {
    Status s2 = s1;
    s2.SetCode(pbop::STATUS_CODE_OUT_OF_MEMORY);
    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE (s1 != s2);
  }

  //test message different
  {
    Status s2 = s1;
    s2.SetDescription("foobar");
    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE (s1 != s2);
  }

  //test both different
  {
    Status s2;
    s2.SetCode(pbop::STATUS_CODE_OUT_OF_MEMORY);
    s2.SetDescription("foobar");
    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE (s1 != s2);
  }
}

std::string ToStringLocal(const StatusCode & code)
{
  const char * name = Status::ToString(code);
  if (name == NULL)
    return "";
  std::string str = name;
  return str;
}

TEST_F(TestStatus, testToString)
{
  StatusCode code;
  std::string str;

  //test known codes
  code = pbop::STATUS_CODE_SUCCESS          ; str = ToStringLocal(code); ASSERT_FALSE(str.empty());
  code = pbop::STATUS_CODE_UNKNOWN          ; str = ToStringLocal(code); ASSERT_FALSE(str.empty());
  code = pbop::STATUS_CODE_SERIALIZE_ERROR  ; str = ToStringLocal(code); ASSERT_FALSE(str.empty());
  code = pbop::STATUS_CODE_DESERIALIZE_ERROR; str = ToStringLocal(code); ASSERT_FALSE(str.empty());
  code = pbop::STATUS_CODE_TIMED_OUT        ; str = ToStringLocal(code); ASSERT_FALSE(str.empty());
  code = pbop::STATUS_CODE_CANCELLED        ; str = ToStringLocal(code); ASSERT_FALSE(str.empty());
  code = pbop::STATUS_CODE_NOT_IMPLEMENTED  ; str = ToStringLocal(code); ASSERT_FALSE(str.empty());
  code = pbop::STATUS_CODE_INVALID_ARGUMENT ; str = ToStringLocal(code); ASSERT_FALSE(str.empty());
  code = pbop::STATUS_CODE_OUT_OF_RANGE     ; str = ToStringLocal(code); ASSERT_FALSE(str.empty());
  code = pbop::STATUS_CODE_OUT_OF_MEMORY    ; str = ToStringLocal(code); ASSERT_FALSE(str.empty());
  code = pbop::STATUS_CODE_PIPE_ERROR       ; str = ToStringLocal(code); ASSERT_FALSE(str.empty());
  code = pbop::STATUS_CODE_IMPLEMENTATION   ; str = ToStringLocal(code); ASSERT_FALSE(str.empty());

  //test for new codes
  {
    StatusCode code_max = STATUS_CODE_SUCCESS;
    StatusCode code_min = STATUS_CODE_SUCCESS;

    code = pbop::STATUS_CODE_SUCCESS          ; if (code > code_max) code_max = code; if (code < code_min) code_min = code;
    code = pbop::STATUS_CODE_UNKNOWN          ; if (code > code_max) code_max = code; if (code < code_min) code_min = code;
    code = pbop::STATUS_CODE_SERIALIZE_ERROR  ; if (code > code_max) code_max = code; if (code < code_min) code_min = code;
    code = pbop::STATUS_CODE_DESERIALIZE_ERROR; if (code > code_max) code_max = code; if (code < code_min) code_min = code;
    code = pbop::STATUS_CODE_TIMED_OUT        ; if (code > code_max) code_max = code; if (code < code_min) code_min = code;
    code = pbop::STATUS_CODE_CANCELLED        ; if (code > code_max) code_max = code; if (code < code_min) code_min = code;
    code = pbop::STATUS_CODE_NOT_IMPLEMENTED  ; if (code > code_max) code_max = code; if (code < code_min) code_min = code;
    code = pbop::STATUS_CODE_INVALID_ARGUMENT ; if (code > code_max) code_max = code; if (code < code_min) code_min = code;
    code = pbop::STATUS_CODE_OUT_OF_RANGE     ; if (code > code_max) code_max = code; if (code < code_min) code_min = code;
    code = pbop::STATUS_CODE_OUT_OF_MEMORY    ; if (code > code_max) code_max = code; if (code < code_min) code_min = code;
    code = pbop::STATUS_CODE_PIPE_ERROR       ; if (code > code_max) code_max = code; if (code < code_min) code_min = code;
    code = pbop::STATUS_CODE_IMPLEMENTATION   ; if (code > code_max) code_max = code; if (code < code_min) code_min = code;

    //all codes between code_min and code_max should have a value ToString() value
    for(int i = code_min; i <= code_max; i++)
    {
      StatusCode tmp = static_cast<StatusCode>(i);
      str = ToStringLocal(tmp);
      ASSERT_FALSE(str.empty()) << "The StatusCode with value " << i << " does not define a value for Status::ToString(). Is this a new value?";
    }
  }

  //test for undefined (non existing) status codes
  //they should be reported as "" (empty string)
  for(int i=0; i<=100; i++)
  {
    StatusCode code = static_cast<StatusCode>(i);

    //ignore known codes already tested above
    if (i == STATUS_CODE_SUCCESS             ||
        i == STATUS_CODE_UNKNOWN             ||
        i == STATUS_CODE_SERIALIZE_ERROR     ||
        i == STATUS_CODE_DESERIALIZE_ERROR   ||
        i == STATUS_CODE_TIMED_OUT           ||
        i == STATUS_CODE_CANCELLED           ||
        i == STATUS_CODE_NOT_IMPLEMENTED     ||
        i == STATUS_CODE_INVALID_ARGUMENT    ||
        i == STATUS_CODE_OUT_OF_RANGE        ||
        i == STATUS_CODE_OUT_OF_MEMORY       ||
        i == STATUS_CODE_PIPE_ERROR          ||
        i == STATUS_CODE_IMPLEMENTATION       
      )
      continue;

    const char * name = Status::ToString(code);
    ASSERT_TRUE(name != NULL);
    std::string str = name;
    ASSERT_TRUE(str.empty());
  }
}
