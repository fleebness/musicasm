#if !defined VOICE_HPP
#define VOICE_HPP

#include <functional>
#include <memory>
#include <vector>

#include "amplitude.hpp"
#include "base_note.hpp"
#include "frequency.hpp"
#include "wave.hpp"

namespace tvr
{
	namespace ma
	{
		/**
		 * A sequence of frequencies and amplitudes that should play, one after another, using a particular tone.
		 **/
		struct voice
		{
			/// A structure to represent either amplitude or frequency to use for a length of time.
			template< typename Elem >
			struct music_elem
			{
				Elem _value;
				double _duration;
				PaTime _last_played;
			};
			/// The type of music element for frequencies.
			typedef music_elem<frequency> frequency_elem;
			/// The type of music element for amplitudes
			typedef music_elem<amplitude> amplitude_elem;
			/// The type of collection for frequencies.
			typedef std::vector<frequency_elem> frequency_collection;
			/// The type of collection for amplitudes.
			typedef std::vector<amplitude_elem> amplitude_collection;
			/// A convenience type for pointers to _waves.
			typedef std::shared_ptr<_wave> wave_ptr;

			/// The timbre of the note.
			wave_ptr _tone;
			/// The collection of amplitudes to apply in time.
			amplitude_collection _amplitudes;
			/// The current amplitude playing.
			amplitude_collection::size_type _current_amp;
			/// The time the current amp started playing.
			PaTime _lp_amp;
			/// The collection of frequencies to apply in time.
			frequency_collection _frequencies;
			/// The current frequency playing.
			frequency_collection::size_type _current_freq;
			/// The time the current freq started playing.
			PaTime _lp_freq;
			
			/// Default constructor.
			voice() :
				_current_amp(0),
				_lp_amp(0),
				_current_freq(0),
				_lp_freq(0)
			{}

			/// Copy constructor
			voice(const voice& orig)
			{
				*this = orig;
			}

			/// Copy assignment operator.
			voice& operator=(const voice& orig)
			{
				if (this != &orig)
				{
					_tone = orig._tone;
					_amplitudes = orig._amplitudes;
					_current_amp = orig._current_amp;
					_lp_amp = orig._lp_amp;
					_frequencies = orig._frequencies;
					_current_freq = orig._current_freq;
					_lp_freq = orig._lp_freq;
				}
				return *this;
			}

			/// Sets how long into the current play time has run.
			void set_time(PaTime current_time)
			{
				if (current_time == 0.0)
				{
					change_amp(current_time);
					change_freq(current_time);
				}
				else
				{
					if (_current_amp < _amplitudes.size())
					{
						// If time has advanced beyond when we started this
						// amplitude plus its duration, we need to go to the
						// next amplitude in our list.
						if (_lp_amp + _amplitudes[_current_amp]._duration < current_time)
						{
							++_current_amp;
							change_amp(current_time);
						}
					}
					if (_current_freq < _frequencies.size())
					{
						// If time has advanced beyond when we started this
						// frequency plus its duration, we need to go to the
						// next frequency in our list.
						if (_lp_freq + _frequencies[_current_freq]._duration < current_time)
						{
							++_current_freq;
							change_freq(current_time);
						}
					}
				}
			}

			/// Returns true when we have run out of information.
			bool is_dead() const
			{
				return !(_current_amp < _amplitudes.size()) || !(_current_freq < _frequencies.size());
			}

			/// Adds a new volume value to the list of amplitudes.
			void add_vol(const amplitude& value, double duration)
			{
				if (_amplitudes.size() > 0 && _amplitudes.rbegin()->_value == value)
				{
					// Just add to the last duration.
					_amplitudes.rbegin()->_duration += duration;
				}
				else
				{
					amplitude_elem elem;
					elem._value = value;
					elem._duration = duration;
					elem._last_played = 0.0;
					_amplitudes.push_back(elem);
				}
			}

			/// Adds a new volume value to the list of amplitudes, adding that duration to the last frequency (extending it).
			void add_vol_and_sync(const amplitude& value, double duration)
			{
				add_vol(value, duration);
				extend_last_freq(duration);
			}

			/// Adds a new pitch value to the list of frequencies.
			void add_freq(const frequency& value, double duration)
			{
				if (_frequencies.size() > 0 && _frequencies.rbegin()->_value == value)
				{
					// Just add to the last duration.
					_frequencies.rbegin()->_duration += duration;
				}
				else
				{
					frequency_elem elem;
					elem._value = value;
					elem._duration = duration;
					elem._last_played = 0.0;
					_frequencies.push_back(elem);
				}
			}

			/// Adds a new pitch value to the list of frequencies, adding that duration to the last amplitude (extending it).
			void add_freq_and_sync(const frequency& value, double duration)
			{
				add_freq(value, duration);
				extend_last_vol(duration);
			}

			/// Adds both the volume and pitch to the lists from a base_note.
			void add_note(const base_note& note)
			{
				add_vol(note._vol, note._duration);
				add_freq(note._freq, note._duration);
			}

			/// Adds duration to the last amplitude value.
			void extend_last_vol(double duration)
			{
				if (_amplitudes.size() > 0)
				{
					_amplitudes.rbegin()->_duration += duration;
				}
			}

			/// Adds duration to the last frequency value.
			void extend_last_freq(double duration)
			{
				if (_frequencies.size() > 0)
				{
					_frequencies.rbegin()->_duration += duration;
				}
			}

		private:

			void change_amp(PaTime current_time)
			{
				if (_current_amp < _amplitudes.size())
				{
					if (_tone.get() != 0)
					{
						_tone->set_vol(_amplitudes[_current_amp]._value);
						_lp_amp = current_time;
					}
				}
				else
				{
					if (_tone.get() != 0)
					{
						_tone->set_vol(0.0);
					}
				}
			}

			void change_freq(PaTime current_time)
			{
				if (_current_freq < _frequencies.size())
				{
					if (_tone.get() != 0)
					{
						_tone->set_freq(_frequencies[_current_freq]._value);
						_lp_freq = current_time;
					}
				}
				else
				{
					if (_tone.get() != 0)
					{
						_tone->set_freq(0.0);
					}
				}
			}
		};
	}
}

#endif // VOICE_HPP
