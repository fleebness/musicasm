#if !defined TONE_PLAYER_HPP
#define TONE_PLAYER_HPP

#include <functional>
#include <memory>
#include <vector>

#include "base_note.hpp"
#include "port_audio.hpp"
#include "voice.hpp"
#include "wave.hpp"

namespace tvr
{
	namespace pa
	{
		class tone_player
		{
		public:
			typedef std::function<void()> void_fn;
			typedef std::vector<voice> voices;
			typedef voice::wave_ptr wave_ptr;
		public:
			tone_player(tvr::pa::host_api_info& host, tvr::pa::device_info& device, PaStreamParameters& params) :
				_stream(0),
				_player(0),
				_host(host),
				_params(params),
				_device(device),
				_time_started(0.0),
				_maxed_value(1.0),
				_vol(1.0),
				_playing(false)
			{
			}

			~tone_player() { abort(); end_stream(); }

			void play()
			{
				if (_player)
				{
					_player->start();
				}
				else
				{
					open();
					start_play();
					_playing = true;
				}
			}

			void stop()
			{
				if (_player)
				{
					_player->stop();
					_playing = false;
				}
			}

			void abort()
			{
				if (_player)
				{
					delete _player;
					_player = 0;
					_playing = false;
				}
			}

			void set_finished_fn(void_fn fn)
			{
				_finished_fn = fn;
			}

			void add_wave(wave_ptr wave)
			{
				_voices.push_back(voice());
				_voices[_voices.size() - 1]._tone = wave;
			}

			void add_voice(voice voice_)
			{
				_voices.push_back(voice_);
			}

			void clear_waves()
			{
				_maxed_value = 1.0;
				_voices.clear();
			}

			void set_vol(float vol)
			{
				_vol = vol;
			}

		private:
			static int glob_callback(const void* in,
				void* out,
				unsigned long frame,
				const PaStreamCallbackTimeInfo* time,
				PaStreamCallbackFlags status,
				void* userData)
			{
				tone_player* ob = static_cast<tone_player*>(userData);
				return ob->callback(in, out, frame, time, status);
			}

			static void glob_finished_callback(void* userData)
			{
				tone_player* ob = static_cast<tone_player*>(userData);
				if (ob != 0)
				{
					ob->finished_callback();
				}
			}

			int callback(const void* in,
				void* out,
				unsigned long frame,
				const PaStreamCallbackTimeInfo* time,
				PaStreamCallbackFlags status)
			{
				int result = paContinue;
				float* dest = static_cast<float*>(out);
				if (_time_started == 0.0)
				{
					_time_started = time->currentTime;
				}
				PaTime now = time->currentTime - _time_started;
				std::size_t dead_voice_count = 0;
				for (unsigned long c = 0; c < frame; ++c)
				{
					float value = 0.0;
					for (std::size_t i = 0; i < _voices.size(); ++i)
					{
						if (!_voices[i].is_dead())
						{
							_voices[i].set_time(now);
							if (_voices[i]._tone.get() != 0)
							{
								value += _voices[i]._tone->get_next_value();
							}
						}
						else
						{
							++dead_voice_count;
						}
					}

					if (dead_voice_count == _voices.size())
					{
						// We have no more voices.
						result = paComplete;
					}
					// We do this to avoid clipping, and allows for any set of waves to add together without
					// breaking the maximum value, yet retaining the sense of the summed wave.
					if (value > 1.0 && value > _maxed_value)
					{
						_maxed_value = value;
					}
					value /= _maxed_value;
					value *= _vol;

					for (std::size_t ch = 0; ch < static_cast<std::size_t>(_params.channelCount); ++ch)
					{
						*dest++ = value;
					}
				}
				return result;
			}

			void finished_callback()
			{
				if (_finished_fn)
				{
					_playing = false;
					void_fn fn = _finished_fn;
					_finished_fn = void_fn();
					fn();
				}
			}

			void start_play()
			{
				if (_player == 0 && _stream)
				{
					_player = new tvr::pa::stream::player(*_stream);
				}
				else
				{
					_player->start();
				}
			}

			void end_stream()
			{
				if (_stream)
				{
					delete _stream;
					_stream = 0;
				}
			}

			void open()
			{
				_stream = new tvr::pa::stream(0, &_params, _device.get_info()->defaultSampleRate, paFramesPerBufferUnspecified, paClipOff, &tone_player::glob_callback, this);
				_stream->set_finished_callback(&tone_player::glob_finished_callback);
			}

		private:
			tvr::pa::stream* _stream;
			tvr::pa::stream::player* _player;
			char _message[20];
			tvr::pa::host_api_info& _host;
			PaStreamParameters& _params;
			tvr::pa::device_info& _device;
			void_fn _finished_fn;
			voices _voices;
			PaTime _time_started;
			float _maxed_value;
			float _adjustment;
			float _vol;
			bool _playing;
		};
	}
}

#endif // TONE_PLAYER_HPP
