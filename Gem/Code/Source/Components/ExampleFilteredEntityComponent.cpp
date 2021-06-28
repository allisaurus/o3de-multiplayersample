/*
 * Copyright (c) Contributors to the Open 3D Engine Project
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <AzCore/Serialization/EditContext.h>
#include <Components/ExampleFilteredEntityComponent.h>

AZ_CVAR(bool, mps_EnableFilteringEntities, true, nullptr, AZ::ConsoleFunctorFlags::Null, "If true, enables the example of filtering entities");

namespace MultiplayerSample
{
    void ExampleFilteredEntityComponent::Reflect(AZ::ReflectContext* context)
    {
        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (serializeContext)
        {
            serializeContext->Class<ExampleFilteredEntityComponent, AZ::Component>()
                ->Field( "Enabled", &ExampleFilteredEntityComponent::m_enabled )
                ->Version(3);

            if (AZ::EditContext* ptrEdit = serializeContext->GetEditContext())
            {
                using namespace AZ::Edit;
                ptrEdit->Class<ExampleFilteredEntityComponent>("ExampleFilteredEntityComponent", "An example of filtering entities out in network replication")
                    ->ClassElement(ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "MultiplayerSample")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Level"))
                    ->DataElement(nullptr, &ExampleFilteredEntityComponent::m_enabled, "Enabled", "enabled if checked")
                ;
            }
        }
    }

    void ExampleFilteredEntityComponent::Activate()
    {
        Multiplayer::GetMultiplayer()->SetFilterEntityManager( this );
    }

    void ExampleFilteredEntityComponent::Deactivate()
    {
        Multiplayer::GetMultiplayer()->SetFilterEntityManager( nullptr );
    }

    bool ExampleFilteredEntityComponent::IsEntityFiltered(
        [[maybe_unused]] AZ::Entity* entity,
        [[maybe_unused]] Multiplayer::ConstNetworkEntityHandle controllerEntity,
        [[maybe_unused]] AzNetworking::ConnectionId connectionId)
    {
        if (m_enabled && mps_EnableFilteringEntities)
        {
            // Note: @IsEntityFiltered is a hot code path, so do your best to optimize this method.
            // This example just uses entity names for filtering, for the sake of simplicity.
            // In practice, one might implement this lookup using a specialized structure such as AZStd::map<AZ::EntityId, bool>, etc.

            const bool evenConnectionId = static_cast<uint32_t>(connectionId) % 2 == 0;

            if (entity->GetName().starts_with( evenConnectionId ? "Filter Even" : "Filter Odd" ))
            {
                return true;
            }
        }

        return false;
    }
}
