#if !defined _BASIC_VALUE_HPP
#define _BASIC_VALUE_HPP

/*
 * "Why do this?" you may ask.
 *
 * I started to get a little bit confused between the various kinds of doubles in this system, and wanted
 * a way I could recognize different kinds of values from each other so I wouldn't accidentally confuse
 * them.  I'd seen a technique like this used for scientific formulas, and thought it seemed rather useful
 * to solve my current problem.
 *
 * I originally wrote separate classes, but they were identical, so I thought I had best just take this
 * approach to ease maintenance.  This way, I can just make changes here to impact all of the tagged types.
 */

namespace tvr
{
	namespace ma
	{
		/// Represents a tagged double used by the musicasm system to store information.
		template< int tag >
		struct _basic_value
		{
			double _value;

			_basic_value() :_value(0) {}
			_basic_value(const _basic_value& orig) :_value(orig._value) {}
			_basic_value(double value) : _value(value) {}

			_basic_value& operator=(const _basic_value& orig) { _value = orig._value; return *this; }
			_basic_value& operator=(double value) { _value = value; return *this; }
			_basic_value& operator+=(const _basic_value& orig) { _value += orig._value; return *this; }
			_basic_value& operator-=(const _basic_value& orig) { _value -= orig._value; return *this; }
			_basic_value& operator*=(const _basic_value& orig) { _value *= orig._value; return *this; }
			_basic_value& operator/=(const _basic_value& orig) { _value /= orig._value; return *this; }

			bool operator==(const _basic_value& orig) const { return _value == orig._value; }
			bool operator!=(const _basic_value& orig) const { return !(*this == orig); }
			bool operator<(const _basic_value& orig) const { return _value < orig._value; }
			bool operator>(const _basic_value& orig) const { return _value > orig._value; }
			bool operator<=(const _basic_value& orig) const { return !(*this > orig); }
			bool operator>=(const _basic_value& orig) const { return !(*this < orig); }

			operator double() const { return _value; }

			static int get_tag() { return tag; }
			static const _basic_value& zero() { static _basic_value none(0.0); return none; }
			static const _basic_value& negative() { static _basic_value neg(-1.0); return neg; }
		};
		
		const int duration_tag = 0;
		const int amplitude_tag = 1;
		const int frequency_tag = 2;

		/// Represents how long to perform a given frequency or amplitude.
		typedef _basic_value<duration_tag> duration;
		/// Represents the volume of a wave.
		typedef _basic_value<amplitude_tag> amplitude;
		/// Represents the pitch of the wave.
		typedef _basic_value<frequency_tag> frequency;
	}
}

template< int tag >
inline tvr::ma::_basic_value<tag> operator-(const tvr::ma::_basic_value<tag>& left, const tvr::ma::_basic_value<tag>& right)
{
	tvr::ma::_basic_value<tag> result = left;
	result -= right;
	return result;
}

template< int tag >
inline tvr::ma::_basic_value<tag> operator/(const tvr::ma::_basic_value<tag>& left, const tvr::ma::_basic_value<tag>& right)
{
	tvr::ma::_basic_value<tag> result = left;
	result /= right;
	return result;
}

template< int tag >
inline tvr::ma::_basic_value<tag> operator+(const tvr::ma::_basic_value<tag>& left, const tvr::ma::_basic_value<tag>& right)
{
	tvr::ma::_basic_value<tag> result = left;
	result += right;
	return result;
}

template< int tag >
inline tvr::ma::_basic_value<tag> operator*(const tvr::ma::_basic_value<tag>& left, const tvr::ma::_basic_value<tag>& right)
{
	tvr::ma::_basic_value<tag> result = left;
	result *= right;
	return result;
}

#endif // _BASIC_VALUE_HPP
