#if !defined STREAM_HPP
#define STREAM_HPP

namespace tvr
{
	namespace pa
	{
		class stream
		{
		public:
			struct player
			{
				player(stream& stream): _stream(stream)
				{
					_error = ::Pa_StartStream(_stream.get_stream());
				}

				~player()
				{
					_error = ::Pa_AbortStream(_stream.get_stream());
				}

				void stop()
				{
					_error = ::Pa_StopStream(_stream.get_stream());
				}

				void start()
				{
					_error = ::Pa_StartStream(_stream.get_stream());
				}
			private:
				PaError _error;
				stream& _stream;
			};
		public:
			stream(): _s(0) {}
			stream(const PaStreamParameters* in,
				const PaStreamParameters* out,
				double sampleRate,
				unsigned long framesPerBuffer,
				PaStreamFlags flags,
				PaStreamCallback* callback,
				void* userData) : _s(0)
			{
				open(in, out, sampleRate, framesPerBuffer, flags, callback, userData);
			}
			stream(int numIn,
				int numOut,
				PaSampleFormat sampleFormat,
				double sampleRate,
				unsigned long framesPerBuffer,
				PaStreamCallback* streamCallback,
				void* userData) : _s(0)
			{
				open(numIn, numOut, sampleFormat, sampleRate, framesPerBuffer, streamCallback, userData);
			}

			~stream()
			{
				close();
			}

			void open(const PaStreamParameters* in,
				const PaStreamParameters* out,
				double sampleRate,
				unsigned long framesPerBuffer,
				PaStreamFlags flags,
				PaStreamCallback* callback,
				void* userData
				)
			{
				_error = ::Pa_OpenStream(&_s, in, out, sampleRate, framesPerBuffer, flags, callback, userData);
			}

			void open(int numIn,
				int numOut,
				PaSampleFormat sampleFormat,
				double sampleRate,
				unsigned long framesPerBuffer,
				PaStreamCallback* streamCallback,
				void* userData)
			{
				_error = ::Pa_OpenDefaultStream(&_s, numIn, numOut, sampleFormat, sampleRate, framesPerBuffer, streamCallback, userData);
			}

			void close()
			{
				if (_s != 0)
				{
					_error = ::Pa_CloseStream(_s);
				}
			}

			PaError get_error() const
			{
				return _error;
			}

			double get_cpu_load()
			{
				return ::Pa_GetStreamCpuLoad(_s);
			}

			const PaStreamInfo* get_info()
			{
				return ::Pa_GetStreamInfo(_s);
			}

			PaTime get_time()
			{
				return ::Pa_GetStreamTime(_s);
			}

			signed long get_write_count()
			{
				return ::Pa_GetStreamWriteAvailable(_s);
			}

			PaError write(const void* buffer, unsigned long frames)
			{
				_error = ::Pa_WriteStream(_s, buffer, frames);
				return _error;
			}

			signed long get_read_count()
			{
				return ::Pa_GetStreamReadAvailable(_s);
			}

			PaError read(void* buffer, unsigned long frames)
			{
				_error = ::Pa_ReadStream(_s, buffer, frames);
				return _error;
			}

			void set_finished_callback(PaStreamFinishedCallback* callback)
			{
				_error = ::Pa_SetStreamFinishedCallback(_s, callback);
			}

			bool is_active()
			{
				bool result = false;
				PaError test = ::Pa_IsStreamActive(_s);
				if (test == 1)
					result = true;
				else if (test < 0)
					_error = test;
				return result;
			}

			bool is_stopped()
			{
				bool result = false;
				PaError test = ::Pa_IsStreamStopped(_s);
				if (test == 1)
					result = true;
				else if (test < 0)
					_error = test;
				return result;
			}

			PaStream* get_stream()
			{
				return _s;
			}

		private:
			PaStream* _s;
			PaError _error;
		};
	}
}

#endif // STREAM_HPP
