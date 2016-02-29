#if !defined PORT_AUDIO_HPP
#define PORT_AUDIO_HPP

#include "portaudio.h"

#include "device_info.hpp"
#include "host_api.hpp"
#include "stream.hpp"


class port_audio_init
{
public:
	static port_audio_init& get_port_audio_init()
	{
		static port_audio_init init;
		return init;
	}
	PaError get_error() { return _error; }
private:
	port_audio_init()
	{
		_error = ::Pa_Initialize();
	}


	~port_audio_init()
	{
		::Pa_Terminate();
	}
private:
	PaError _error;
};


#endif // PORT_AUDIO_HPP

