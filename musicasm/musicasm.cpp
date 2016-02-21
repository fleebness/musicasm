// PortAudioSine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include "port_audio.hpp"
#include "scale.hpp"
#include "tone_player.hpp"

#define PLAY_MAJOR
// #define PLAY_MINOR
// #define PLAY_DORIAN

template< typename scale >
void play_note(tvr::pa::tone_player::wave_ptr voice, int i, scale& scale)
{
	voice->set_freq(scale.get_note(i));
	std::cout << (i + 1) << std::endl;
	::Pa_Sleep(500);
}

int main()
{
	using namespace tvr::pa;

	{
		port_audio_init& init = port_audio_init::get_port_audio_init();
		host_api_info def_host;
		PaStreamParameters outParam = { 0 };
		device_info output_device = def_host.get_default_output_device();
		outParam.channelCount = 2; // stereo
		outParam.device = output_device.get_index();
		outParam.sampleFormat = paFloat32;
		outParam.suggestedLatency = output_device.get_info()->defaultLowOutputLatency;

		tone_player player(def_host, output_device, outParam);
		player.set_vol(static_cast<float>(0.3));

		western::concert_scale scale(static_cast<std::size_t>(49));
		western::concert_scale minor_scale(static_cast<std::size_t>(49), western::aeolian_mode);
		western::concert_scale dorian_scale(static_cast<std::size_t>(49), western::dorian_mode);
		western::scale_432 alternate(static_cast<std::size_t>(49));
		western::scale_432 alternate_minor(static_cast<std::size_t>(49), western::aeolian_mode);
		western::scale_432 alternate_dorian(static_cast<std::size_t>(49), western::dorian_mode);

		tone_player::wave_ptr voice(new sine_wave());

		player.add_wave(voice);
		player.play();
		play_note(voice, -9, alternate);
		::Pa_Sleep(1000);
		play_note(voice, -1, alternate);
		::Pa_Sleep(1000);
#if defined PLAY_MAJOR
		for (int i = 0; i < 8; ++i)
		{
			play_note(voice, i, alternate);
		}
		for (int i = 7; i >= 0; --i)
		{
			play_note(voice, i, scale);
		}
#endif // PLAY_MAJOR
#if defined PLAY_MINOR
		for (int i = 0; i < 8; ++i)
		{
			play_note(voice, i, alternate_minor);
		}
		for (int i = 7; i >= 0; --i)
		{
			play_note(voice, i, minor_scale);
		}
#endif // PLAY_MINOR
#if defined PLAY_DORIAN
		for (int i = 0; i < 8; ++i)
		{
			play_note(voice, i, alternate_dorian);
		}
		for (int i = 7; i >= 0; --i)
		{
			play_note(voice, i, dorian_scale);
		}
#endif // PLAY_DORIAN
		::Pa_Sleep(500);
	}
    return 0;
}

