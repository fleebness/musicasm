#if !defined WAVE_HPP
#define WAVE_HPP

#include <cmath>
#include <functional>

#include "_basic_value.hpp"

namespace tvr
{
	namespace ma
	{
		const double _pi = std::acos(-1);

		/// Provides SINE wave values.
		struct sine_value
		{
			double operator()(double i, double table_size)
			{
				double result = 0.0;
				result = std::sin(i / table_size * _pi * 2.0);
				return result;
			}
		};

		/// Provides a sawtooth wave.
		struct sawtooth_value
		{
			double operator()(double i, double table_size)
			{
				double result = 0.0;
				double half = table_size / 2;
				if ( i < half)
				{
					result = ((i * 2) / half) - 1;
				}
				else
				{
					double test = i - half;
					test = half - test;
					result = ((test * 2) / half) - 1;
				}
				return result;
			}
		};

		struct pulse_value
		{
			double operator()(double i, double table_size)
			{
				double result = 0.0;
				result = std::sin(i / table_size * _pi * 2.0);
				if (result > 0.0)
				{
					result = 1.0;
				}
				else
				{
					result = -1.0;
				}
				return result;
			}
		};

		class _wave
		{
		public:
			virtual void set_sample_rate(double sample_rate) = 0;
			virtual void set_freq(frequency freq) = 0;
			virtual void set_vol(amplitude vol) = 0;
			virtual void set_phase(double phase) = 0;
			virtual float get_next_value() = 0;
		};

		template<std::size_t table_size, typename fn_t>
		class wave : public _wave
		{
		private:
			typedef std::vector<float> float_collection;
			typedef std::vector<double> double_collection;
		public:
			wave() :
				_freq(440.0),
				_next_table_amount(0),
				_next_table_item(0),
				_phase(0.0),
				_sample_rate(44100.0),
				_vol(1.0)
			{
				if (get_table().empty())
				{
					get_table().resize(table_size);
					fill_table();
				}
				_set_freq();
			}

			wave(const wave& orig)
			{
				if (this != &orig)
				{
					*this = orig;
				}
			}

			virtual ~wave(){}

			wave& operator=(const wave& orig)
			{
				_freq = orig._freq;
				_sample_rate = orig._sample_rate;
				_next_table_amount = orig._next_table_amount;
				_next_table_item = orig._next_table_item;
				_vol = orig._vol;
				_phase = orig._phase;
				return *this;
			}

			virtual void set_sample_rate(double sample_rate)
			{
				_sample_rate = sample_rate;
				_set_freq();
			}

			virtual void set_freq(frequency freq)
			{
				_freq = freq;
				_set_freq();
			}

			virtual void set_vol(amplitude vol)
			{
				_vol = limit(vol._value);
			}

			virtual void set_phase(double phase)
			{
				_phase = limit(phase);
			}

			virtual float get_next_value()
			{
				std::size_t pick = static_cast<std::size_t>(_next_table_item + (_phase * get_table().size())) % get_table().size();
				float result = get_table()[pick] * static_cast<float>(_vol._value);
				_next_table_item += _next_table_amount;
				pick = static_cast<std::size_t>(_next_table_item) % get_table().size();
				return result;
			}
		private:
			void _set_freq()
			{
				_next_table_amount = (get_table().size() * _freq._value) / _sample_rate;
			}
			static void fill_table()
			{
				fn_t fn;
				for (std::size_t i = 0; i < get_table().size(); ++i)
				{
					get_table()[i] = (float)fn(static_cast<double>(i), static_cast<double>(get_table().size()));
				}
			}
			static float_collection& get_table()
			{
				static float_collection table;
				return table;
			}
			double limit(double in)
			{
				double result = in;
				if (in < 0.0)
				{
					result = 0;
				}
				else if (in > 1.0)
				{
					result = 1.0;
				}
				return result;
			}
		private:
			frequency _freq;
			double _next_table_amount;
			double _next_table_item;
			double _phase;
			double _sample_rate;
			amplitude _vol;
		};

		typedef wave<500, sine_value> sine_wave;
		typedef wave<500, sawtooth_value> sawtooth_wave;
		typedef wave<500, pulse_value> pulse_wave;
	}
}

#endif // WAVE_HPP
