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
		// Testing adsr.
		adsr env;
		adsr::v_point attack, decay, sustain, release;
		attack._duration = 0.05;
		attack._target = 1.0;
		env.set_attack(attack);
		decay._duration = 0.03;
		decay._target = 0.75;
		env.set_decay(decay);
		sustain._duration = 0;
		sustain._target = 0.75;
		env.set_sustain(sustain);
		release._duration = 0.04;
		release._target = 0.0;
		env.set_release(release);
		typedef std::vector< amplitude_dur > amp_dur_coll;
		amp_dur_coll env_info;
		env.get_durations(env_info, 1.0, 0.25);
		std::cout << "size: " << env_info.size() << std::endl;
		for (amplitude_dur amp : env_info)
		{
			std::cout << "dur: " << amp._length << ", amp: " << amp._val << std::endl;
		}
	}

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
			western::concert_scale scale_e(static_cast<std::size_t>(49));
			western::five_limit_a_scale scale(static_cast<std::size_t>(49));
			western::pythagorian_a_scale scale_p(static_cast<std::size_t>(49));

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
			env.set_release(point); //                         1                           2                       3
			 //                    1   2   3   4   5 6 7 8   9 0 1 2   3 4 5 6 7   8   9   0   1 2 3 4   5 6 7 8 9 0 1 2
			std::size_t moo[] =  { 2,  1,  0,  1,  2,2,2,    1,1,1,    2,4,4,  2,  1,  0,  1,  2,2,2,2,  1,1,2,1,0,     -1 };
			std::size_t bark[] = { 2,3,2,1,0,1,2,3,5,5,5,1,2,3,3,3,2,1,0,2,2,  2,3,2,1,0,1,2,3,5,5,5,4,3,3,5,4,3,2,     -1 };
			typedef std::vector<int> intervals;
			intervals notes;
			for (unsigned int i = 0; moo[i] != -1; ++i)
			{
				notes.push_back(moo[i]);
			}
			intervals notesb;
			for (unsigned int i = 0; bark[i] != -1; ++i)
			{
				notesb.push_back(bark[i]);
			}

			beat meter(32);    // 32 pulses in Merrily We Stroll Along.
			beat meter2(32);   // Same for second voice.
			beat whole(4, 1);  // It has a whole note.
			beat half(2, 1);   // And a half note.  Rest of them are quarter notes.
			beat eight(1, 2);  // Half our quarter is an eight;
			meter.set_division_to_beat(7, half);
			meter.set_division_to_beat(11, half);
			meter.set_division_to_beat(15, half);
			meter.set_division_to_beat(29, whole);

			meter2.set_division_to_beat(1, eight);
			meter2.set_division_to_beat(2, eight);
			meter2.set_division_to_beat(3, eight);
			meter2.set_division_to_beat(4, eight);
			meter2.set_division_to_beat(8, eight);
			meter2.set_division_to_beat(12, eight);
			meter2.set_division_to_beat(15, half);
			meter2.set_division_to_beat(17, eight);
			meter2.set_division_to_beat(18, eight);
			meter2.set_division_to_beat(19, eight);
			meter2.set_division_to_beat(20, eight);
			meter2.set_division_to_beat(24, eight);
			meter2.set_division_to_beat(29, whole);
			typedef std::vector<duration> durations;
			durations durs, dursb;
			meter.get_durations(durs, 0.5);
			meter2.get_durations(dursb, 0.5);
			// print_durations(durs);

			voice voice1;
			voice voice2;
			voice voice3;
			voice1._tone.reset(new sawtooth_wave());
			voice2._tone.reset(new sawtooth_wave());
			voice3._tone.reset(new sawtooth_wave());

			voice voice1b;
			voice voice2b;
			voice voice3b;
			voice1b._tone.reset(new sawtooth_wave());
			voice2b._tone.reset(new sawtooth_wave());
			voice3b._tone.reset(new sawtooth_wave());

			amplitude vol = 0.15;
			amplitude silence = 0.0;

			std::cout << "Voice 1:";
			durations::iterator dur_iter = durs.begin();
			intervals::iterator int_iter = notes.begin();
			while (dur_iter != durs.end()
				&& int_iter != notes.end()
				)
			{
				// std::cout << " " << dur_iter->operator double();
				env(voice1, base_note( scale.get_freq(*int_iter, 0), vol, *dur_iter));
				env(voice2, base_note(0, silence, *dur_iter));
				env(voice3, base_note(0, silence, *dur_iter));
				/*
				voice1.add_freq(scale.get_freq(*int_iter, 0), *dur_iter);
				voice2.add_freq(scale.get_freq(*int_iter, 0), *dur_iter);
				voice3.add_freq(scale.get_freq(*int_iter, 0), *dur_iter);
				voice1.add_vol(vol, *dur_iter);
				voice2.add_vol(vol, *dur_iter);
				voice3.add_vol(vol, *dur_iter);
				*/
				++dur_iter;
				++int_iter;
			}
			/*
			dur_iter = durs.begin();
			int_iter = notes.begin();
			while (dur_iter != durs.end()
				&& int_iter != notes.end()
				)
			{
				env(voice1, base_note(0, silence, *dur_iter));
				env(voice2, base_note(scale_e.get_freq(*int_iter, 0), vol, *dur_iter));
				env(voice3, base_note(0, silence, *dur_iter));
				++dur_iter;
				++int_iter;
			}
			dur_iter = durs.begin();
			int_iter = notes.begin();
			while (dur_iter != durs.end()
				&& int_iter != notes.end()
				)
			{
				env(voice1, base_note(0, silence, *dur_iter));
				env(voice2, base_note(0, silence, *dur_iter));
				env(voice3, base_note(scale_p.get_freq(*int_iter, 0), vol, *dur_iter));
				++dur_iter;
				++int_iter;
			}
			dur_iter = durs.begin();
			int_iter = notes.begin();
			while (dur_iter != durs.end()
				&& int_iter != notes.end()
				)
			{
				env(voice1, base_note(scale.get_freq(*int_iter, 0), vol, *dur_iter));
				env(voice2, base_note(scale_e.get_freq(*int_iter, 0), vol, *dur_iter));
				env(voice3, base_note(scale_p.get_freq(*int_iter, 0), vol, *dur_iter));
				++dur_iter;
				++int_iter;
			}
			*/

			std::cout << std::endl;
			std::cout << "Voice 2:";
			// Second voice
			dur_iter = dursb.begin();
			int_iter = notesb.begin();
			while (dur_iter != dursb.end()
				&& int_iter != notesb.end()
				)
			{
				// std::cout << " " << dur_iter->operator double();
				env(voice1b, base_note(scale.get_freq(*int_iter, 0), vol, *dur_iter));
				env(voice2b, base_note(0, silence, *dur_iter));
				env(voice3b, base_note(0, silence, *dur_iter));
				/*
				voice1b.add_freq(scale.get_freq(*int_iter, 0), *dur_iter);
				voice2b.add_freq(scale.get_freq(*int_iter, 0), *dur_iter);
				voice3b.add_freq(scale.get_freq(*int_iter, 0), *dur_iter);
				voice1b.add_vol(vol, *dur_iter);
				voice2b.add_vol(vol, *dur_iter);
				voice3b.add_vol(vol, *dur_iter);
				*/
				++dur_iter;
				++int_iter;
			}
			std::cout << std::endl;
			/*
			dur_iter = dursb.begin();
			int_iter = notesb.begin();
			while (dur_iter != dursb.end()
				&& int_iter != notesb.end()
				)
			{
				env(voice1b, base_note(0, silence, *dur_iter));
				env(voice2b, base_note(scale_e.get_freq(*int_iter, 0), vol, *dur_iter));
				env(voice3b, base_note(0, silence, *dur_iter));
				++dur_iter;
				++int_iter;
			}
			dur_iter = dursb.begin();
			int_iter = notesb.begin();
			while (dur_iter != dursb.end()
				&& int_iter != notesb.end()
				)
			{
				env(voice1b, base_note(0, silence, *dur_iter));
				env(voice2b, base_note(0, silence, *dur_iter));
				env(voice3b, base_note(scale_p.get_freq(*int_iter, 0), vol, *dur_iter));
				++dur_iter;
				++int_iter;
			}
			dur_iter = dursb.begin();
			int_iter = notesb.begin();
			while (dur_iter != dursb.end()
				&& int_iter != notesb.end()
				)
			{
				env(voice1b, base_note(scale.get_freq(*int_iter, 0), vol, *dur_iter));
				env(voice2b, base_note(scale_e.get_freq(*int_iter, 0), vol, *dur_iter));
				env(voice3b, base_note(scale_p.get_freq(*int_iter, 0), vol, *dur_iter));
				++dur_iter;
				++int_iter;
			}
			*/

			player.add_voice(voice1);
			player.add_voice(voice2);
			player.add_voice(voice3);
			player.add_voice(voice1b);
			player.add_voice(voice2b);
			player.add_voice(voice3b);
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

