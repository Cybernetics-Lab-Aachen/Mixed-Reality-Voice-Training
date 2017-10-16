// Fill out your copyright notice in the Description page of Project Settings.

#include "voice_training.h"
#include "ConvolutionReverb.h"


// Sets default values
AConvolutionReverb::AConvolutionReverb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AConvolutionReverb::BeginPlay()
{
	Super::BeginPlay();
	
	Init();
}

// Called every frame
void AConvolutionReverb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Update();
}

// Initialize FMOD
void AConvolutionReverb::Init()
{
	if (IFMODStudioModule::IsAvailable())
	{
		FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
		if (StudioSystem)
		{
			StudioSystem->getLowLevelSystem(&LowLevelSystem);
			LowLevelSystem->init(1, FMOD_INIT_NORMAL, nullptr);

			int DriverConnected = 0;
			LowLevelSystem->getRecordNumDrivers(nullptr, &DriverConnected);


			/*
			Determine latency in samples.
			*/

			LowLevelSystem->getRecordDriverInfo(DEVICE_INDEX, nullptr, 0, nullptr, &nativeRate, nullptr, &nativeChannels, nullptr);

			driftThreshold = (nativeRate * DRIFT_MS) / 1000;       /* The point where we start compensating for drift */
			desiredLatency = (nativeRate * LATENCY_MS) / 1000;     /* User specified latency */
			adjustedLatency = desiredLatency;                      /* User specified latency adjusted for driver update granularity */
			actualLatency = desiredLatency;                        /* Latency measured once playback begins (smoothened for jitter) */


																   /*
																   Create user sound to record into, then start recording.
																   */
			FMOD_CREATESOUNDEXINFO exinfo = { 0 };
			exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
			exinfo.numchannels = nativeChannels;
			exinfo.format = FMOD_SOUND_FORMAT_PCM16;
			exinfo.defaultfrequency = nativeRate;
			exinfo.length = nativeRate * sizeof(short) * nativeChannels; /* 1 second buffer, size here doesn't change latency */

			LowLevelSystem->createSound(nullptr, FMOD_LOOP_NORMAL | FMOD_OPENUSER, &exinfo, &Sound);

			LowLevelSystem->recordStart(DEVICE_INDEX, Sound, true);

			Sound->getLength(&soundLength, FMOD_TIMEUNIT_PCM);
		}
	}
}

// Update recording and playback
void AConvolutionReverb::Update()
{
	if (Enabled && LowLevelSystem)
	{
		LowLevelSystem->update();

		/*
		Determine how much has been recorded since we last checked
		*/
		unsigned int recordPos = 0;
		LowLevelSystem->getRecordPosition(DEVICE_INDEX, &recordPos);
		
		static unsigned int lastRecordPos = 0;
		unsigned int recordDelta = (recordPos >= lastRecordPos) ? (recordPos - lastRecordPos) : (recordPos + soundLength - lastRecordPos);
		lastRecordPos = recordPos;
		samplesRecorded += recordDelta;

		static unsigned int minRecordDelta = (unsigned int)-1;
		if (recordDelta && (recordDelta < minRecordDelta))
		{
			minRecordDelta = recordDelta; /* Smallest driver granularity seen so far */
			adjustedLatency = (recordDelta <= desiredLatency) ? desiredLatency : recordDelta; /* Adjust our latency if driver granularity is high */
		}


		/*
		Delay playback until our desired latency is reached.
		*/
		if (!Channel && samplesRecorded >= adjustedLatency)
		{
			LowLevelSystem->playSound(Sound, nullptr, false, &Channel);
		}


		if (Channel)
		{

			/*
			Stop playback if recording stops.
			*/
			bool isRecording = false;
			LowLevelSystem->isRecording(DEVICE_INDEX, &isRecording);
			
			if (!isRecording)
			{
				Channel->setPaused(true);
			}


			/*
			Determine how much has been played since we last checked.
			*/
			unsigned int playPos = 0;
			Channel->getPosition(&playPos, FMOD_TIMEUNIT_PCM);
			
			static unsigned int lastPlayPos = 0;
			unsigned int playDelta = (playPos >= lastPlayPos) ? (playPos - lastPlayPos) : (playPos + soundLength - lastPlayPos);
			lastPlayPos = playPos;
			samplesPlayed += playDelta;


			/*
			Compensate for any drift.
			*/
			int latency = samplesRecorded - samplesPlayed;
			actualLatency = (0.97f * actualLatency) + (0.03f * latency);

			int playbackRate = nativeRate;
			if (actualLatency < (int)(adjustedLatency - driftThreshold))
			{
				/* Play position is catching up to the record position, slow playback down by 2% */
				playbackRate = nativeRate - (nativeRate / 50);
			}
			else if (actualLatency >(int)(adjustedLatency + driftThreshold))
			{
				/* Play position is falling behind the record position, speed playback up by 2% */
				playbackRate = nativeRate + (nativeRate / 50);
			}

			Channel->setFrequency((float)playbackRate);	
		}
	}
}