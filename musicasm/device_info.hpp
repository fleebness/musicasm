#if !defined DEVICE_INFO_HPP
#define DEVICE_INFO_HPP

namespace tvr
{
	namespace pa
	{
		class device_info
		{
		public:
			device_info(PaDeviceIndex index): _index(index)
			{
				_info = ::Pa_GetDeviceInfo(_index);
			}
			~device_info(){}

			const PaDeviceInfo* get_info() const
			{
				return _info;
			}

			PaDeviceIndex get_index() const
			{
				return _index;
			}

			static device_info get_default_input_device()
			{
				device_info result(::Pa_GetDefaultInputDevice());
				return result;
			}

			static device_info get_default_output_device()
			{
				device_info result(::Pa_GetDefaultOutputDevice());
				return result;
			}

		private:
			PaDeviceIndex _index;
			const PaDeviceInfo* _info;
		};
	}
}

#endif // DEVICE_INFO_HPP
