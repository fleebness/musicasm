#if !defined TONE_PLAYER_HPP
#define TONE_PLAYER_HPP

#include <functional>
#include <memory>
#include <vector>

#include "port_audio.hpp"
#include "wave.hpp"

namespace tvr
{
	namespace pa
	{
		class tone_player
		{
		public:
			typedef std::function<void()> void_fn;
			typedef std::shared_ptr<_wave> wave_ptr;
			typedef std::vector< wave_ptr > wave_ptr_collection;

		public:
			tone_player(tvr::pa::host_api_info& host, tvr::pa::device_info& device, PaStreamParameters& params) :
				_host(host),
				_device(device),
				_params(params),
				_stream(0),
				_maxed_value(1.0),
				_vol(1.0)
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
				}
			}

			void stop()
			{
				if (_player)
				{
					_player->stop();
				}
			}

			void abort()
			{
				if (_player)
				{
					delete _player;
					_player = 0;
				}
			}

			void set_finished_fn(void_fn fn)
			{
				_finished_fn = fn;
			}

			wave_ptr_collection& get_waves()
			{
				return _waves;
			}

			const wave_ptr_collection& get_waves() const
			{
				return _waves;
			}

			void add_wave(wave_ptr wave)
			{
				_waves.push_back(wave);
			}

			void clear_waves()
			{
				_waves.clear();
				_maxed_value = 1.0;
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
				for (unsigned long c = 0; c < frame; ++c)
				{
					float value = 0.0;
					for (std::size_t i = 0; i < _waves.size(); ++i)
					{
						value += _waves[i].get()->get_next_value();
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
			wave_ptr_collection _waves;
			float _maxed_value;
			float _adjustment;
			float _vol;
		};

	}
}

#endif // TONE_PLAYER_HPP
