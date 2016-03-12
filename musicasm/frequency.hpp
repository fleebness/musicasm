#if !defined FREQUENCY_HPP
#define FREQUENCY_HPP

namespace tvr
{
	namespace ma
	{
		/// Represents a frequency for a sound.
		struct frequency
		{
			double _value;

			frequency():_value(0){}
			frequency(double value) :_value(value){}
			frequency(const frequency& orig):_value(orig._value) {}

			frequency& operator=(const frequency& orig) { _value = orig._value; return *this; }
			bool operator==(const frequency& orig) { return _value == orig._value; }
			bool operator<(const frequency& orig) { return _value < orig._value; }
			frequency& operator+=(const frequency& orig) { _value += orig._value; return *this; }
			frequency& operator-=(const frequency& orig) { _value -= orig._value; return *this; }

			operator double() const { return _value; }
		};
	}
}

#endif // FREQUENCY_HPP
