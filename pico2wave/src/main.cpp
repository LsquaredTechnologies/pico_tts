#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <TtsEngine.h> // TTS engine
#include <sndfile.hh>  // Audio output
#include <getopt.h>

#define OUTPUT_BUFFER_SIZE (128 * 1024)

static SndfileHandle wave;
static bool synthesis_complete = false;

static void usage(void);

using namespace android;
tts_callback_status synth_done(void *&userdata, uint32_t sample_rate, tts_audio_format audio_format, int channels, int8_t *&data, size_t &size, tts_synth_status status);

const int channels = 1;
const int rate = 16000;

int main(int argc, char *argv[])
{
	uint8_t *synthBuffer;
	char *synthInput = NULL;

	tts_result result;
	TtsEngine *ttsEngine = getTtsEngine();

	if (argc == 1)
		usage();

	const int none = 0;
	const int required = 1;
	const int optional = 2;

	char *outputFilename = NULL;
	char *inputLang = NULL;
	outputFilename = (char *)"/dev/stdout";
	inputLang = (char *)"en-US";

	int show_help, show_usage; /* flag variables */
	struct option longopts[] = {
		{"file", optional, NULL, 'f'},
		{"help", none, &show_help, 1},
		{"lang", optional, NULL, 'l'},
		{"usage", none, &show_usage, 1},
		{0, 0, 0, 0}};
	int c;
	while ((c = getopt_long(argc, argv, ":f:l:", longopts, NULL)) != -1)
	{
		switch (c)
		{
		case 'f':
			outputFilename = optarg;
#if DEBUG
			fprintf(stderr, "[DEBUG] Audio set to '%s'.\n", outputFilename);
#endif
			break;
		case 'l':
			inputLang = optarg;
#if DEBUG
			fprintf(stderr, "[DEBUG] Input language set to '%s'.\n", inputLang);
#endif
			break;
		}
	}

	if (show_help == 1 || show_usage == 1)
	{
		usage();
	}

	if (optind < argc)
	{
		synthInput = argv[optind];
	}
	if (!synthInput)
	{
#if DEBUG
		fprintf(stderr, "[ERROR] No input string found.\n");
#endif
		usage();
	}
#if DEBUG
	fprintf(stderr, "[DEBUG] Input string: \"%s\".\n", synthInput);
#endif

	synthBuffer = new uint8_t[OUTPUT_BUFFER_SIZE];

	result = ttsEngine->init(synth_done, "/usr/share/pico/lang/");
	if (result != TTS_SUCCESS)
	{
#if DEBUG
		fprintf(stderr, "[DEBUG] Failed to init TTS engine.\n");
#endif
		exit(-1);
	}

	int len = strlen(inputLang);
	if (len == 2)
	{
		result = ttsEngine->setLanguage(inputLang, "", "");
	}
	else if (len == 5)
	{
		char lang[3];
		char country[3];
		strncpy(lang, inputLang, 2);
		strncpy(country, inputLang + 3, 2);
		lang[2] = 0;
		country[2] = 0;
		result = ttsEngine->setLanguage(lang, country, "");
	}
	else
	{
#if DEBUG
		fprintf(stderr, "[DEBUG] Unknown language: \"%s\". Load default one.\n", inputLang);
#endif
		result = ttsEngine->setLanguage("en", "US", "");
	}

	if (result != TTS_SUCCESS)
	{
#if DEBUG
		fprintf(stderr, "[DEBUG] Failed to load language. Load default one.\n");
#endif
		result = ttsEngine->setLanguage("en", "US", "");
		if (result != TTS_SUCCESS)
		{
#if DEBUG
			fprintf(stderr, "[ERROR] Cannot load default language.\n");
#endif
			exit(-4);
		}
		else
		{
#if DEBUG
			fprintf(stderr, "[DEBUG] Default language loaded.\n");
#endif
		}
	}
	else
	{
#if DEBUG
		fprintf(stderr, "[DEBUG] Language loaded.\n");
#endif
	}

#if DEBUG
	fprintf(stderr, "[DEBUG] Check audio file format.\n");
#endif

	int format = SF_FORMAT_PCM_16 | SF_FORMAT_WAV | SF_ENDIAN_LITTLE;
#if false
	const char *ext = strrchr(outputFilename, '.');
	if (ext)
	{
		if (ext[1] == 'w' || ext[1] == 'W')
		{
			if (ext[2] == 'a' || ext[2] == 'A')
			{
				if (ext[3] == 'v' || ext[3] == 'V')
				{
					format = SF_FORMAT_PCM_16 | SF_FORMAT_WAV | SF_ENDIAN_LITTLE;
#if DEBUG
					fprintf(stderr, "[DEBUG] Set format to WAVE.\n");
#endif
				}
			}
		}
		else if (ext[1] == 'a' || ext[1] == 'A')
		{
			if (ext[2] == 'u' || ext[2] == 'U')
			{
				format = SF_FORMAT_AU | SF_ENDIAN_BIG | SF_FORMAT_FLOAT;
#if DEBUG
				fprintf(stderr, "[DEBUG] Set format to AU.\n");
#endif
			}
			else if (ext[2] == 'i' || ext[2] == 'I')
			{
				if (ext[3] == 'f' || ext[3] == 'F')
				{
					format = SF_FORMAT_AIFF | SF_ENDIAN_BIG;
#if DEBUG
					fprintf(stderr, "[DEBUG] Set format to AIFF.\n");
#endif
				}
			}
		}
		else if (ext[1] == 'o' || ext[1] == 'O')
		{
			if (ext[2] == 'g' || ext[2] == 'G')
			{
				if (ext[3] == 'g' || ext[3] == 'G' || ext[3] == 'a' || ext[3] == 'A')
				{
					format = SF_FORMAT_OGG | SF_ENDIAN_LITTLE;
#if DEBUG
					fprintf(stderr, "[DEBUG] Set format to OGG.\n");
#endif
				}
			}
		}
		else if (ext[1] == 'f' || ext[1] == 'F')
		{
			if (ext[2] == 'l' || ext[2] == 'L')
			{
				if (ext[3] == 'a' || ext[3] == 'A')
				{
					format = SF_FORMAT_PCM_16 | SF_FORMAT_FLAC | SF_ENDIAN_LITTLE;
#if DEBUG
					fprintf(stderr, "[DEBUG] Set format to FLAC.\n");
#endif
				}
			}
		}
		else
		{
			format = SF_FORMAT_PCM_16 | SF_FORMAT_WAV | SF_ENDIAN_LITTLE;
#if DEBUG
			fprintf(stderr, "[DEBUG] No file specified. Set format to WAVE.\n");
#endif
		}
	}
	else
	{
		format = SF_FORMAT_PCM_16 | SF_FORMAT_WAV | SF_ENDIAN_LITTLE;
#if DEBUG
		fprintf(stderr, "[DEBUG] No file specified. Set format to WAVE.\n");
#endif
	}
#endif

	wave = SndfileHandle(outputFilename, SFM_WRITE, format, channels, rate);
	if (!wave)
	{
		fprintf(stderr, "[ERROR] Cannot create file.\n");
		exit(-2);
	}

#if DEBUG
	fprintf(stderr, "[DEBUG] Synthesising text...\n");
#endif

	result = ttsEngine->synthesizeText(synthInput, synthBuffer, OUTPUT_BUFFER_SIZE, NULL);
	if (result != TTS_SUCCESS)
	{
#if DEBUG
		fprintf(stderr, "[ERROR] Failed to synth text.\n");
#endif
		exit(-3);
	}

	while (!synthesis_complete)
	{
		usleep(100);
	}

#if DEBUG
	fprintf(stderr, "[DEBUG] Completed.\n");
#endif

	// if (outputFilename)
	// {
	// 	fclose(outfp);
	// }

	// result = ttsEngine->shutdown();
	// if (result != TTS_SUCCESS)
	// {
	// 	fprintf(stderr, "Failed to shutdown TTS.\n");
	// }

	delete[] synthBuffer;

	return 0;
}

