#if !defined DURATION_HPP
#define DURATION_HPP

namespace tvr
{
	namespace ma
	{
		/// Represents how many seconds the system should play a given pitch or volume.
		struct duration
		{
			double _value;

			duration() :_value(0) {}
			duration(const duration& orig) :_value(orig._value) {}
			duration(double value) : _value(value) {}

			duration& operator=(const duration& orig) { _value = orig._value; return *this; }
			duration& operator=(double value) { _value = value; return *this; }
			duration& operator+=(const duration& orig) { _value += orig._value; return *this; }
			duration& operator-=(const duration& orig) { _value -= orig._value; return *this; }
			duration& operator*=(const duration& orig) { _value *= orig._value; return *this; }
			duration& operator/=(const duration& orig) { _value /= orig._value; return *this; }

			bool operator==(const duration& orig) const { return _value == orig._value; }
			bool operator!=(const duration& orig) const { return !(*this == orig); }
			bool operator<(const duration& orig) const { return _value < orig._value; }
			bool operator>(const duration& orig) const { return _value > orig._value; }
			bool operator<=(const duration& orig) const { return !(*this > orig); }
			bool operator>=(const duration& orig) const { return !(*this < orig); }

			static const duration& no_duration() { static duration none(0.0); return none; }
		};
	}
}



inline tvr::ma::duration operator-(const tvr::ma::duration& left, const tvr::ma::duration& right)
{
	tvr::ma::duration result = left;
	result -= right;
	return result;
}

inline tvr::ma::duration operator/(const tvr::ma::duration& left, const tvr::ma::duration& right)
{
	tvr::ma::duration result = left;
	result /= right;
	return result;
}

inline tvr::ma::duration operator+(const tvr::ma::duration& left, const tvr::ma::duration& right)
{
	tvr::ma::duration result = left;
	result += right;
	return result;
}

inline tvr::ma::duration operator*(const tvr::ma::duration& left, const tvr::ma::duration& right)
{
	tvr::ma::duration result = left;
	result *= right;
	return result;
}

#endif // DURATION_HPP

