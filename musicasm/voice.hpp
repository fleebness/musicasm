#if !defined VOICE_HPP
#define VOICE_HPP

#include <functional>
#include <memory>
#include <vector>

#include "base_note.hpp"

namespace tvr
{
	namespace pa
	{
		struct voice
		{
			typedef std::vector<base_note> base_note_collection;
			typedef std::shared_ptr<_wave> wave_ptr;

			base_note_collection::size_type _current_note;
			PaTime _last_played;
			wave_ptr _tone;
			base_note_collection _notes;

			voice() :
				_last_played(0.0),
				_current_note(0)
			{}

			voice(const voice& orig)
			{
				*this = orig;
			}

			voice& operator=(const voice& orig)
			{
				if (this != &orig)
				{
					_current_note = orig._current_note;
					_last_played = orig._last_played;
					_tone = orig._tone;
					_notes = orig._notes;
				}
				return *this;
			}

			voice& operator=(const base_note_collection& notes)
			{
				_notes = notes;
				return *this;
			}

			void set_time(PaTime current_time)
			{
				if (_current_note < _notes.size())
				{
					if (_last_played + _notes[_current_note]._duration < current_time)
					{
						++_current_note;
						play_note(current_time);
					}
					else if (current_time == 0.0)
					{
						play_note(current_time);
					}
				}
			}

			bool is_dead() const
			{
				return !(_current_note < _notes.size());
			}

		private:
			void play_note(PaTime current_time)
			{
				if (_current_note < _notes.size())
				{
					if (_tone.get() != 0)
					{
						base_note note = _notes[_current_note];
						_tone->set_freq(note._freq);
						_tone->set_vol(note._vol);
						_last_played = current_time;
					}
				}
				else
				{
					// No more notes.  Silence!
					if (_tone.get() != 0)
					{
						_tone->set_vol(0.0);
					}
				}
			}
		};
	}
}

#endif // VOICE_HPP
