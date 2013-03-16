/******************************************************************************
 * Icinga 2                                                                   *
 * Copyright (C) 2012 Icinga Development Team (http://www.icinga.org/)        *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License                *
 * as published by the Free Software Foundation; either version 2             *
 * of the License, or (at your option) any later version.                     *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software Foundation     *
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ******************************************************************************/

#ifndef VALUE_H
#define VALUE_H

#include "base/object.h"
#include "base/qstring.h"
#include <boost/variant.hpp>

struct cJSON;

namespace icinga
{

/**
 * The type of a Value.
 *
 * @ingroup base
 */
enum ValueType
{
	ValueEmpty = 0,
	ValueNumber = 1,
	ValueString = 2,
	ValueObject = 3
};

/**
 * A type that can hold an arbitrary value.
 *
 * @ingroup base
 */
class I2_BASE_API Value
{
public:
	inline Value(void)
		: m_Value()
	{ }

	inline Value(int value)
		: m_Value(value)
	{ }

	inline Value(long value)
		: m_Value(double(value))
	{ }

	inline Value(double value)
		: m_Value(value)
	{ }

	inline Value(const String& value)
		: m_Value(value)
	{ }

	inline Value(const char *value)
		: m_Value(String(value))
	{ }

	template<typename T>
	inline Value(const shared_ptr<T>& value)
		: m_Value()
	{
		if (!value)
			return;

		Object::Ptr object = dynamic_pointer_cast<Object>(value);

		if (!object)
			BOOST_THROW_EXCEPTION(std::invalid_argument("shared_ptr value type must inherit from Object class."));

		m_Value = object;
	}

	operator double(void) const;
	operator String(void) const;

	template<typename T>
	operator shared_ptr<T>(void) const
	{
		if (IsEmpty())
			return shared_ptr<T>();

		shared_ptr<T> object = dynamic_pointer_cast<T>(boost::get<Object::Ptr>(m_Value));

		if (!object)
			BOOST_THROW_EXCEPTION(std::bad_cast());

		return object;
	}

	bool IsEmpty(void) const;
	bool IsScalar(void) const;
	bool IsObject(void) const;

	template<typename T>
	bool IsObjectType(void) const
	{
		if (!IsObject())
			return false;

		return (dynamic_pointer_cast<T>(boost::get<Object::Ptr>(m_Value)));
	}

	static Value FromJson(cJSON *json);
	cJSON *ToJson(void) const;

	String Serialize(void) const;
	static Value Deserialize(const String& jsonString);

	ValueType GetType(void) const;

private:
	mutable boost::variant<boost::blank, double, String, Object::Ptr> m_Value;
};

static Value Empty;

}

#endif /* VALUE_H */
