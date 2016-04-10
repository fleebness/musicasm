// PortAudioSine.cpp : Defines the entry point for the console application.
//

#include <cmath>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include "adsr.hpp"
#include "base_note.hpp"
#include "beat.hpp"
#include "console_wait.hpp"
#include "port_audio.hpp"
#include "scale.hpp"
#include "tone_player.hpp"

bool finished = false;

void set_finished()
{
	finished = true;
}

void print_durations(const std::vector<tvr::ma::duration>& durations)
{
	std::cout << "====================================" << std::endl;
	for (std::vector<tvr::ma::duration>::const_iterator iter = durations.begin();
		iter != durations.end();
		++iter)
	{
		std::cout << *iter << std::endl;
	}
	std::cout << "====================================" << std::endl;
}

int main()
{
	using namespace tvr::pa;
	using namespace tvr::ma;

	if (false)
	{
		beat a(8);
		beat b(1, 3);
		beat c(1, 2);
		a.set_division_to_beat(2, b);
		a.set_division_to_beat(4, c);
		b.set_tie(2);
		a.set_division_to_beat(6, b);
		b.erase_tie(2);
		a.set_division_to_beat(8, c);
		a.set_tie(8);



		typedef std::vector<duration> durations;
		durations durs;
		a.get_durations(durs, 1.0, a.get_durations(durs, 1.0));

		for (durations::iterator iter = durs.begin(); iter != durs.end(); ++iter)
		{
			std::cout << *iter << std::endl;
		}
	}

	if (false)
	{
		std::cout << "====================================" << std::endl;
		beat a(4);                  // represents 4 pulses, no divisions.
		beat duplet(1, 2);                // represents 1 pulse, 2 divisions, so a duplet.
		beat triplet(1, 3);               // represents 1 pulse, 3 divisions, so a triplet.
		beat altered_triplet(triplet);    // copy the triplet
		altered_triplet.set_division_to_beat(3, duplet);  // make the third pulse in the triplet a duplet
		a.set_division_to_beat(2, duplet);            // Set the 2nd beat to a duplet.
		a.set_division_to_beat(3, triplet);           // Set the 3rd beat to a triplet
		a.set_division_to_beat(4, altered_triplet);   // Set the 4th beat to our altered triplet.
		typedef std::vector<duration> durations;
		durations durs;
		a.get_durations(durs, 1.0);
		for (durations::iterator iter = durs.begin(); iter != durs.end(); ++iter)
		{
			std::cout << *iter << std::endl;
		}
	}

	if (true)
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
			western::scale_432 scale(static_cast<std::size_t>(49));

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

			typedef std::vector<std::size_t> intervals;
			intervals notes;
			notes.push_back(2);
			notes.push_back(1);
			notes.push_back(0);
			notes.push_back(1);
			notes.push_back(2);
			notes.push_back(2);
			notes.push_back(2);
			notes.push_back(1);
			notes.push_back(1);
			notes.push_back(1);
			notes.push_back(2);
			notes.push_back(4);
			notes.push_back(4);
			notes.push_back(2);
			notes.push_back(1);
			notes.push_back(0);
			notes.push_back(1);
			notes.push_back(2);
			notes.push_back(2);
			notes.push_back(2);
			notes.push_back(2);
			notes.push_back(1);
			notes.push_back(1);
			notes.push_back(2);
			notes.push_back(1);
			notes.push_back(0);

			beat meter(26); // 32 pulses in Merrily We Stroll Along.
			beat whole(4, 1);  // It has a whole note.
			beat half(2, 1);   // And a half note.  Rest of them are quarter notes.
			meter.set_division_to_beat(7, half);
			meter.set_division_to_beat(10, half);
			meter.set_division_to_beat(13, half);
			meter.set_division_to_beat(26, whole);
			typedef std::vector<duration> durations;
			durations durs;
			meter.get_durations(durs, 0.5);
			print_durations(durs);

			voice voice1;
			// voice1._tone.reset(new sine_wave());
			voice1._tone.reset(new sawtooth_wave());
			// voice1._tone.reset(new pulse_wave());

			amplitude vol = 0.15;

			durations::iterator dur_iter = durs.begin();
			intervals::iterator int_iter = notes.begin();
			while (dur_iter != durs.end()
				&& int_iter != notes.end()
				)
			{
				env(voice1, base_note(scale.get_freq(*int_iter, 0), vol, *dur_iter));
				++dur_iter;
				++int_iter;
			}

			player.add_voice(voice1);
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

	if (false)
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
			// voice1._tone.reset(new sawtooth_wave());
			voice1._tone.reset(new pulse_wave());
			voice voice2;
			// voice2._tone.reset(new sine_wave());
			voice2._tone.reset(new sawtooth_wave());
			// voice2._tone.reset(new pulse_wave());
			voice voice3;
			voice3._tone.reset(new sine_wave());
			// voice3._tone.reset(new sawtooth_wave());
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

