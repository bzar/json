#ifndef INCLUDED_JSON_H
#define INCLUDED_JSON_H

#ifdef __cplusplus
extern "C" {
#endif

enum JSON_Value_Type {
  JSON_VALUE_TYPE_STRING = 1,
  JSON_VALUE_TYPE_NUMBER,
  JSON_VALUE_TYPE_OBJECT,
  JSON_VALUE_TYPE_ARRAY,
  JSON_VALUE_TYPE_TRUE,
  JSON_VALUE_TYPE_FALSE,
  JSON_VALUE_TYPE_NULL,
  JSON_VALUE_TYPE_ERROR
};

struct JSON_Value {
  enum JSON_Value_Type type;
  union {
    const char *string_value;
    double number_value;
    struct JSON_Value *compound_value;
  };
  struct JSON_Value *next_value;
};

struct JSON_Error {
  enum JSON_Value_Type type;
  int line_number;
  const char *message;
};

struct JSON_Value* JSON_Decode(const char* jsonData);
char* JSON_Encode(struct JSON_Value* pValue, unsigned long allocSize, unsigned long *length);

struct JSON_Value* JSON_Value_New_String(const char* stringData);
struct JSON_Value* JSON_Value_New_Number(double numberValue);
struct JSON_Value* JSON_Value_New_Object();
struct JSON_Value* JSON_Value_New_Array();
struct JSON_Value* JSON_Value_New_True();
struct JSON_Value* JSON_Value_New_False();
struct JSON_Value* JSON_Value_New_Null();
struct JSON_Value* JSON_Value_Clone(struct JSON_Value* value);
void JSON_Object_Set_Property(struct JSON_Value* object, const char* property, struct JSON_Value* value);
struct JSON_Value* JSON_Object_Get_Property(struct JSON_Value* object, const char* property);
int JSON_Object_Properties_Count(struct JSON_Value* object);
int JSON_Object_Properties(struct JSON_Value* object, char** buffer, unsigned long bufferSize);
void JSON_Array_Append(struct JSON_Value* array, struct JSON_Value* value);
unsigned int JSON_Array_Size(struct JSON_Value* array);
void JSON_Value_Free(struct JSON_Value* pValue);
struct JSON_Value* JSON_Array_Get_Value(struct JSON_Value* array, unsigned int index);

#ifdef __cplusplus
}
#endif

#endif
