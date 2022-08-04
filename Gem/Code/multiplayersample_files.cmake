#
# Copyright (c) Contributors to the Open 3D Engine Project
#
# SPDX-License-Identifier: Apache-2.0 OR MIT
#
#

set(FILES
    Include/NetworkPrefabSpawnerInterface.h
    Source/AutoGen/NetworkAiComponent.AutoComponent.xml
    Source/AutoGen/NetworkAnimationComponent.AutoComponent.xml
    Source/AutoGen/NetworkCoinComponent.AutoComponent.xml
    Source/AutoGen/NetworkCoinSpawnerComponent.AutoComponent.xml
    Source/AutoGen/NetworkHealthComponent.AutoComponent.xml
    Source/AutoGen/NetworkMatchComponent.AutoComponent.xml
    Source/AutoGen/NetworkPlayerMovementComponent.AutoComponent.xml
    Source/AutoGen/NetworkPlayerSpawnerComponent.AutoComponent.xml
    Source/AutoGen/NetworkRandomComponent.AutoComponent.xml
    Source/AutoGen/NetworkRandomImpulseComponent.AutoComponent.xml
    Source/AutoGen/NetworkRandomTranslateComponent.AutoComponent.xml
    Source/AutoGen/NetworkSimplePlayerCameraComponent.AutoComponent.xml
    Source/AutoGen/NetworkStressTestComponent.AutoComponent.xml
    Source/AutoGen/NetworkTestSpawnerComponent.AutoComponent.xml
    Source/AutoGen/NetworkRandomImpulseComponent.AutoComponent.xml
    Source/AutoGen/NetworkRandomTranslateComponent.AutoComponent.xml
    Source/AutoGen/NetworkTeleportComponent.AutoComponent.xml
    Source/AutoGen/NetworkTeleportCompatibleComponent.AutoComponent.xml
    Source/AutoGen/NetworkWeaponsComponent.AutoComponent.xml
    Source/AutoGen/PlayerCoinCollectorComponent.AutoComponent.xml
    Source/Components/ExampleFilteredEntityComponent.h
    Source/Components/ExampleFilteredEntityComponent.cpp
    Source/Components/NetworkAiComponent.cpp
    Source/Components/NetworkAiComponent.h
    Source/Components/NetworkAnimationComponent.cpp
    Source/Components/NetworkAnimationComponent.h
    Source/Components/NetworkCoinComponent.cpp
    Source/Components/NetworkCoinComponent.h
    Source/Components/NetworkCoinSpawnerComponent.cpp
    Source/Components/NetworkCoinSpawnerComponent.h
    Source/Components/NetworkHealthComponent.cpp
    Source/Components/NetworkHealthComponent.h
    Source/Components/NetworkMatchComponent.cpp
    Source/Components/NetworkMatchComponent.h
    Source/Components/NetworkPlayerSpawnerComponent.cpp
    Source/Components/NetworkPlayerSpawnerComponent.h
    Source/Components/NetworkRandomComponent.cpp
    Source/Components/NetworkRandomComponent.h
    Source/Components/NetworkTeleportComponent.cpp
    Source/Components/NetworkTeleportComponent.h
    Source/Components/NetworkTeleportCompatibleComponent.cpp
    Source/Components/NetworkTeleportCompatibleComponent.h
    Source/Components/NetworkWeaponsComponent.cpp
    Source/Components/NetworkWeaponsComponent.h
    Source/Components/NetworkSimplePlayerCameraComponent.cpp
    Source/Components/NetworkSimplePlayerCameraComponent.h
    Source/Components/PerfTest/NetworkPrefabSpawnerComponent.cpp
    Source/Components/PerfTest/NetworkPrefabSpawnerComponent.h
    Source/Components/PerfTest/NetworkRandomImpulseComponent.cpp
    Source/Components/PerfTest/NetworkRandomImpulseComponent.h
    Source/Components/PerfTest/NetworkTestSpawnerComponent.cpp
    Source/Components/PerfTest/NetworkTestSpawnerComponent.h
    Source/Components/PerfTest/NetworkRandomTranslateComponent.cpp
    Source/Components/PerfTest/NetworkRandomTranslateComponent.h
    Source/Components/NetworkStressTestComponent.cpp
    Source/Components/NetworkStressTestComponent.h
    Source/Components/NetworkPlayerMovementComponent.cpp
    Source/Components/NetworkPlayerMovementComponent.h
    Source/Components/UI/HUDComponent.cpp
    Source/Components/UI/HUDComponent.h
    Source/Components/UI/MatchOverComponent.cpp
    Source/Components/UI/MatchOverComponent.h

    Source/Components/Multiplayer/PlayerCoinCollectorComponent.cpp
    Source/Components/Multiplayer/PlayerCoinCollectorComponent.h

    Source/Spawners/IPlayerSpawner.h
    Source/Spawners/RoundRobinSpawner.h
    Source/Spawners/RoundRobinSpawner.cpp
    Source/Weapons/BaseWeapon.cpp
    Source/Weapons/BaseWeapon.h
    Source/Weapons/IWeapon.h
    Source/Weapons/ProjectileWeapon.cpp
    Source/Weapons/ProjectileWeapon.h
    Source/Weapons/TraceWeapon.cpp
    Source/Weapons/TraceWeapon.h
    Source/Weapons/WeaponGathers.cpp
    Source/Weapons/WeaponGathers.h
    Source/Weapons/WeaponTypes.cpp
    Source/Weapons/WeaponTypes.h
    Source/Weapons/SceneQuery.cpp
    Source/Weapons/SceneQuery.h
    Source/MultiplayerSampleSystemComponent.cpp
    Source/MultiplayerSampleSystemComponent.h
    Source/MultiplayerSampleTypes.h
)
