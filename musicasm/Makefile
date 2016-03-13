CC = g++
DEBUG = -g
CFLAGS = -Wall -C -DTVR_POSIX $(DEBUG)
LFLAGS = -Wall $(DEBUG) -lportaudio -lrt -lm -lasound -pthread
HEADERS = adsr.hpp base_note.hpp console_wait.hpp device_info.hpp equal_temperament.hpp host_api.hpp port_audio.hpp scale.hpp stream.hpp temperament.hpp tone_player.hpp voice.hpp wave.hpp frequency.hpp amplitude.hpp

musicasm : musicasm.cpp $(HEADERS)
	$(CC) -std=c++11 musicasm.cpp $(CFLAGS) $(LFLAGS) $(OBJS) -o musicasm

clean :
	rm musicasm

