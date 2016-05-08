#if !defined ADSR_HPP
#define ADSR_HPP

#include <memory>
#include <vector>

#include "_basic_value.hpp"
#include "voice.hpp"

namespace tvr
{
	namespace ma
	{
		class adsr
		{
		public:
			typedef std::vector<base_note> notes_t;
			typedef std::shared_ptr<adsr> ptr;

			struct v_point
			{
				duration _duration;
				amplitude _target;
				
				v_point():
					_duration(0.0),
					_target(0.0)
				{}

				v_point(const v_point& orig)
				{
					*this = orig;
				}

				v_point& operator=(const v_point& orig)
				{
					_duration = orig._duration;
					_target = orig._target;
					return *this;
				}
			};

		public:
			adsr(): _min_dur(0.0)
			{}

			adsr(const v_point& attack, const v_point& decay, const v_point &sustain, const v_point& release):
				_attack(attack),
				_decay(decay),
				_sustain(sustain),
				_release(release)
			{
				calculate_min_dur();
			}

			adsr(const adsr& orig)
			{
				*this = orig;
			}

			~adsr()
			{}

			adsr& operator=(const adsr& orig)
			{
				_attack = orig._attack;
				_decay = orig._decay;
				_sustain = orig._sustain;
				_release = orig._release;
				_min_dur = orig._min_dur;
				return *this;
			}

			void set_attack(const v_point& attack)
			{
				_attack = attack;
				calculate_min_dur();
			}

			void set_decay(const v_point& decay)
			{
				_decay = decay;
				calculate_min_dur();
			}

			void set_sustain(const v_point& sustain)
			{
				_sustain = sustain;
			}

			void set_release(const v_point& release)
			{
				_release = release;
				calculate_min_dur();
			}

			void operator()(voice& result, const base_note& note) const
			{
				operator()(result, note._vol, note._dur, note._freq);
			}

			void operator()(voice& result, amplitude vol, duration dur, frequency freq) const
			{
				// Fun stuff!
				// Add base_notes to results for:
				// attack (ignore begin, and assume vol = 0, ramp up to end's vol, taking duration's time to do it).
				// decay (ignore start and assume vol = attack's end vol, change vol end, taking duration's time to do it).
				// sustain (ignore start and assume decay's end, change vol to end, taking a modification of dur (not duration) time to do it).
				// release (ignore start and assume sustain's end, change vol to 0, taking duration's time to do it).

				// The trick?  All of this happens within 'dur', where anything left over is in 'sustain'.
				// If we don't get enough time for all v_points, take time first from sustain, then decay, then attack, then release (in that order).
				// Take time from the end of a v_point when needed.
				if (_min_dur > duration::zero())
				{
					// We won't even try if _min_dur == 0.
					// Let's calculate how much we'll actually use of each part.
					v_point attack = _attack;
					v_point decay = _decay;
					v_point sustain = _sustain;
					v_point release = _release;
					calculate_env(attack, decay, sustain, release, _min_dur, dur);
					apply_env(result, attack, decay, sustain, release, vol, freq);
				}
			}

			template<typename Coll>
			void get_durations(Coll& result, amplitude vol, duration dur) const
			{
				v_point attack = _attack;
				v_point decay = _decay;
				v_point sustain = _sustain;
				v_point release = _release;
				calculate_env(attack, decay, sustain, release, _min_dur, dur);
				get_env_durations(result, attack, decay, sustain, release, vol);
			}

		private:
			static void calculate_env( v_point& attack,
				v_point& decay,
				v_point& sustain,
				v_point& release,
				duration minimum,
				duration dur)
			{
				if (minimum > dur)
				{
					// We have to trim durations.
					duration leftover = minimum - dur;
					sustain._duration = 0; // We don't have enough for minimum, ergo, can't do a sustain.
					decay._duration -= leftover;
					if (decay._duration < duration::zero())
					{
						leftover = decay._duration * duration::negative();
						decay._duration = duration::zero();
					}
					if (leftover > duration::zero())
					{
						attack._duration -= leftover;
						if (attack._duration < duration::zero())
						{
							leftover = attack._duration * duration::negative();
							attack._duration = duration::zero();
							if (leftover > duration::zero())
							{
								release._duration -= leftover;
								if (release._duration < duration::zero())
								{
									// We should never, ever, get here.
									// If we do, we have a miscalculation somewhere.
									release._duration = duration::zero();
								}
							}
						}
					}
				}
				else
				{
					duration leftover = dur - minimum;
					sustain._duration = leftover;
				}
				release._target._value = 0;
			}

