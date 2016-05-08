#if !defined BEAT_HPP
#define BEAT_HPP

#include <map>
#include <set>

#include "_basic_value.hpp"

namespace tvr
{
	namespace ma
	{

		/// Models tempo divisions for metered rythms.
		///
		/// Metered music relies upon a regular pulse at a particular tempo.  This
		/// class helps build rich, complex divisions of the pulses of the tempo
		/// by dividing a count of these pulses into divisions, which themselves
		/// may be fed into other instances of this class.
		///
		/// This class also models a regular pulse without divisions when the
		/// 'division' attribute is set to 0.
		///
		/// For example, to model a measure of 4/4 time where the second beat is
		/// a duplet, the third beat is a triplet, and the last beat is a triplet
		/// that has a duplet, (and the first beat is a quarter note), you can do
		/// this:
		///
		///     #include <beat.hpp>
		///     namespace tvr::ma;
		///     beat a(4);                  // represents 4 pulses, no divisions.
		///     beat duplet(1, 2);                // represents 1 pulse, 2 divisions, so a duplet.
		///     beat triplet(1, 3);               // represents 1 pulse, 3 divisions, so a triplet.
		///     beat altered_triplet(triplet);    // copy the triplet
		///     altered_triplet.set_division_to_beat(3, duplet);  // make the third pulse in the triplet a duplet
		///     a.set_division_to_beat(2, duplet);            // Set the 2nd beat to a duplet.
		///     a.set_division_to_beat(3, triplet);           // Set the 3rd beat to a triplet
		///     a.set_division_to_beat(4, altered_triplet);   // Set the 4th beat to our altered triplet.
		///
		/// You may tie a pulse to the next duration by calling set_tie:
		/// 
		///     triplet.set_tie(3);                    // Indicate the last note of the triplet is tied to
		///                                            // the next note.
		///     a.set_division_to_beat(3, triplet);    // Replace the 3rd note of `a` with our tied triplet,
		///                                            // so the last triplet is tied to the 4th beat's
		///                                            // first triplet.
		/// Alternatively, you could also have done this as:
		///
		///     a.set_tie(3);                          // Even though the third note of `a` is actually a
		///                                            // beat, the last note of the beat should be tied.
		class beat
		{
		private:
			typedef std::set<std::size_t> ordered_sizes;
			typedef std::map<std::size_t, beat> beat_locations_t;

		public:
			/// Constructor
			/// 
			/// \param beat_count the number of tempo pulses in which this beat lives.
			/// \param divisions the number of divisions to make to the total duration of tempo pulses.
			beat(std::size_t beat_count = 1, std::size_t divisions = 0) :
				_beat_count(beat_count),
				_divisions(divisions)
			{
				if (_beat_count == 0)
				{
					_beat_count = 1;
				}
			}

			/// Copy constructor
			beat(const beat& orig) { *this = orig; }

			~beat(){}

			/// Assignment operator
			beat& operator=(const beat& orig)
			{
				_beat_count = orig._beat_count;
				_divisions = orig._divisions;
				_ties = orig._ties;
				_subdivs = orig._subdivs;
				return *this;
			}

			/// Manually sets the number of divisions for this beat.
			void set_divisions(std::size_t divs)
			{
				_divisions = divs;
			}

			/// Acquires the current divisions for this beat.
			std::size_t get_divisions() const
			{
				return _divisions;
			}

			/// Acquires the count of pulses for this beat.
			std::size_t get_beat_count() const
			{
				return _beat_count;
			}

			/// Sub-divides a pulse/beat.
			/// \param div The specific division to be sub-divided
			/// \param b The sub-division for the division.
			void set_division_to_beat(std::size_t div, const beat& b)
			{
				_subdivs[div] = b;
			}
			
			/// Clears a pulse/beat's subdivision.
			/// \param div The division from which to remove the beat.
			void clear_division_from_beat(std::size_t div)
			{
				_subdivs.erase(div);
			}

			/// Indicates that the duration of the division of the beat is tied to the next duration.
			/// \param div The division to tie to the next duration.
			void set_tie(std::size_t div)
			{
				_ties.insert(div);
			}

			/// Removes a tie from the named division.
			/// \param div The division whose tie should be erased.
			void erase_tie(std::size_t div)
			{
				_ties.erase(div);
			}

			/// Adds either another division or another beat, depending upon context.
			beat& operator++()
			{
				if (_divisions > 0)
					++_divisions;
				else
					++_beat_count;
				return *this;
			}

			/// Adds either another division or another beat, depending upon context (postfix).
			beat operator++(int)
			{
				beat result = *this;
				if (_divisions > 0)
					++_divisions;
				else
					++_beat_count;
				return result;
			}

			/// Adds `count` to either division or beat count, depending upon context.
			/// \param count The amount to add to the division or beat count.
			beat operator+(std::size_t count)
			{
				beat result(*this);
				if (_divisions > 0)
					result._divisions += count;
				else
					result._beat_count += count;
				return result;
			}

			/// Subtracts `count` safely from either division or beat count, depending upon context.
			/// \param count The amount t oremove from the division or beat count.
			beat operator-(std::size_t count)
			{
				beat result(*this);
				if (_divisions > 0)
				{
					if (count < result._divisions)
					{
						result._divisions -= count;
					}
					else
					{
						result._divisions = 0;
					}
				}
				else
				{
					if (count+1 < result._beat_count)
					{
						result._beat_count -= count;
					}
					else
					{
						// We can't have a zero beat count.
						result._beat_count = 1;
					}
				}
				return result;
			}

			/// Adds durations to a collection.
			///
			/// This adds to a collection of durations with the durations implicated by this beat.
			/// \param col The collection to which to add durations.
			/// \param tempo Each pulse takes this much time.
			/// \param tie Set this to true to indicate that the last note already in the
			///            collection has its duration exended by the duration of the first
			///            note this beat generates.
			/// \result Returns true if the last note this beat created should be tied to the
			///         next note generated.
			template< typename Collection >
			bool get_durations(Collection& col, const duration& tempo, bool tie = false) const
			{
				bool result = tie;
				duration divs = tempo;
				std::size_t count = _beat_count;
				if (_divisions != 0)
				{
					// When _divisions != 0, we are dividing our total playtime
					// by that number.
					divs = tempo * _beat_count / _divisions;
					count = _divisions;
				}
				for (std::size_t i = 1; i <= count; ++i)
				{
					beat_locations_t::const_iterator found = _subdivs.find(i);
					if (found != _subdivs.end())
					{
						result = found->second.get_durations(col, divs, result);
						if (found->second._beat_count > 1)
						{
							// Need to increment 'i' past the count of beats.
							i += (found->second._beat_count - 1);
						}
					}
					else
					{
						if (result)
						{
							Collection::reverse_iterator iter = col.rbegin();
							*iter = *iter + divs;
						}
						else
						{
							col.insert(col.end(), divs);
						}
						ordered_sizes::const_iterator f_tie = _ties.find(i);
						if ( f_tie != _ties.end() )
						{
							result = true;
						}
						else
						{
							result = false;
						}
					}
				}
				return result;
			}
		private:
			std::size_t _beat_count;
			std::size_t _divisions;
			ordered_sizes _ties;
			beat_locations_t _subdivs;
		};
	}
}

#endif // BEAT_HPP
