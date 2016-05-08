#if !defined JUST_TEMPERAMENT_HPP
#define JUST_TEMPERAMENT_HPP

#include <array>
#include <cmath>
#include <map>
#include <vector>

#include "_basic_value.hpp"
#include "fraction.hpp"
#include "temperament.hpp"
#include "wave.hpp"

namespace tvr
{
	namespace ma
	{
		const std::array<std::size_t, 22> pythagorean_tuning = {
			256, 243, // Db
			9, 8,     // D
			32, 27,   // Eb
			81, 64,   // E
			4, 3,     // F
			729, 512, // F#... this can also be Gb as 1024, 729
			3, 2,     // G
			128, 81,  // Ab
			27, 16,   // A
			16, 9,    // Bb
			243, 128  // B
		};

		const std::array<std::size_t, 22> five_limit_tuning = {
			16, 15, // Db
			9, 8,   // D
			6, 5,   // Eb
			5, 4,   // E
			4, 3,   // F
			45, 32, // F#
			3, 2,   // G
			8, 5,   // Ab
			5, 3,   // A
			9, 5,   // Bb
			15, 8   // B
		};

		class just_temperament :public _temperament
		{
		public:
			typedef std::map<std::size_t, frequency> frequency_map;
			typedef std::vector<fraction> fraction_coll;

			just_temperament(const frequency_map& scale, frequency ref, std::size_t ref_num):
				_scale(scale),
				_ref(ref),
				_ref_num(ref_num)
			{}

			template< typename Array >
			just_temperament(const Array& scale, frequency ref, std::size_t ref_num) :
				_ref(ref),
				_ref_num(ref_num)
			{
				std::size_t count = 0;
				_ratios.resize(scale.size() / 2);
				for (std::size_t i = 0; i < scale.size(); i += 2)
				{
					if (i + 1 < scale.size())
					{
						_ratios[count++] = fraction(scale.at(i), scale.at(i + 1));
					}
				}
			}

			just_temperament(const just_temperament& orig)
			{
				*this = orig;
			}

			just_temperament& operator=(const just_temperament& orig)
			{
				if (&orig != this)
				{
					_scale = orig._scale;
					_ref = orig._ref;
					_ref_num = orig._ref_num;
				}
				return *this;
			}

			virtual std::size_t get_octave_division() const
			{
				return _ratios.size() + 1;
			}
			virtual frequency get_freq(std::size_t number)
			{
				frequency result = 0.0;
				if (static_cast<double>(number) == _ref_num)
				{
					result = _ref;
				}
				else if (_scale.find(number) == _scale.end())
				{
					// Now, calculate this one.
					result = _get_freq(number);
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
				return _ref;
			}
			virtual std::size_t get_ref_num() const
			{
				return _ref_num;
			}

			void set_scale(const frequency_map& ratios)
			{
				_scale = ratios;
			}
		private:
			frequency _get_freq(std::size_t number) const
			{
				frequency result = 0.0;
				// Need to determine the octave, and the
				// division of the octave.
				int octave = static_cast<int>((static_cast<long long>(number) - static_cast<long long>(_ref_num)) / static_cast<int>(get_octave_division()));
				std::size_t division = std::abs(static_cast<long long>(number) - static_cast<long long>(_ref_num)) % get_octave_division();
				if (division == 0)
				{
					if (octave >= 0)
					{
						result = _ref._value * (2 * octave);
					}
					else
					{
						result = _ref._value / (2 * std::abs(octave));
					}
				}
				else
				{
					if (division - 1 < _ratios.size())
					{
						fraction f = _ratios[division - 1];
						result = (_ref * f._num) / f._den;
						if (octave > 0)
						{
							result = result * (2 * octave);
						}
						else if (octave < 0)
						{
							result = result / (2 * std::abs(octave));
						}
					}
					else
					{
						// Not sure what to do... don't really want to throw an
						// exception, but the caller didn't give us a full scale.
						// So, we'll give silence.
						result = 0.0;
					}
				}
				return result;
			}

		private:
			frequency_map _scale;
			fraction_coll _ratios;
			frequency _ref;
			std::size_t _ref_num;
		};

		template<std::size_t ref, typename std::size_t ref_num>
		struct get_pythagorean_temperment : public _get_temperament
		{
			get_pythagorean_temperment()
			{}

			virtual _temperament_ptr operator()()
			{
				return get_temper();
			}

			static _temperament_ptr get_temper()
			{
				_temperament_ptr result;
				result.reset(new just_temperament(pythagorean_tuning, ref, ref_num));
				return result;
			}
		private:
		};

		template<std::size_t ref, std::size_t ref_num>
		struct get_five_limit_temperment : public _get_temperament
		{
			get_five_limit_temperment()
			{}

			virtual _temperament_ptr operator()()
			{
				return get_temper();
			}

			static _temperament_ptr get_temper()
			{
				_temperament_ptr result;
				result.reset(new just_temperament(five_limit_tuning, ref, ref_num));
				return result;
			}
		private:
		};

		struct get_just_temperament_around_freq : public _get_temperament
		{
			template<typename RatioArray>
			get_just_temperament_around_freq(const RatioArray& ratios, frequency ref, std::size_t ref_num):
				_ref(ref),
				_ref_num(ref_num)
			{
				_temperament.reset(new just_temperament(ratios, _ref, _ref_num));
			}

			virtual _temperament_ptr operator()()
			{
				return _temperament;
			}

			static get_just_temperament_around_freq five_limit_factory(frequency ref, std::size_t ref_num)
			{
				return get_just_temperament_around_freq(five_limit_tuning, ref, ref_num);
			}

			static get_just_temperament_around_freq pythagorean_factory(frequency ref, std::size_t ref_num)
			{
				return get_just_temperament_around_freq(pythagorean_tuning, ref, ref_num);
			}

		private:
			frequency _ref;
			std::size_t _ref_num;
			_temperament_ptr _temperament;
		};

		typedef get_pythagorean_temperment<440, 49> pyth_a440_49_t;
		typedef get_five_limit_temperment<440, 49> five_lim_a440_49_t;
	}
}

#endif // JUST_TEMPERAMENT_HPP
