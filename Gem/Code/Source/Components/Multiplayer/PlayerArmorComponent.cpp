/*
 * Copyright (c) Contributors to the Open 3D Engine Project. For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <UiPlayerArmorBus.h>
#include <Components/NetworkHealthComponent.h>
#include <Source/Components/Multiplayer/PlayerArmorComponent.h>

namespace MultiplayerSample
{
    void PlayerArmorComponent::Reflect(AZ::ReflectContext* context)
    {
        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (serializeContext)
        {
            serializeContext->Class<PlayerArmorComponent, PlayerArmorComponentBase>()
                ->Version(1);
        }
        PlayerArmorComponentBase::Reflect(context);
    }

    void PlayerArmorComponent::OnInit()
    {
    }

    void PlayerArmorComponent::OnActivate([[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
    }

    void PlayerArmorComponent::OnDeactivate([[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
    }

    void PlayerArmorComponent::SignalArmorZeroEvent()
    {
        AZ_TracePrintf("PlayerArmorComponent", "SignalArmorZeroEvent was invoked! \n", "...");
        m_armorZeroEvent.Signal(5.0f);
    }

    void PlayerArmorComponent::BindArmorZeroEventHandler(ArmorZeroEvent::Handler handler)
    {
        AZ_TracePrintf("PlayerArmorComponent", "...BindArmorZeroEventHandler called!\n", "...");
        handler.Connect(m_armorZeroEvent);
    }

    // controller methods

    PlayerArmorComponentController::PlayerArmorComponentController(PlayerArmorComponent& parent)
        : PlayerArmorComponentControllerBase(parent)
    {
    }

    void PlayerArmorComponentController::OnActivate([[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
        if (IsAutonomous())
        {
            GetNetworkHealthComponentController()->GetParent().HealthAddEvent(m_changedHandler);
        }

        if (IsAuthority())
        {
            GetNetworkHealthComponentController()->SetHealth(GetStartingArmor());
        }
    }

    void PlayerArmorComponentController::OnDeactivate([[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
        m_changedHandler.Disconnect();
    }

    void PlayerArmorComponentController::OnAmountChanged(float armor)
    {
        UiPlayerArmorNotificationBus::Broadcast(&UiPlayerArmorNotificationBus::Events::OnPlayerArmorChanged, armor, GetStartingArmor());
        if (armor <= 0)
        {
            AZ_TracePrintf("PlayerArmorComponentController", "ARMOR is below zero...\n", "...");
            GetParent().SignalArmorZeroEvent();
        }
    }
}