			template<typename Coll>
			void get_env_durations(Coll& result,
				const v_point& attack,
				const v_point& decay,
				const v_point& sustain,
				const v_point& release,
				amplitude vol) const
			{
				amplitude current_vol = 0.0;
				double count, amount;
				count = amount = 0.0;
				if (attack._duration > duration::zero())
				{
					calculate_count_and_amount(count, amount, attack, _attack, current_vol);
					apply_point_to_coll(result, attack, _attack, count, amount, vol, current_vol);
				}

				if (decay._duration > duration::zero())
				{
					calculate_count_and_amount(count, amount, decay, _decay, current_vol);
					apply_point_to_coll(result, decay, _decay, count, amount, vol, current_vol);
				}

				if (sustain._duration > duration::zero())
				{
					calculate_count_and_amount(count, amount, sustain, _sustain, current_vol);
					apply_point_to_coll(result, sustain, _sustain, count, amount, vol, current_vol);
				}

				if (release._duration > duration::zero())
				{
					calculate_count_and_amount(count, amount, release, release, current_vol);
					apply_point_to_coll(result, release, release, count, amount, vol, current_vol);
				}
			}

			void apply_env(voice& result,
				const v_point& attack,
				const v_point& decay,
				const v_point& sustain,
				const v_point& release,
				amplitude vol,
				frequency freq) const
			{
				amplitude current_vol = 0.0;
				double count, amount;
				count = amount = 0.0;
				if (attack._duration > duration::zero())
				{
					calculate_count_and_amount(count, amount, attack, _attack, current_vol);
					apply_point(result, attack, _attack, count, amount, vol, current_vol, freq);
				}

				if (decay._duration > duration::zero())
				{
					calculate_count_and_amount(count, amount, decay, _decay, current_vol);
					apply_point(result, decay, _decay, count, amount, vol, current_vol, freq);
				}

				if (sustain._duration > duration::zero())
				{
					calculate_count_and_amount(count, amount, sustain, _sustain, current_vol);
					apply_point(result, sustain, _sustain, count, amount, vol, current_vol, freq);
				}

				if (release._duration > duration::zero())
				{
					calculate_count_and_amount(count, amount, release, release, current_vol);
					apply_point(result, release, release, count, amount, vol, current_vol, freq);
				}
			}

			static void apply_point(voice& result,
				const v_point& calculated,
				const v_point& original,
				double count,
				double amount,
				amplitude vol,
				amplitude& current_vol,
				frequency freq)
			{
				for (duration i = 0.0; i < calculated._duration; i += count)
				{
					current_vol._value += amount;
					result.add_note(base_note(freq, current_vol._value * vol._value, count));
				}
			}

			static void apply_point(voice& result,
				const v_point& calculated,
				const v_point& original,
				double count,
				double amount,
				amplitude vol,
				amplitude& current_vol)
			{
				for (duration i = 0.0; i < calculated._duration; i += count)
				{
					current_vol._value += amount;
					result.add_vol(current_vol._value * vol._value, count);
				}
			}

			template<typename Coll>
			static void apply_point_to_coll(Coll& result,
				const v_point& calculated,
				const v_point& original,
				double count,
				double amount,
				amplitude vol,
				amplitude& current_vol)
			{
				// doubles are super-terrible at this.  So, let's use ints!
				unsigned int max = static_cast<unsigned int>(calculated._duration._value * 100);
				unsigned int iter = static_cast<unsigned int>(count * 100);
				for (unsigned int i = 0; i < max; i += iter)
				{
					current_vol._value += amount;
					amplitude_dur info;
					info._val = current_vol._value;
					info._length = count;
					if (info._val._value < 0.0)
					{
						info._val._value = 0.0;
					}
					result.insert(result.end(), info);
				}
			}

			static void calculate_count_and_amount(double& count,
				double& amount,
				const v_point& item,
				const v_point& original,
				amplitude& current_vol)
			{
				count = 0;
				amount = 0;
				// The smaller the total duration, the larger the jump in volume may be.
				count = item._duration._value / 0.01;
				// Determine the amount between values.
				amount = ((original._target._value - current_vol._value) / count);
				count = item._duration._value / count;
			}

			void calculate_min_dur()
			{
				_min_dur = _attack._duration;
				_min_dur += _decay._duration;
				_min_dur += _release._duration;
			}

		private:
			v_point _attack;
			v_point _decay;
			v_point _sustain;
			v_point _release;
			duration _min_dur;
		};
	}
}

#endif // ADSR_HPP

