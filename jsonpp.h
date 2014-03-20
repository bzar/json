#ifndef JSONPP_H
#define JSONPP_H

#include "json.h"
#include <string>
#include <stdexcept>

class JSONValue
{
public:
  enum class Type {
    NONE,
    NULL_JSON,
    BOOLEAN,
    NUMBER,
    STRING,
    ARRAY,
    OBJECT,
    ERROR
  };

  class ValueException : public std::runtime_error
  {
  public:
    ValueException(std::string const& reason) : std::runtime_error(reason)
    {
    }
  };

  static JSONValue null();
  static JSONValue boolean(bool value);
  static JSONValue number(double value);
  static JSONValue string(std::string const& value);
  static JSONValue array();
  static JSONValue object();

  JSONValue(JSON_Value* value = nullptr);
  JSONValue(JSONValue const& other);
  JSONValue(JSONValue&& other);
  ~JSONValue();

  JSONValue& operator=(JSONValue const& other);
  JSONValue& operator=(JSONValue&& other);
  JSON_Value* extract();
  Type type() const;
  bool booleanValue() const;
  double numberValue() const;
  std::string stringValue() const;
  unsigned int size() const;
  JSONValue at(unsigned int index) const;
  JSONValue& append(JSONValue const& value);
  JSONValue get(std::string const& property) const;
  JSONValue& set(std::string const& property, JSONValue const& value);

  static JSONValue parse(std::string const& str);
  std::string toString(unsigned long initialSize = 1024) const;

private:
  JSON_Value* _value;
};

JSONValue::JSONValue(JSON_Value* value) : _value(value)
{
}
JSONValue JSONValue::null()
{
  return JSONValue(JSON_Value_New_Null());
}

JSONValue JSONValue::boolean(bool value)
{
  return JSONValue(value ? JSON_Value_New_True() : JSON_Value_New_False());
}
JSONValue JSONValue::number(double value)
{
  return JSONValue(JSON_Value_New_Number(value));
}
JSONValue JSONValue::string(std::string const& value)
{
  return JSONValue(JSON_Value_New_String(value.data()));
}
JSONValue JSONValue::array()
{
  return JSONValue(JSON_Value_New_Array());
}
JSONValue JSONValue::object()
{
  return JSONValue(JSON_Value_New_Object());
}

JSONValue::JSONValue(JSONValue const& other) : _value(JSON_Value_Clone(other._value))
{
}
JSONValue::JSONValue(JSONValue&& other) : _value(other.extract())
{
}
JSONValue::~JSONValue()
{
  if(_value != nullptr)
  {
    JSON_Value_Free(_value);
  }
}

JSONValue& JSONValue::operator=(JSONValue const& other)
{
  if(&other != this)
  {
    if(_value != nullptr)
    {
      JSON_Value_Free(_value);
    }
    _value = JSON_Value_Clone(other._value);
  }

  return *this;
}
JSONValue& JSONValue::operator=(JSONValue&& other)
{
  if(_value != nullptr)
  {
    JSON_Value_Free(_value);
  }
  _value = other.extract();

  return *this;
}
JSON_Value* JSONValue::extract()
{
  JSON_Value* result = _value;
  _value = nullptr;
  return result;
}

JSONValue::Type JSONValue::type() const
{
  Type result = Type::NONE;
  if(_value != nullptr)
  {
    switch(_value->type)
    {
      case JSON_VALUE_TYPE_NULL:
      {
        result = Type::NULL_JSON;
        break;
      }
      case JSON_VALUE_TYPE_TRUE:
      case JSON_VALUE_TYPE_FALSE:
      {
        result = Type::BOOLEAN;
        break;
      }
      case JSON_VALUE_TYPE_NUMBER:
      {
        result = Type::NUMBER;
        break;
      }
      case JSON_VALUE_TYPE_STRING:
      {
        result = Type::STRING;
        break;
      }
      case JSON_VALUE_TYPE_ARRAY:
      {
        result = Type::ARRAY;
        break;
      }
      case JSON_VALUE_TYPE_OBJECT:
      {
        result = Type::OBJECT;
        break;
      }
      case JSON_VALUE_TYPE_ERROR:
      default:
      {
        result = Type::ERROR;
        break;
      }
    }
  }

  return result;
}
bool JSONValue::booleanValue() const
{
  if(_value != nullptr && _value->type == JSON_VALUE_TYPE_TRUE)
  {
    return true;
  }
  else if(_value != nullptr && _value->type == JSON_VALUE_TYPE_FALSE)
  {
    return false;
  }
  else
  {
    throw ValueException("Not a boolean value");
  }
}
double JSONValue::numberValue() const
{
  if(_value != nullptr && _value->type == JSON_VALUE_TYPE_NUMBER)
  {
    return _value->number_value;
  }
  else
  {
    throw ValueException("Not a number value");
  }
}
std::string JSONValue::stringValue() const
{
  if(_value != nullptr && _value->type == JSON_VALUE_TYPE_STRING)
  {
    return _value->string_value;
  }
  else
  {
    throw ValueException("Not a string value");
  }
}
unsigned int JSONValue::size() const
{
  if(_value != nullptr && _value->type == JSON_VALUE_TYPE_ARRAY)
  {
    return JSON_Array_Size(_value);
  }
  else
  {
    throw ValueException("Not an array value");
  }

}
JSONValue JSONValue::at(unsigned int index) const
{
  if(_value != nullptr && _value->type == JSON_VALUE_TYPE_ARRAY)
  {
    JSON_Value* v = JSON_Array_Get_Value(_value, index);
    if(v == nullptr)
    {
      return JSONValue(nullptr);
    }

    return JSONValue(JSON_Value_Clone(v));
  }
  else
  {
    throw ValueException("Not an array value");
  }

}
JSONValue& JSONValue::append(JSONValue const& value)
{
  if(_value != nullptr && _value->type == JSON_VALUE_TYPE_ARRAY)
  {
    JSON_Array_Append(_value, JSON_Value_Clone(value._value));
  }
  else
  {
    throw ValueException("Not an array value");
  }

  return *this;
}

JSONValue JSONValue::get(std::string const& property) const
{
  if(_value != nullptr && _value->type == JSON_VALUE_TYPE_OBJECT)
  {
    JSON_Value* v = JSON_Object_Get_Property(_value, property.data());
    if(v == nullptr)
    {
      return JSONValue(nullptr);
    }
    return JSONValue(JSON_Value_Clone(v));
  }
  else
  {
    throw ValueException("Not an object value");
  }

}
JSONValue& JSONValue::set(std::string const& property, JSONValue const& value)
{
  if(_value != nullptr && _value->type == JSON_VALUE_TYPE_OBJECT)
  {
    JSON_Object_Set_Property(_value, property.data(), JSON_Value_Clone(value._value));
  }
  else
  {
    throw ValueException("Not an object value");
  }
  return *this;
}
JSONValue JSONValue::parse(std::string const& str)
{
  JSON_Value* v = JSON_Decode(str.data());
  return JSONValue(v);
}

std::string JSONValue::toString(unsigned long initialSize) const
{
  unsigned long size = initialSize;
  unsigned long length = 0;
  char* buffer = nullptr;

  do
  {
    if(buffer != nullptr)
    {
      delete[] buffer;
    }

    buffer = JSON_Encode(_value, size, &length);
  } while(length == size);

  std::string result(buffer);
  delete[] buffer;
  return result;
}

#endif // JSONPP_H