static void usage(void)
{
	fprintf(stderr, "Usage: pico <words>\n"
					"  -f, --file=<filename>         Write output to this file\n"
					"  -l, --lang=<lang>             Language (default: \"en-US\")\n"
					"\nHelp options:\n"
					"  -?, --help                    Show this help message\n"
					"      --usage                   Display brief usage message\n");
	exit(0);
}

// @param [inout] void *&       - The userdata pointer set in the original
//                                 synth call
// @param [in]    uint32_t      - Track sampling rate in Hz
// @param [in] tts_audio_format - The audio format
// @param [in]    int           - The number of channels
// @param [inout] int8_t *&     - A buffer of audio data only valid during the
//                                execution of the callback
// @param [inout] size_t  &     - The size of the buffer
// @param [in] tts_synth_status - indicate whether the synthesis is done, or
//                                 if more data is to be synthesized.
// @return TTS_CALLBACK_HALT to indicate the synthesis must stop,
//         TTS_CALLBACK_CONTINUE to indicate the synthesis must continue if
//            there is more data to produce.
tts_callback_status synth_done(void *&userdata, uint32_t sample_rate, tts_audio_format audio_format, int channels, int8_t *&data, size_t &size, tts_synth_status status)
{
#if DEBUG
	fprintf(stderr, "TTS callback, rate: %d, data size: %ld, status: %i\n", sample_rate, size, status);
#endif

	if (status == TTS_SYNTH_DONE)
	{
		synthesis_complete = true;
	}

	if ((size == OUTPUT_BUFFER_SIZE) || (status == TTS_SYNTH_DONE))
	{
		wave.writeRaw((const void *)data, (sf_count_t)size);
	}

	return TTS_CALLBACK_CONTINUE;
}
