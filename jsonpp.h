#ifndef JSONPP_H
#define JSONPP_H

#include "json.h"
#include <string>
#include <vector>
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
  std::vector<std::string> properties() const;

  static JSONValue parse(std::string const& str);
  std::string toString(unsigned long initialSize = 1024) const;

private:
  JSON_Value* _value;
};

#endif // JSONPP_H
