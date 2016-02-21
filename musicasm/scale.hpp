#if !defined SCALE_HPP
#define SCALE_HPP

#include "equal_temperament.hpp"

namespace tvr
{
	namespace pa
	{
		namespace western
		{
			const size_t lydian_mode[7] = {
				0, 2, 4, 6, 7, 9, 11
			};

			const size_t ionian_mode[7] = {
				0, 2, 4, 5, 7, 9, 11
			};

			const size_t mixolydian_mode[7] = {
				0, 2, 4, 5, 7, 9, 10
			};

			const size_t dorian_mode[7] = {
				0, 2, 3, 5, 7, 9, 10
			};

			const size_t aeolian_mode[7] = {
				0, 2, 3, 5, 7, 8, 10
			};

			const size_t phrygian_mode[7] = {
				0, 1, 3, 5, 7, 8, 10
			};

			const size_t locrian_mode[7] = {
				0, 1, 3, 5, 6, 8, 10
			};

			const size_t harmonic_minor_mode[7] = {
				0, 2, 3, 5, 7, 8, 11
			};

			class note
			{
			public:
				note(equal_temperament& tuning):_key(49), _tuning(tuning) {}
				note(const note& orig): _tuning(orig._tuning)
				{
					*this = orig;
				}
				note(std::size_t key, equal_temperament& tuning): _key(key), _tuning(tuning)
				{}
				~note(){}

				note& operator=(const note& orig)
				{
					_key = orig._key;
					return *this;
				}

				operator double() const
				{
					return _tuning.get_freq(_key);
				}
			private:
				equal_temperament& _tuning;
				std::size_t _key;
			};
			
			template< typename get_temperament >
			class scale
			{
			public:
				scale(std::size_t key, const size_t(&modal_scale)[7] = ionian_mode):
					_key(key),
					_mode(modal_scale),
					_temper(get_temperament::get_temper())
				{
				}

				scale(const scale& orig):
					_key(orig._key),
					_mode(orig._mode),
					_temper(get_temperament::get_temper())
				{
				}

				note get_note(int interval, int octave = 0, bool flat = false, bool sharp = false)
				{
					octave += interval / 7;
					if (interval < 0)
					{
						interval = 7 + interval;
						--octave;
					}
					if (interval > 6)
					{
						interval %= 7;
					}
					std::size_t mod = _key + (_temper.get_octave_division() * octave);
					mod += _mode[interval];
					if (sharp)
					{
						++mod;
					}
					if (flat)
					{
						--mod;
					}
					note result(mod, _temper);
					return result;
				}

				double get_freq(std::size_t interval, std::size_t octave, bool flat = false, bool sharp = false)
				{
					scale_note result = get_note(interval, octave, flat, sharp);
					return result;
				}

			private:
				equal_temperament _temper;
				const size_t(&_mode)[7];
				std::size_t _key;
			};

			typedef scale<a440_12_TET> concert_scale;
			typedef scale<a432_12_TET> scale_432;
		}
	}
}

#endif // SCALE_HPP
