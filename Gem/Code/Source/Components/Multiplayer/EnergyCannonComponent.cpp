/*
 * Copyright (c) Contributors to the Open 3D Engine Project. For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <AzCore/Component/ComponentApplicationBus.h>
#include <AzCore/Component/TransformBus.h>
#include <Source/Components/Multiplayer/EnergyBallComponent.h>
#include <Source/Components/Multiplayer/EnergyCannonComponent.h>

namespace MultiplayerSample
{
    EnergyCannonComponentController::EnergyCannonComponentController(EnergyCannonComponent& parent)
        : EnergyCannonComponentControllerBase(parent)
    {
    }

    void EnergyCannonComponentController::OnActivate([[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
        m_firingEvent.Enqueue(GetFiringPeriod(), true);
    }

    void EnergyCannonComponentController::OnDeactivate([[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
        m_firingEvent.RemoveFromQueue();
    }

    void EnergyCannonComponentController::OnFireEnergyBall()
    {
        // Re-using the same ball entity.
        AZ::Entity* ball = nullptr;
        AZ::ComponentApplicationBus::BroadcastResult(ball,
            &AZ::ComponentApplicationBus::Events::FindEntity, GetEnergyBallEntity());
        if (ball)
        {
            if (EnergyBallComponent* ballComponent = ball->FindComponent<EnergyBallComponent>())
            {
                const AZ::Transform& cannonTm = GetEntity()->GetTransform()->GetWorldTM();
                const AZ::Vector3 forward = cannonTm.TransformVector(AZ::Vector3::CreateAxisY(-1.f));
                ballComponent->RPC_LaunchBall(cannonTm.GetTranslation(), forward);
            }
        }
    }
}