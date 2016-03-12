#if !defined BASE_NOTE_HPP
#define BASE_NOTE_HPP

#include "port_audio.hpp"

namespace tvr
{
	namespace ma
	{
		struct base_note
		{
			frequency _freq;
			amplitude _vol;
			double _duration;
			PaTime _last_played;

			base_note() :
				_freq(0.0),
				_vol(0.0),
				_duration(0.0),
				_last_played(0.0)
			{}

			base_note(frequency freq, amplitude vol, double duration) :
				_freq(freq),
				_vol(vol),
				_duration(duration)
			{}

			base_note(amplitude vol, frequency freq, double duration) :
				_freq(freq),
				_vol(vol),
				_duration(duration)
			{}

			base_note(const base_note& orig)
			{
				*this = orig;
			}

			~base_note() {}

			base_note& operator=(const base_note& orig)
			{
				_freq = orig._freq;
				_vol = orig._vol;
				_duration = orig._duration;
				_last_played = orig._last_played;
				return *this;
			}
		};
	}
}


#endif // BASE_NOTE_HPP