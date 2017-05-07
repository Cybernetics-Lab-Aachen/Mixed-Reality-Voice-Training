// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#include "FMODStudioModule.h"
#include "fmod_studio.hpp"

#include "ConvolutionReverb.generated.h"

UCLASS()
class AConvolutionReverb : public AActor
{
	GENERATED_BODY()

private:
	FMOD::System* LowLevelSystem = nullptr;
	FMOD::Sound *Sound = nullptr;
	FMOD::Channel *Channel = nullptr;

	unsigned int samplesRecorded = 0;
	unsigned int samplesPlayed = 0;

	int nativeRate = 0;
	int nativeChannels = 0;

	int LATENCY_MS = 50;
	int DRIFT_MS = 0;
	int DEVICE_INDEX = 0;

	unsigned int driftThreshold = 0;
	unsigned int desiredLatency = 0;
	unsigned int adjustedLatency = 0;
	int actualLatency = 0;

	unsigned int soundLength = 0;
public:	
	// Sets default values for this actor's properties
	AConvolutionReverb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Update();
};
