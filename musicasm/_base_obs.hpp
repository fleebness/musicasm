#if !defined _BASE_OBS_HPP
#define _BASE_OBS_HPP

#include <memory>

#include "_basic_value.hpp"

namespace tvr
{
	namespace ma
	{
		/// Interface for obtaining the frequency of a given object.
		struct _freq
		{
			typedef std::shared_ptr<_freq> ptr;
			virtual frequency get_freq() const = 0;
		};

		/// Interface for obtaining the duration of a given object.
		struct _dur
		{
			typedef std::shared_ptr<_dur> ptr;
			virtual duration get_dur() const = 0;
		};

		/// Simple class to store/retrieve frequencies.
		class freq : public _freq
		{
		public:
			freq(const frequency& value): _frequency(value){}
			virtual frequency get_freq() const { return _frequency; }
		private:
			freq(){}
		private:
			frequency _frequency;
		};

		/// Simple class to store/retrieve durations.
		class dur : public _dur
		{
		public:
			dur(const duration& value): _duration(value) {}
			virtual duration get_dur() const { return _duration; }
		private:
			dur(){}
		private:
			duration _duration;
		};
	}
}

#endif // _BASE_OBS_HPP
