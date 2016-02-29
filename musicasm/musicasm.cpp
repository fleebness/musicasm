// PortAudioSine.cpp : Defines the entry point for the console application.
//

#if defined _MSC_VER
#include "stdafx.h"
#endif

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

// #define PLAY_MAJOR
// #define PLAY_MINOR
// #define PLAY_DORIAN

bool finished = false;

void set_finished()
{
	finished = true;
}

int main()
{
	using namespace tvr::pa;

	{
		port_audio_init& init = port_audio_init::get_port_audio_init();
		init.get_error(); // TODO: Evaluate this.
		host_api_info def_host;
		PaStreamParameters outParam = { 0 };
		device_info output_device = def_host.get_default_output_device();
		outParam.channelCount = 2; // stereo
		outParam.device = output_device.get_index();
		outParam.sampleFormat = paFloat32;
		if (output_device.get_info() != 0)
		{
			outParam.suggestedLatency = output_device.get_info()->defaultLowOutputLatency;

			tone_player player(def_host, output_device, outParam);
			player.set_vol(static_cast<float>(0.5));

			// western::concert_scale scale(static_cast<std::size_t>(49));
			// western::concert_scale scale(static_cast<std::size_t>(49), western::aeolian_mode);
			// western::concert_scale scale(static_cast<std::size_t>(49), western::dorian_mode);
			western::scale_432 scale(static_cast<std::size_t>(49));
			// western::scale_432 scale(static_cast<std::size_t>(49), western::aeolian_mode);
			// western::scale_432 scale(static_cast<std::size_t>(49), western::dorian_mode);

			tvr::pa::voice voice1;
			voice1._tone.reset(new sine_wave());
			tvr::pa::voice voice2;
			voice2._tone.reset(new sine_wave());
			tvr::pa::voice voice3;
			voice3._tone.reset(new sine_wave());

			double vol = 1.0;
			double dur = 0.5;
			for (int i = 0; i < 8; ++i)
			{
				voice1._notes.push_back(base_note(scale.get_freq(i, 0), vol, dur));
				voice2._notes.push_back(base_note(scale.get_freq(i + 2, 0), vol, dur));
				voice3._notes.push_back(base_note(scale.get_freq(i + 4, 0), dur, dur));
			}
			for (int i = 7; i > 0; --i)
			{
				voice1._notes.push_back(base_note(scale.get_freq(i, 0), vol, dur));
				voice2._notes.push_back(base_note(scale.get_freq(i + 2, 0), vol, dur));
				voice3._notes.push_back(base_note(scale.get_freq(i + 4, 0), dur, dur));
			}
			if (true)
			{
				int i = 0;
				dur = 0.75;
				voice1._notes.push_back(base_note(scale.get_freq(i, 0), vol, dur));
				voice2._notes.push_back(base_note(scale.get_freq(i + 2, 0), vol, dur));
				voice3._notes.push_back(base_note(scale.get_freq(i + 4, 0), dur, dur));
			}

			player.add_voice(voice1);
			player.add_voice(voice2);
			player.add_voice(voice3);

			player.set_finished_fn(&set_finished);
			player.play();
			while (!finished)
			{
				::Pa_Sleep(100);
			}
		}
		else
		{
			std::cout << "Unable to initialize a sound library." << std::endl;
		}
	}
    return 0;
}

