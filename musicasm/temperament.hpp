#if !defined TEMPERAMENT_HPP
#define TEMPERAMENT_HPP
#include <memory>
#include "_basic_value.hpp"

namespace tvr
{
	namespace ma
	{
		struct _temperament
		{
			typedef std::shared_ptr<_temperament> _ptr;
			virtual frequency get_freq(std::size_t number) = 0;
			virtual frequency get_ref_freq() const = 0;
			virtual std::size_t get_ref_num() const = 0;
			virtual std::size_t get_octave_division() const = 0;
		};

		struct _get_temperament
		{
			typedef _temperament::_ptr _temperament_ptr;
			virtual _temperament_ptr operator()() = 0;
		};
	}
}

#endif // TEMPERAMENT_HPP
