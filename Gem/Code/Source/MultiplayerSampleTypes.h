/*
 * Copyright (c) Contributors to the Open 3D Engine Project. For complete copyright and license terms please see the LICENSE at the root of this distribution.
 * 
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzNetworking/Utilities/QuantizedValues.h>
#include <AzNetworking/DataStructures/FixedSizeBitset.h>

namespace MultiplayerSample
{
    using StickAxis = AzNetworking::QuantizedValues<1, 1, -1, 1>;
    using MouseAxis = AzNetworking::QuantizedValues<1, 2, -1, 1>;

    //! Various character animation states.
    enum class CharacterAnimState
    {
        Idle,
        Sprinting,
        Crouching,
        Jumping,
        Falling,
        Landing,
        Climbing,
        Aiming,
        Shooting,
        Hit,
        Dying,
        MAX
    };
    using CharacterAnimStateBitset = AzNetworking::FixedSizeBitset<static_cast<AZStd::size_t>(CharacterAnimState::MAX)>;

    enum class Action
    {
        Default,
        Strafing,
        Sprinting,
        Jumping,
        Crouching,
        COUNT = Crouching + 1
    };

    using RoundTimeSec = AzNetworking::QuantizedValues<1, 2, 0, 3600>; // 1 hour max round duration

    struct PlayerState
    {
        AZStd::string m_playerName;
        uint32_t m_score;          // coins collected
        uint8_t m_remainingSheild; // % of shield left, max of ~200% allowed for buffs
        bool operator!=(const PlayerState& rhs) const;
        bool Serialize(AzNetworking::ISerializer& serializer);
    };

    inline bool PlayerState::Serialize(AzNetworking::ISerializer& serializer)
    {
        return serializer.Serialize(m_playerName, "playerName")
            && serializer.Serialize(m_score, "score") 
            && serializer.Serialize(m_remainingSheild, "remainingSheild");
    }

    inline bool PlayerState::operator!=(const PlayerState& rhs) const
    {
        return m_playerName != rhs.m_playerName
            || m_score != rhs.m_score
            || m_remainingSheild != rhs.m_remainingSheild;
    }
    
    struct MatchResults
    {
        AZStd::string m_winningPlayerName;
        AZStd::vector<PlayerState> m_playerStates;
        bool operator!=(const MatchResults& rhs) const;
        bool Serialize(AzNetworking::ISerializer& serializer);
    };

    inline bool MatchResults::Serialize(AzNetworking::ISerializer& serializer)
    {
        return serializer.Serialize(m_winningPlayerName, "winningPlayerName")
            && serializer.Serialize(m_playerStates, "playerStates");
    }

    inline bool MatchResults::operator!=(const MatchResults& rhs) const
    {
        return m_winningPlayerName != rhs.m_winningPlayerName;
    }
}

namespace AZ
{
    AZ_TYPE_INFO_SPECIALIZE(MultiplayerSample::CharacterAnimState, "{2DC36B4D-3B14-45A8-911A-60F8732F6A88}");
    AZ_TYPE_INFO_SPECIALIZE(MultiplayerSample::Action, "{1BFDEBD3-ED36-465D-BFA0-9160CFB24F37}");
}
