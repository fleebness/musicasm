#if !defined EQUAL_TEMPERAMENT_HPP
#define EQUAL_TEMPERAMENT_HPP


#include <cmath>
#include <map>

#include "_basic_value.hpp"
#include "wave.hpp"
#include "temperament.hpp"

namespace tvr
{
	namespace ma
	{
		class equal_temperament :public _temperament
		{
		public:
			typedef std::map<std::size_t, frequency> frequency_map;
		public:
			equal_temperament(frequency ref_freq, std::size_t ref_num, std::size_t division) :
				_ref_freq(ref_freq),
				_ref_num(ref_num),
				_division(division)
			{}
			equal_temperament(const equal_temperament& orig) :
				_ref_freq(orig._ref_freq),
				_ref_num(orig._ref_num),
				_division(orig._division)
			{
				*this = orig;
			}
			virtual ~equal_temperament() {}

			equal_temperament& operator=(const equal_temperament& orig)
			{
				if (orig._ref_freq == _ref_freq
					&& orig._ref_num == _ref_num
					&& orig._division == _division
					)
				{
					_scale = orig._scale;
				}
				return *this;
			}

			virtual frequency get_freq(std::size_t number)
			{
				frequency result = 0.0;
				if (static_cast<double>(number) == _ref_num)
				{
					result = _ref_freq;
				}
				else if (_scale.find(number) == _scale.end())
				{
					// Now, calculate this one.
					result = _get_freq(static_cast<double>(number));
					_scale[number] = result;
				}
				else
				{
					result = _scale[number];
				}
				return result;
			}
			virtual frequency get_ref_freq() const
			{
				return _ref_freq;
			}
			virtual std::size_t get_ref_num() const
			{
				return _ref_num;
			}
			virtual std::size_t get_octave_division() const
			{
				return _division;
			}
		private:
			equal_temperament();
		private:
			frequency _get_freq(frequency number)
			{
				frequency result = 0.0;
				result = std::exp2((number._value - static_cast<double>(_ref_num)) / static_cast<double>(_division)) * _ref_freq._value;
				return result;
			}

		private:
			const frequency _ref_freq;
			const std::size_t _ref_num;
			const std::size_t _division;
			frequency_map _scale;
		};

		template<std::size_t freq, std::size_t num, std::size_t division>
		struct get_equal_temperament:public _get_temperament
		{
			virtual _temperament_ptr operator()()
			{
				return get_temper();
			}

			static _temperament_ptr get_temper()
			{
				_temperament_ptr result;
				result.reset(new equal_temperament(freq, num, division));
				return result;
			}
		};

		typedef get_equal_temperament<440, 49, 12> a440_12_TET_p;
		typedef get_equal_temperament<432, 49, 12> a432_12_TET_p;
	}
}

#endif // EQUAL_TEMPERAMENT_HPP

