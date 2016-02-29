#if !defined HOST_API_HPP
#define HOST_API_HPP

#include <iterator>

#include "device_info.hpp"

namespace tvr
{
	namespace pa
	{
		class host_api_info
		{
		public:
			class iterator: public std::iterator<std::random_access_iterator_tag, host_api_info, PaHostApiIndex>
			{
			public:
				iterator() {}
				iterator(PaHostApiIndex index) : _index(index) { check_index(); }
				~iterator(){}

				bool operator==(const iterator& item) const
				{
					return _index == item._index;
				}

				bool operator!=(const iterator& item) const
				{
					return _index != item._index;
				}

				bool operator<(const iterator& item) const
				{
					return _index < item._index;
				}

				iterator& operator=(const iterator& orig)
				{
					_index = orig._index;
					return *this;
				}

				iterator& operator++()
				{
					// We can never have more than Pa_GetHostApiCount host apis.
					if (++_index > ::Pa_GetHostApiCount())
					{
						--_index;
					}
					return *this;
				}

				iterator& operator++(int)
				{
					// We can never have more than Pa_GetHostApiCount host apis.
					if (++_index > ::Pa_GetHostApiCount())
					{
						--_index;
					}
					return *this;
				}

				iterator& operator--()
				{
					// We can never have less than zero.
					if (--_index < 0)
					{
						_index = 0;
					}
					return *this;
				}

				iterator& operator--(int)
				{
					// We can never have less than zero.
					if (--_index < 0)
					{
						_index = 0;
					}
					return *this;
				}

				iterator& operator+=(PaHostApiIndex diff)
				{
					_index += diff;
					check_index();
					return *this;
				}

				iterator& operator-=(PaHostApiIndex diff)
				{
					_index -= diff;
					check_index();
					return *this;
				}

				host_api_info operator[](PaHostApiIndex loc) const
				{
					iterator iter(loc);
					return *iter;
				}

				host_api_info operator*() const
				{
					host_api_info result(_index);
					return result;
				}

			private:
				void check_index()
				{
					if (_index < 0)
						_index = 0;
					if (_index > ::Pa_GetHostApiCount())
						_index = ::Pa_GetHostApiCount();
				}

			private:
				PaHostApiIndex _index;
			};

			struct device_iterator : public std::iterator<std::random_access_iterator_tag, device_info, int>
			{
				device_iterator(const host_api_info& host): _index(0), _host(host){ check_index(); }
				device_iterator(const host_api_info& host, int index):_index(index), _host(host){ check_index(); }
				~device_iterator(){}

				bool operator==(const device_iterator& item) const
				{
					return _index == item._index;
				}

				bool operator!=(const device_iterator& item) const
				{
					return _index != item._index;
				}

				bool operator<(const device_iterator& item) const
				{
					return _index < item._index;
				}

				device_iterator& operator=(const device_iterator& orig)
				{
					_index = orig._index;
					return *this;
				}

				device_iterator& operator++()
				{
					// We can never have more than Pa_GetHostApiCount host apis.
					if (++_index > max())
					{
						--_index;
					}
					return *this;
				}

				device_iterator& operator++(int)
				{
					// We can never have more than Pa_GetHostApiCount host apis.
					if (++_index > max())
					{
						--_index;
					}
					return *this;
				}

				device_iterator& operator--()
				{
					// We can never have less than zero.
					if (--_index < 0)
					{
						_index = 0;
					}
					return *this;
				}

				device_iterator& operator--(int)
				{
					// We can never have less than zero.
					if (--_index < 0)
					{
						_index = 0;
					}
					return *this;
				}

				device_iterator& operator+=(int diff)
				{
					_index += diff;
					check_index();
					return *this;
				}

				device_iterator& operator-=(int diff)
				{
					_index -= diff;
					check_index();
					return *this;
				}

				device_info operator[](int loc) const
				{
					device_iterator iter(_host, loc);
					return *iter;
				}

				device_info operator*() const
				{
					device_info result(_host.get_device_index(_index));
					return result;
				}

			private:
				void check_index()
				{
					if (_index < 0)
						_index = 0;
					if (_index > max())
						_index = max();
				}

				int max() const
				{
					return _host.get_info().deviceCount;
				}
			private:
				int _index;
				const host_api_info& _host;
			};

		public:
			host_api_info():_info(0)
			{
				_index = ::Pa_GetDefaultHostApi();
				_info = ::Pa_GetHostApiInfo(_index);
			}

			host_api_info(PaHostApiIndex index): _index(index)
			{
				_info = ::Pa_GetHostApiInfo(index);
			}

			host_api_info(const host_api_info& orig)
			{
				*this = orig;
			}

			host_api_info& operator=(const host_api_info& orig)
			{
				_info = orig._info;
				_index = orig._index;
				return *this;
			}

			const PaHostApiInfo& get_info() const
			{
				return *_info;
			}

			PaHostApiIndex get_index() const
			{
				return _index;
			}

			int get_device_count() const
			{
				return _info->deviceCount;
			}

			PaDeviceIndex get_device_index(int hostApiDeviceIndex) const
			{
				return ::Pa_HostApiDeviceIndexToDeviceIndex(_index, hostApiDeviceIndex);
			}

			device_iterator device_begin() const
			{
				device_iterator result(*this, 0);
				return result;
			}

			device_iterator device_end() const
			{
				device_iterator result(*this, get_device_count());
				return result;
			}

			static iterator begin()
			{
				iterator iter(0);
				return iter;
			}

			static iterator end()
			{
				iterator iter(::Pa_GetHostApiCount());
				return iter;
			}

			static host_api_info get_host_api_info(PaHostApiTypeId id)
			{
				host_api_info result(::Pa_HostApiTypeIdToHostApiIndex(id));
				return result;
			}

			device_info get_device(int hostApiDeviceIndex)
			{
				device_info result(::Pa_HostApiDeviceIndexToDeviceIndex(_index, hostApiDeviceIndex));
				return result;
			}

			device_info get_default_output_device()
			{
				device_info result(::Pa_HostApiDeviceIndexToDeviceIndex(_index, _info->defaultOutputDevice));
				return result;
			}

			device_info get_default_input_device()
			{
				device_info result(::Pa_HostApiDeviceIndexToDeviceIndex(_index, _info->defaultInputDevice));
				return result;
			}
		private:
			const PaHostApiInfo* _info;
			PaHostApiIndex _index;
		};
	}
}




#endif // HOST_API_HPP

