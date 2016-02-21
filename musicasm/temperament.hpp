#if !defined TEMPERAMENT_HPP
#define TEMPERAMENT_HPP

namespace tvr
{
	namespace pa
	{
		struct _temperament
		{

			virtual double get_freq(std::size_t number) = 0;
			virtual double get_ref_freq() const = 0;
			virtual std::size_t get_ref_num() const = 0;
			virtual std::size_t get_octave_division() const = 0;
		};
	}
}

#endif // TEMPERAMENT_HPP
