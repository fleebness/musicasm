#if !defined SCALE_HPP
#define SCALE_HPP

#include "equal_temperament.hpp"

namespace tvr
{
	namespace ma
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
				note(_temperament::_ptr tuning):_tuning(tuning), _key(49) {}
				note(const note& orig): _tuning(orig._tuning)
				{
					*this = orig;
				}
				note(std::size_t key, _temperament::_ptr tuning): _tuning(tuning), _key(key)
				{}
				~note(){}

				note& operator=(const note& orig)
				{
					_key = orig._key;
					return *this;
				}

				operator frequency() const
				{
					return _tuning->get_freq(_key);
				}
			private:
				_temperament::_ptr _tuning;
				std::size_t _key;
			};
			
			template< typename get_temperament, std::size_t diatonic_count >
			class scale
			{
			public:
				scale(std::size_t key, const size_t(&modal_scale)[diatonic_count] = ionian_mode):
					_temper(get_temperament::get_temper()),
					_mode(modal_scale),
					_key(key)
				{
				}

				scale(const scale& orig):
					_temper(get_temperament::get_temper()),
					_mode(orig._mode),
					_key(orig._key)
				{
				}

				note get_note(int interval, int octave = 0, bool flat = false, bool sharp = false)
				{
					octave += interval / diatonic_count;
					if (interval < 0)
					{
						interval = diatonic_count + interval;
						--octave;
					}
					if (interval > diatonic_count - 1)
					{
						interval %= diatonic_count;
					}
					std::size_t mod = _key + (_temper->get_octave_division() * octave);
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

				frequency get_freq(int interval, int octave, bool flat = false, bool sharp = false)
				{
					frequency result = get_note(interval, octave, flat, sharp);
					return result;
				}

			private:
				_temperament::_ptr _temper;
				const size_t(&_mode)[diatonic_count];
				std::size_t _key;
			};

			typedef scale<a440_12_TET_p, 7> concert_scale;
			typedef scale<a432_12_TET_p, 7> scale_432;
		}
	}
}

#endif // SCALE_HPP
