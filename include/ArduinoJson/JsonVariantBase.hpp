// Copyright Benoit Blanchon 2014-2017
// MIT License
//
// Arduino JSON library
// https://github.com/bblanchon/ArduinoJson
// If you like this project, please add a star!

#pragma once

#include "Data/JsonVariantAs.hpp"
#include "Polyfills/attributes.hpp"
#include "Serialization/JsonPrintable.hpp"
#include "TypeTraits/IsArray.hpp"

namespace ArduinoJson {

// Forward declarations.
class JsonArraySubscript;
template <typename TKey>
class JsonObjectSubscript;
template <typename TImpl>
class JsonVariantBase;

namespace TypeTraits {
template <typename T>
struct IsVariant : IsBaseOf<JsonVariantBase<T>, T> {};
}

template <typename TImpl>
class JsonVariantBase : public Internals::JsonPrintable<TImpl> {
 public:
#if ARDUINOJSON_ENABLE_DEPRECATED
  DEPRECATED("use as<JsonArray>() instead")
  FORCE_INLINE JsonArray &asArray() const {
    return as<JsonArray>();
  }

  DEPRECATED("use as<JsonObject>() instead")
  FORCE_INLINE JsonObject &asObject() const {
    return as<JsonObject>();
  }

  DEPRECATED("use as<char*>() instead")
  FORCE_INLINE const char *asString() const {
    return as<const char *>();
  }
#endif

  // Gets the variant as an array.
  // Returns a reference to the JsonArray or JsonArray::invalid() if the
  // variant
  // is not an array.
  FORCE_INLINE operator JsonArray &() const {
    return as<JsonArray &>();
  }

  // Gets the variant as an object.
  // Returns a reference to the JsonObject or JsonObject::invalid() if the
  // variant is not an object.
  FORCE_INLINE operator JsonObject &() const {
    return as<JsonObject &>();
  }

  template <typename T>
  FORCE_INLINE operator T() const {
    return as<T>();
  }

  template <typename T>
  FORCE_INLINE const typename Internals::JsonVariantAs<T>::type as() const {
    return impl()->template as<T>();
  }

  template <typename T>
  FORCE_INLINE bool is() const {
    return impl()->template is<T>();
  }

  // Mimics an array or an object.
  // Returns the size of the array or object if the variant has that type.
  // Returns 0 if the variant is neither an array nor an object
  size_t size() const {
    return as<JsonArray>().size() + as<JsonObject>().size();
  }

  // Mimics an array.
  // Returns the element at specified index if the variant is an array.
  // Returns JsonVariant::invalid() if the variant is not an array.
  FORCE_INLINE const JsonArraySubscript operator[](int index) const;
  FORCE_INLINE JsonArraySubscript operator[](int index);

  // Mimics an object.
  // Returns the value associated with the specified key if the variant is
  // an object.
  // Return JsonVariant::invalid() if the variant is not an object.
  //
  // const JsonObjectSubscript operator[](TKey) const;
  // TKey = const std::string&, const String&
  template <typename TString>
  FORCE_INLINE typename TypeTraits::EnableIf<
      Internals::StringTraits<TString>::has_equals,
      const JsonObjectSubscript<const TString &> >::type
  operator[](const TString &key) const {
    return as<JsonObject>()[key];
  }
  //
  // const JsonObjectSubscript operator[](TKey) const;
  // TKey = const char*, const char[N], const FlashStringHelper*
  template <typename TString>
  FORCE_INLINE typename TypeTraits::EnableIf<
      Internals::StringTraits<TString>::has_equals,
      JsonObjectSubscript<const TString &> >::type
  operator[](const TString &key) {
    return as<JsonObject>()[key];
  }
  //
  // JsonObjectSubscript operator[](TKey);
  // TKey = const std::string&, const String&
  template <typename TString>
  FORCE_INLINE typename TypeTraits::EnableIf<
      Internals::StringTraits<TString *>::has_equals,
      JsonObjectSubscript<const TString *> >::type
  operator[](const TString *key) {
    return as<JsonObject>()[key];
  }
  //
  // JsonObjectSubscript operator[](TKey);
  // TKey = const char*, const char[N], const FlashStringHelper*
  template <typename TString>
  FORCE_INLINE typename TypeTraits::EnableIf<
      Internals::StringTraits<TString *>::has_equals,
      const JsonObjectSubscript<const TString *> >::type
  operator[](const TString *key) const {
    return as<JsonObject>()[key];
  }

  // bool operator==(T)
  //
  // T = JsonVariant
  template <typename T>
  typename TypeTraits::EnableIf<TypeTraits::IsVariant<T>::value, bool>::type
  operator==(const T &comparand) const {
    using namespace Internals;
    if (is<bool>() && comparand.is<bool>())
      return as<bool>() == comparand.as<bool>();
    if (is<JsonFloat>() && comparand.is<JsonFloat>())
      return as<JsonFloat>() == comparand.as<JsonFloat>();
    if (is<JsonInteger>() && comparand.is<JsonInteger>())
      return as<JsonInteger>() == comparand.as<JsonInteger>();
    if (is<char *>() && comparand.is<char *>())
      return !strcmp(as<char *>(), comparand.as<char *>());
    if (is<JsonArray>() && comparand.is<JsonArray>())
      return as<JsonArray>() == comparand.as<JsonArray>();
    if (is<JsonObject>() && comparand.is<JsonObject>())
      return as<JsonObject>() == comparand.as<JsonObject>();
    return false;
  }
  //
  // T = const char*, const char[N], const FlashStringHelper*
  template <typename T>
  typename TypeTraits::EnableIf<TypeTraits::IsString<T *>::value, bool>::type
  operator==(const T *comparand) const {
    return Internals::StringTraits<T *>::equals(comparand, as<char *>());
  }
  //
  // T = const std::string&, const String&
  template <typename T>
  typename TypeTraits::EnableIf<TypeTraits::IsString<T>::value, bool>::type
  operator==(const T &comparand) const {
    return Internals::StringTraits<T>::equals(comparand, as<char *>());
  }
  //
  // T = bool, int, double
  template <typename T>
  typename TypeTraits::EnableIf<!TypeTraits::IsString<T>::value &&
                                    !TypeTraits::IsArray<T>::value &&
                                    !TypeTraits::IsVariant<T>::value,
                                bool>::type
  operator==(const T &comparand) const {
    return as<T>() == comparand;
  }

  // bool operator!=(T)
  //
  // T = const char*, const char[N], const FlashStringHelper*
  template <typename T>
  typename TypeTraits::EnableIf<TypeTraits::IsString<T *>::value, bool>::type
  operator!=(const T *comparand) const {
    return !Internals::StringTraits<T *>::equals(comparand, as<char *>());
  }
  //
  // T = const std::string&, const String&
  template <typename T>
  typename TypeTraits::EnableIf<TypeTraits::IsString<T>::value, bool>::type
  operator!=(const T &comparand) const {
    return !Internals::StringTraits<T>::equals(comparand, as<char *>());
  }
  //
  // T = bool, int, double
  template <typename T>
  typename TypeTraits::EnableIf<!TypeTraits::IsString<T>::value &&
                                    !TypeTraits::IsArray<T>::value &&
                                    !TypeTraits::IsVariant<T>::value,
                                bool>::type
  operator!=(const T &comparand) const {
    return as<T>() != comparand;
  }

 private:
  const TImpl *impl() const {
    return static_cast<const TImpl *>(this);
  }
};
}
