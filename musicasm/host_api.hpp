#if !defined HOST_API_HPP
#define HOST_API_HPP

#include <iterator>

#include "device_info.hpp"

namespace tvr
{
	namespace pa
	{
		/**
		 * A C++ binding for PortAudio's PaHostApiInfo structure.
		 **/
		class host_api_info
		{
		public:
			/**
			 * A C++-style iterator over the collection of PaHostApiInfo structures.
			 * This isn't quite according to the standard, in that it does not support
			 * opereator->(), but this should none-the-less be servicable.
			 **/
			class iterator: public std::iterator<std::random_access_iterator_tag, host_api_info, PaHostApiIndex>
			{
			public:
				iterator():_index(::Pa_GetDefaultHostApi()) {}
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

			/**
			 * A C++-style device_info iterator.
			 * 
			 * This provides a way to iterate over a list of devices for a provided host api.  It isn't quite
			 * according to standard, as it does not support operator->(), but should none-the-less be servicable.
			 * This iterator requires the host_api_info whose devices you wish to iterate.
			 **/
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
			/// Default constructor
			host_api_info():_info(0)
			{
				_index = ::Pa_GetDefaultHostApi();
				_info = ::Pa_GetHostApiInfo(_index);
			}

			/// Constructor using PaHostApiIndex.
			host_api_info(PaHostApiIndex index): _index(index)
			{
				check_index();
				_info = ::Pa_GetHostApiInfo(index);
			}

			/// Copy constructor
			host_api_info(const host_api_info& orig)
			{
				*this = orig;
			}

			/// Copy assignment operator
			host_api_info& operator=(const host_api_info& orig)
			{
				_info = orig._info;
				_index = orig._index;
				return *this;
			}

			/// Returns a reference to the PaHostApiInfo structure itself.
			const PaHostApiInfo& get_info() const
			{
				return *_info;
			}

			/// Acquires the PaHostApiIndex for this instance.
			PaHostApiIndex get_index() const
			{
				return _index;
			}

			/// Acquires a count of devices for this host_api_info.
			int get_device_count() const
			{
				return _info->deviceCount;
			}

			/// Converts the host api device index to a device index.
			PaDeviceIndex get_device_index(int hostApiDeviceIndex) const
			{
				return ::Pa_HostApiDeviceIndexToDeviceIndex(_index, hostApiDeviceIndex);
			}

			/// Acquires an iterator from the start of the devices for this host_api_info.
			device_iterator device_begin() const
			{
				device_iterator result(*this, 0);
				return result;
			}

			/// Acquires an iterator representing one past the end of the devices for this host_api_info.
			device_iterator device_end() const
			{
				device_iterator result(*this, get_device_count());
				return result;
			}

			/// Acquires an iterator from the start of all the host_api_info instances available.
			static iterator begin()
			{
				iterator iter(0);
				return iter;
			}

			/// Acquires an iterator of the end of all the host_api_info instances available.
			static iterator end()
			{
				iterator iter(::Pa_GetHostApiCount());
				return iter;
			}

			/// Acquires a host_api_info object from a PaHostApiTypeId.
			static host_api_info get_host_api_info(PaHostApiTypeId id)
			{
				host_api_info result(::Pa_HostApiTypeIdToHostApiIndex(id));
				return result;
			}

			/// Acquires a device from this host_api_info based on its host api device index.
			device_info get_device(int hostApiDeviceIndex)
			{
				device_info result(::Pa_HostApiDeviceIndexToDeviceIndex(_index, hostApiDeviceIndex));
				return result;
			}

			/// Acquires the default output device for this host api info.
			device_info get_default_output_device()
			{
				device_info result(::Pa_HostApiDeviceIndexToDeviceIndex(_index, _info->defaultOutputDevice));
				return result;
			}

			/// Acquires the default input device for this host api info.
			device_info get_default_input_device()
			{
				device_info result(::Pa_HostApiDeviceIndexToDeviceIndex(_index, _info->defaultInputDevice));
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
			const PaHostApiInfo* _info;
			PaHostApiIndex _index;
		};
	}
}




#endif // HOST_API_HPP

