#if !defined TEMPERAMENT_HPP
#define TEMPERAMENT_HPP

#include "frequency.hpp"

namespace tvr
{
	namespace ma
	{
		struct _temperament
		{

			virtual frequency get_freq(std::size_t number) = 0;
			virtual frequency get_ref_freq() const = 0;
			virtual std::size_t get_ref_num() const = 0;
			virtual std::size_t get_octave_division() const = 0;
		};
	}
}

#endif // TEMPERAMENT_HPP
