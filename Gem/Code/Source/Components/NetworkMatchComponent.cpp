/*
 * Copyright (c) Contributors to the Open 3D Engine Project. For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <MultiplayerSampleTypes.h>
#include <UiGameOverBus.h>
#include <GameState/GameStateMatchEnded.h>
#include <GameState/GameStateMatchInProgress.h>
#include <GameState/GameStatePreparingMatch.h>
#include <Source/Components/Multiplayer/MatchPlayerCoinsComponent.h>
#include <Source/Components/Multiplayer/PlayerIdentityComponent.h>
#include <Source/Components/NetworkTeleportCompatibleComponent.h>
#include <Source/Components/NetworkHealthComponent.h>
#include <Source/Components/NetworkMatchComponent.h>
#include <Source/Spawners/IPlayerSpawner.h>
#include <GameState/GameStateRequestBus.h>
#include <GameState/GameStateWaitingForPlayers.h>

namespace MultiplayerSample
{
    void NetworkMatchComponent::Reflect(AZ::ReflectContext* context)
    {
        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (serializeContext)
        {
            serializeContext->Class<NetworkMatchComponent, NetworkMatchComponentBase>()
                ->Version(1);
        }
        NetworkMatchComponentBase::Reflect(context);
    }

    void NetworkMatchComponent::OnActivate([[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
        if (IsNetEntityRoleAuthority() || IsNetEntityRoleServer())
        {
            PlayerIdentityNotificationBus::Handler::BusConnect();
            PlayerMatchLifecycleBus::Handler::BusConnect();
        }
    }

    void NetworkMatchComponent::OnDeactivate([[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
        PlayerMatchLifecycleBus::Handler::BusDisconnect();
        PlayerIdentityNotificationBus::Handler::BusDisconnect();
    }

    void NetworkMatchComponent::OnPlayerActivated(Multiplayer::NetEntityId playerEntity)
    {
        RPC_PlayerActivated(playerEntity);
    }

    void NetworkMatchComponent::OnPlayerDeactivated(Multiplayer::NetEntityId playerEntity)
    {
        RPC_PlayerDeactivated(playerEntity);
    }

    void NetworkMatchComponent::OnPlayerArmorZero(Multiplayer::NetEntityId playerEntity)
    {
        AZ_TracePrintf("NetworkMatchComponent", "call OnPlayerArmorZero for player: %f\n", playerEntity);
        RPC_PlayerArmorZero(playerEntity);
    }

    void NetworkMatchComponent::HandleRPC_EndMatch(
        [[maybe_unused]] AzNetworking::IConnection* invokingConnection, [[maybe_unused]] const MatchResultsSummary& results)
    {
        if (IsNetEntityRoleClient())
        {
            UiGameOverBus::Broadcast(&UiGameOverBus::Events::SetGameOverScreenEnabled, true);
            UiGameOverBus::Broadcast(&UiGameOverBus::Events::DisplayResults, results);
        }
    }

    // Controller methods

    NetworkMatchComponentController::NetworkMatchComponentController(NetworkMatchComponent& parent)
        : NetworkMatchComponentControllerBase(parent)
    {
    }

    void NetworkMatchComponentController::OnActivate([[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
        GameState::GameStateRequests::AddGameStateFactoryOverrideForType<GameStateWaitingForPlayers>([this]()
            {
                return AZStd::make_shared<GameStateWaitingForPlayers>(this);
            });
        GameState::GameStateRequests::AddGameStateFactoryOverrideForType<GameStatePreparingMatch>([this]()
            {
                return AZStd::make_shared<GameStatePreparingMatch>(this);
            });
        GameState::GameStateRequests::AddGameStateFactoryOverrideForType<GameStateMatchInProgress>([this]()
            {
                return AZStd::make_shared<GameStateMatchInProgress>(this);
            });
        GameState::GameStateRequests::AddGameStateFactoryOverrideForType<GameStateMatchEnded>([this]()
            {
                return AZStd::make_shared<GameStateMatchEnded>(this);
            });

        GameState::GameStateRequests::CreateAndPushNewOverridableGameStateOfType<GameStateWaitingForPlayers>();

        
    }

    void NetworkMatchComponentController::OnDeactivate([[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {

        GameState::GameStateRequestBus::Broadcast(&GameState::GameStateRequestBus::Events::PopAllGameStates);

        GameState::GameStateRequests::RemoveGameStateFactoryOverrideForType<GameStateWaitingForPlayers>();
        GameState::GameStateRequests::RemoveGameStateFactoryOverrideForType<GameStatePreparingMatch>();
        GameState::GameStateRequests::RemoveGameStateFactoryOverrideForType<GameStateMatchInProgress>();
        GameState::GameStateRequests::RemoveGameStateFactoryOverrideForType<GameStateMatchEnded>();

        m_roundTickEvent.RemoveFromQueue();
    }

    

    void NetworkMatchComponentController::StartMatch()
    {
        SetRoundTime(RoundTimeSec{ GetRoundDuration() });
        SetRoundNumber(1);

        // Tick once a second, this way we can keep the time as an 2 byte integer instead of a float.
        m_roundTickEvent.Enqueue(AZ::TimeMs{ 1000 }, true);
    }

    void NetworkMatchComponentController::EndMatch()
    {
        //Signal event to end the match
        m_roundTickEvent.RemoveFromQueue();

        MatchResultsSummary results;

        const AZStd::vector<PlayerCoinState>& coinStates = GetMatchPlayerCoinsComponentController()->GetParent().
            GetPlayerCoinCounts();

        int highestCoins = -1;

        for (const Multiplayer::NetEntityId playerNetEntity : m_players)
        {
            PlayerState state;
            const auto playerHandle = Multiplayer::GetNetworkEntityManager()->GetEntity(playerNetEntity);
            if (playerHandle.Exists())
            {
                if (PlayerIdentityComponent* identity = playerHandle.GetEntity()->FindComponent<PlayerIdentityComponent>())
                {
                    state.m_playerName = identity->GetPlayerName();
                    RespawnPlayer(playerNetEntity, PlayerResetOptions{ true, 100 });
                }
                if (const NetworkHealthComponent* armor = playerHandle.GetEntity()->FindComponent<NetworkHealthComponent>())
                {
                    // Treating health as armor
                    state.m_remainingShield = aznumeric_cast<uint8_t>(armor->GetHealth());
                }
            }

            const auto coinStateIterator = AZStd::find_if(coinStates.begin(), coinStates.end(), [playerNetEntity](const PlayerCoinState& state)
                {
                    return state.m_playerId == playerNetEntity;
                });
            if (coinStateIterator != coinStates.end())
            {
                state.m_score = coinStateIterator->m_coins;
                if (highestCoins < aznumeric_cast<int>(state.m_score))
                {
                    highestCoins = aznumeric_cast<int>(state.m_score);
                    results.m_winningPlayerName = state.m_playerName;
                }
            }

            results.m_playerStates.push_back(state);
        }

        RPC_EndMatch(results);
    }

    void NetworkMatchComponentController::EndRound()
    {
        ModifyRoundNumber()++;
        SetRoundTime(RoundTimeSec{ GetRoundDuration() });
    }

    void NetworkMatchComponentController::HandleRPC_PlayerActivated([[maybe_unused]] AzNetworking::IConnection* invokingConnection,
        const Multiplayer::NetEntityId& playerEntity)
    {
        const auto playerIterator = AZStd::find(m_players.begin(), m_players.end(), playerEntity);
        if (playerIterator == m_players.end())
        {
            m_players.push_back(playerEntity);
            AssignPlayerIdentity(playerEntity);
        }
    }

    void NetworkMatchComponentController::HandleRPC_PlayerDeactivated([[maybe_unused]] AzNetworking::IConnection* invokingConnection,
        const Multiplayer::NetEntityId& playerEntity)
    {
        const auto playerIterator = AZStd::find(m_players.begin(), m_players.end(), playerEntity);
        if (playerIterator != m_players.end())
        {
            m_players.erase(playerIterator);
        }
        else
        {
            AZ_Warning("NetworkMatchComponentController", false, "An unknown player deactivated %llu", aznumeric_cast<AZ::u64>(playerEntity));
        }
    }

    void NetworkMatchComponentController::HandleRPC_PlayerArmorZero(
        [[maybe_unused]] AzNetworking::IConnection* invokingConnection, const Multiplayer::NetEntityId& playerEntity)
    {
        AZ_TracePrintf("NetworkMatchComponentController", "HandleRPC_PlayerArmorZero for player: %f\n", playerEntity);
        const auto playerIterator = AZStd::find(m_players.begin(), m_players.end(), playerEntity);
        const auto playerHandle = Multiplayer::GetNetworkEntityManager()->GetEntity(playerEntity);
        if ((playerIterator != m_players.end()) && playerHandle.Exists())
        {
            RespawnPlayer(playerEntity, PlayerResetOptions{ true, GetRespawnPenaltyPercent() });
        }
        else
        {
            AZ_Warning("NetworkMatchComponentController", false, "An unknown player reported depleted armor: %llu", aznumeric_cast<AZ::u64>(playerEntity));
        }
    }

    void NetworkMatchComponentController::RoundTickOnceASecond()
    {
        // m_roundTickEvent is configured to tick once a second
        SetRoundTime(RoundTimeSec(GetRoundTime() - 1.f));

        if (GetRoundTime() <= RoundTimeSec(0.f))
        {
            EndRound();
        }
    }

    void NetworkMatchComponentController::AssignPlayerIdentity(Multiplayer::NetEntityId playerEntity)
    {
        const Multiplayer::ConstNetworkEntityHandle entityHandle = Multiplayer::GetNetworkEntityManager()->GetEntity(playerEntity);
        if (entityHandle.Exists())
        {
            if (PlayerIdentityComponent* identity = entityHandle.GetEntity()->FindComponent<PlayerIdentityComponent>())
            {
                identity->AssignPlayerName(PlayerNameString::format("Player %d", m_nextPlayerId));
            }
            else
            {
                AZ_Warning("NetworkMatchComponentController", false, "Player entity did not have PlayerIdentityComponent");
            }
        }

        m_nextPlayerId++;
    }

    void NetworkMatchComponentController::RespawnPlayer(Multiplayer::NetEntityId playerEntity, PlayerResetOptions resets)
    {
        const auto playerHandle = Multiplayer::GetNetworkEntityManager()->GetEntity(playerEntity);
        if (playerHandle.Exists())
        {
            // reset state
            if (PlayerIdentityComponent* identity = playerHandle.GetEntity()->FindComponent<PlayerIdentityComponent>())
            {
                identity->RPC_ResetPlayerState(resets);
            }

            // move to valid respawn point
            if (NetworkTeleportCompatibleComponent* teleport = playerHandle.GetEntity()->FindComponent<NetworkTeleportCompatibleComponent>())
            {
                AZStd::pair<Multiplayer::PrefabEntityId, AZ::Transform> entityParams = 
                    AZ::Interface<IPlayerSpawner>::Get()->GetNextPlayerSpawn();

                teleport->Teleport(entityParams.second.GetTranslation());
            }
        }
        else
        {
            AZ_Warning("NetworkMatchComponentController", false, "Attempted respawn of an unknown player: %llu", aznumeric_cast<AZ::u64>(playerEntity));
        }
    }

}
