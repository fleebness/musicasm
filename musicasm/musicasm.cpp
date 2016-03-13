// PortAudioSine.cpp : Defines the entry point for the console application.
//

#include <cmath>
#include <iomanip>
#include <iostream>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include "adsr.hpp"
#include "base_note.hpp"
#include "console_wait.hpp"
#include "port_audio.hpp"
#include "scale.hpp"
#include "tone_player.hpp"

bool finished = false;

void set_finished()
{
	finished = true;
}

int main()
{
	using namespace tvr::pa;
	using namespace tvr::ma;

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
			// western::scale_432 scale(static_cast<std::size_t>(49));
			// western::scale_432 scale(static_cast<std::size_t>(49), western::aeolian_mode);
			western::scale_432 scale(static_cast<std::size_t>(49), western::dorian_mode);
			adsr env;
			adsr::v_point point;
			point._duration = 0.05;
			point._target = 1.0;
			env.set_attack(point);
			point._duration = 0.03;
			point._target = 0.75;
			env.set_decay(point);
			point._duration = 0;
			point._target = 0.75;
			env.set_sustain(point);
			point._duration = 0.04;
			point._target = 0.0;
			env.set_release(point);

			voice voice1;
			// voice1._tone.reset(new sine_wave());
			voice1._tone.reset(new sawtooth_wave());
			// voice1._tone.reset(new pulse_wave());
			voice voice2;
			// voice2._tone.reset(new sine_wave());
			voice2._tone.reset(new sawtooth_wave());
			// voice2._tone.reset(new pulse_wave());
			voice voice3;
			// voice3._tone.reset(new sine_wave());
			voice3._tone.reset(new sawtooth_wave());
			// voice3._tone.reset(new pulse_wave());

			amplitude vol = 0.15;
			double dur = 0.5;
			for (int i = 0; i < 8; ++i)
			{
				env(voice1, base_note(scale.get_freq(i, 0), vol, dur));
				env(voice2, base_note(scale.get_freq(i + 2, 0), vol, dur));
				env(voice3, base_note(scale.get_freq(i + 4, 0), vol, dur));
			}
			vol = 0.75;
			for (int i = 7; i > 0; --i)
			{
				env(voice1, base_note(scale.get_freq(i, 0), vol, dur));
				env(voice2, base_note(scale.get_freq(i + 2, 0), vol, dur));
				env(voice3, base_note(scale.get_freq(i + 4, 0), vol, dur));
			}
			if (true)
			{
				int i = 0;
				dur = 0.75;
				env(voice1, base_note(scale.get_freq(i, 0), vol, dur));
				env(voice2, base_note(scale.get_freq(i + 2, 0), vol, dur));
				env(voice3, base_note(scale.get_freq(i + 4, 0), vol, dur));
			}

			player.add_voice(voice1);
			player.add_voice(voice2);
			player.add_voice(voice3);

			player.set_finished_fn(&set_finished);

			player.play();
			std::cout << "Tap 'Q' to end." << std::endl;
			while (!finished)
			{
				::Pa_Sleep(100);
				if (_kbhit())
				{
					int ch = _getch();
					if (ch == 'q' || ch == 'Q')
					{
						finished = true;
					}
				}
			}
		}
		else
		{
			std::cout << "Unable to initialize a sound library." << std::endl;
		}
	}
    return 0;
}

