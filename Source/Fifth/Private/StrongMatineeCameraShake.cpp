// Fill out your copyright notice in the Description page of Project Settings.


#include "StrongMatineeCameraShake.h"

UStrongMatineeCameraShake::UStrongMatineeCameraShake()
{
	OscillationDuration = 2.0f;
	OscillationBlendInTime = 0.8f;
	OscillationBlendOutTime = 1.0f;

	RotOscillation.Pitch.Amplitude = FMath::RandRange(5.f, 10.f);
	RotOscillation.Pitch.Frequency = FMath::RandRange(25.f, 35.f);

	RotOscillation.Yaw.Amplitude = FMath::RandRange(5.f, 10.f);
	RotOscillation.Yaw.Frequency = FMath::RandRange(25.f, 35.f);

	RotOscillation.Roll.Amplitude = FMath::RandRange(5.f, 10.f);
	RotOscillation.Roll.Frequency = FMath::RandRange(25.f, 35.f);


}

