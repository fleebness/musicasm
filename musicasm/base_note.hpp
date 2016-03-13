#if !defined BASE_NOTE_HPP
#define BASE_NOTE_HPP

#include "amplitude.hpp"
#include "duration.hpp"
#include "frequency.hpp"
#include "port_audio.hpp"

namespace tvr
{
	namespace ma
	{
		struct base_note
		{
			frequency _freq;
			amplitude _vol;
			duration _dur;
			PaTime _last_played;

			base_note() :
				_freq(0.0),
				_vol(0.0),
				_dur(0.0),
				_last_played(0.0)
			{}

			base_note(frequency freq, amplitude vol, duration dur) :
				_freq(freq),
				_vol(vol),
				_dur(dur)
			{}

			base_note(amplitude vol, frequency freq, duration dur) :
				_freq(freq),
				_vol(vol),
				_dur(dur)
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
				_dur = orig._dur;
				_last_played = orig._last_played;
				return *this;
			}
		};
	}
}


#endif // BASE_NOTE_HPP