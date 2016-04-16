#if !defined FRACTION_HPP
#define FRACTION_HPP

#include <cmath>

namespace tvr
{
	namespace ma
	{
		struct fraction
		{
			std::size_t _num;
			std::size_t _den;

			fraction(): _num(0), _den(1){}
			fraction(const fraction& orig) { *this = orig; }
			fraction(std::size_t num, std::size_t den) :
				_num(num),
				_den(den)
			{
				sanitize();
			}
			fraction(std::size_t num):
				_num(num),
				_den(1)
			{}

			fraction& operator=(const fraction& orig)
			{
				_num = orig._num;
				_den = orig._den;
				sanitize();
				return *this;
			}

			fraction& operator=(std::size_t num)
			{
				_num = num;
				_den = 1;
				return *this;
			}

			bool is_valid() const { return _den != 0; }
			void sanitize()
			{
				if (_den == 0)
				{
					_den = 1;
				}
			}

			template<typename Float>
			Float value() const
			{
				Float result = _num / _den;
				return result;
			}

			tvr::ma::fraction& operator+=(const tvr::ma::fraction& item)
			{
				_num += item._num;
				_den += item._den;
				return *this;
			}

			tvr::ma::fraction& operator-=(const tvr::ma::fraction& item)
			{
				_num -= item._num;
				_den -= item._den;
				return *this;
			}

			tvr::ma::fraction& operator/=(const tvr::ma::fraction& item)
			{
				_num /= item._num;
				_den /= item._den;
				return *this;
			}

			tvr::ma::fraction& operator*=(const tvr::ma::fraction& item)
			{
				_num *= item._num;
				_den *= item._den;
				return *this;
			}

			bool operator==(const tvr::ma::fraction& item)
			{
				bool result = false;
				return result;
			}

			std::size_t gcd() const
			{
				std::size_t result = 0;
				std::size_t a, b;
				a = _num;
				b = _den;
				while (a != b)
				{
					if (a > b)
						a = a - b;
					else
						b = b - a;
				}
				result = a;
				return result;
			}

			void simplify()
			{
				sanitize();
				std::size_t g = gcd();
				if (g != 0)
				{
					_num /= g;
					_den /= g;
				}
			}

			void musical_simplify()
			{
				// Octaves are equivalent
				simplify();
				while ((_num % 2) == 0)
				{
					_num /= 2;
				}
				while ((_den % 2) == 0)
				{
					_den /= 2;
				}
			}
		};
	}
}

tvr::ma::fraction operator+(const tvr::ma::fraction& left, const tvr::ma::fraction& right)
{
	tvr::ma::fraction result;
	result._num = left._num + right._num;
	result._den = left._den + right._den;
	return result;
}

tvr::ma::fraction operator-(const tvr::ma::fraction& left, const tvr::ma::fraction& right)
{
	tvr::ma::fraction result;
	result._num = left._num - right._num;
	result._den = left._den - right._den;
	return result;
}

tvr::ma::fraction operator/(const tvr::ma::fraction& left, const tvr::ma::fraction& right)
{
	tvr::ma::fraction result;
	result._num = left._num / right._num;
	result._den = left._den / right._den;
	return result;
}

tvr::ma::fraction operator*(const tvr::ma::fraction& left, const tvr::ma::fraction& right)
{
	tvr::ma::fraction result;
	result._num = left._num * right._num;
	result._den = left._den * right._den;
	return result;
}

#endif // FRACTION_HPP
