#if !defined SHEET_HPP
#define SHEET_HPP

#include <map>
#include <memory>
#include <ratio>
#include <vector>

#include "_base_obs.hpp"
#include "_basic_value.hpp"
#include "adsr.hpp"
#include "base_note.hpp"
#include "voice.hpp"

namespace tvr
{
	namespace ma
	{
		/// Interface for the tempo data type, allowing for different kinds of tempos.
		struct _tempo
		{
			virtual std::intmax_t num() const = 0;
			virtual std::intmax_t den() const = 0;
		};

		/// Provides a way to express different tempos using std::ratio.
		template <typename Ratio>
		struct tempo : public _tempo
		{
			virtual std::intmax_t num() const { return Ratio::num; }
			virtual std::intmax_t den() const { return Ratio::den; }
		};

		/// Provides a system for handling metered rhythms in music.
		class sheet
		{
		public:
			typedef std::shared_ptr< sheet > ptr;
			typedef std::vector< _dur::ptr > durations_t;
			typedef std::vector< _freq::ptr > frequencies_t;
			typedef std::vector< ptr > sheets_t;
			typedef std::map< std::size_t, _dur::ptr > duration_map_t;
			typedef std::map< std::size_t, voice::ptr > voice_map_t;
			typedef std::map< std::size_t, adsr::ptr > adsr_map_t;

		public:
			/// Default constructor.
			sheet():
				_beat_dur(0.5),
				_max(0.0)
			{}

			/// Constructor with a tempo.
			sheet(const _tempo& tmpo)
			{
				(*this) << tmpo;
			}

			sheet& operator<<(const _tempo& tmpo)
			{
				// This probably looks weird.
				// We expect this ratio to represent a count of beats to a quantity of time.
				// So, for example, 120 beats a minute is 120 beats every 60 seconds.
				// To figure out how long a beat lasts, you can't say 120/60, since that
				// gives you 2 (the number of beats a second)... instead, you need 60/120,
				// which gives 0.5 (how long the beat must last in seconds).
				_beat_dur = static_cast<double>(tmpo.den()) / static_cast<double>(tmpo.num());
				calculate_durations();
				return *this;
			}

			/// Destructor
			~sheet() {}

		private:
			void set_duration(duration dur)
			{
				// Everything we play must fit within this duration.
				_max = dur;
				calculate_durations();
			}

			void calculate_durations()
			{
				// If we have a _max, ensure everything we play fits within it.
				// If we do not have a _max, we need to go through everything and
				// calculate their durations according to _beat_dur.
			}

		private:
			duration _beat_dur;
			duration _max;
		};
	}
}

#endif // SHEET_HPP

