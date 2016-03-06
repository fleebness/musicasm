#if !defined ADSR_HPP
#define ADSR_HPP

#include <vector>

#include "base_note.hpp"

namespace tvr
{
	namespace pa
	{
		class adsr
		{
		public:
			typedef std::vector<base_note> notes_t;

			struct v_point
			{
				double _begin;
				double _duration;
				double _end;
				
				v_point():
					_begin(0.0),
					_duration(0.0),
					_end(0.0)
				{}

				v_point(const v_point& orig)
				{
					*this = orig;
				}

				v_point& operator=(const v_point& orig)
				{
					_begin = orig._begin;
					_duration = orig._duration;
					_end = orig._end;
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

			void operator()(notes_t& results, double freq, double vol, double dur) const
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
				if (_min_dur > 0)
				{
					// We won't even try if _min_dur == 0.
					// Let's calculate how much we'll actually use of each part.
					v_point attack = _attack;
					v_point decay = _decay;
					v_point sustain = _sustain;
					v_point release = _release;
					if (_min_dur > dur)
					{
						// We have to trim durations.
						double leftover = _min_dur - dur;
						sustain._duration = 0; // We don't have enough for minimum, ergo, can't do a sustain.
						decay._duration -= leftover;
						if (decay._duration < 0.0)
						{
							leftover = decay._duration * -1.0;
							decay._duration = 0.0;
						}
						if (leftover > 0)
						{
							attack._duration -= leftover;
							if (attack._duration < 0.0)
							{
								leftover = attack._duration * -1.0;
								attack._duration = 0;
								if (leftover > 0)
								{
									release._duration -= leftover;
									if (release._duration < 0.0)
									{
										// We should never, ever, get here.
										// If we do, we have a miscalculation somewhere.
										release._duration = 0;
									}
								}
							}
						}
					}
					else
					{
						double leftover = dur - _min_dur;
						sustain._duration = leftover;
					}

					// By now, we should have everything we need.
					double current_vol = 0.0;
					double count, amount;
					count = amount = 0.0;
					if (attack._duration > 0)
					{
						calculate_count_and_amount(count, amount, attack, _attack, current_vol);
						for (double i = 0.0; i < attack._duration; i += count)
						{
							current_vol += amount;
							results.push_back(base_note(freq, current_vol, count));
						}
					}

					if (decay._duration > 0)
					{
						calculate_count_and_amount(count, amount, decay, _decay, current_vol);
						for (double i = 0.0; i < decay._duration; i += count)
						{
							current_vol += amount;
							results.push_back(base_note(freq, current_vol, count));
						}
					}

					if (sustain._duration > 0)
					{
						calculate_count_and_amount(count, amount, sustain, _sustain, current_vol);
						for (double i = 0.0; i < sustain._duration; i += count)
						{
							current_vol += amount;
							results.push_back(base_note(freq, current_vol, count));
						}
					}

					if (release._duration > 0)
					{
						release._end = 0;
						calculate_count_and_amount(count, amount, release, release, current_vol);
						for (double i = 0.0; i < release._duration; i += count)
						{
							current_vol += amount;
							results.push_back(base_note(freq, current_vol, count));
						}
					}
				}
			}

		private:
			static void calculate_count_and_amount(double& count, double& amount, const v_point& item, const v_point& original, double current_vol)
			{
				count = 0;
				amount = 0;
				// The smaller the total duration, the larger the jump in volume may be.
				count = item._duration / 0.01;
				// Determine the amount between values.
				amount = ((original._end - current_vol) / count);
				count = item._duration / count;
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
			double _min_dur;
		};
	}
}

#endif // ADSR_HPP

