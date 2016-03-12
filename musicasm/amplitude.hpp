#if !defined AMPLITUDE_HPP
#define AMPLITUDE_HPP

namespace tvr
{
	namespace ma
	{
		/// Represents an amplitude for a sound.
		struct amplitude
		{
			double _value;

			amplitude() :_value(0) {}
			amplitude(double value) :_value(value) {}
			amplitude(const amplitude& orig) :_value(orig._value) {}

			amplitude& operator=(const amplitude& orig) { _value = orig._value; return *this; }
			bool operator==(const amplitude& orig) { return _value == orig._value; }
			bool operator<(const amplitude& orig) { return _value < orig._value; }
			amplitude& operator+=(const amplitude& orig) { _value += orig._value; return *this; }
			amplitude& operator-=(const amplitude& orig) { _value -= orig._value; return *this; }

			operator double() const { return _value; }
		};
	}
}

#endif // AMPLITUDE_HPP
