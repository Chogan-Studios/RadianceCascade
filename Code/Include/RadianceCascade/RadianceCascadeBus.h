
#pragma once

#include <RadianceCascade/RadianceCascadeTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace RadianceCascade
{
    class RadianceCascadeRequests
    {
    public:
        AZ_RTTI(RadianceCascadeRequests, RadianceCascadeRequestsTypeId);
        virtual ~RadianceCascadeRequests() = default;
        // Put your public methods here
    };

    class RadianceCascadeBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using RadianceCascadeRequestBus = AZ::EBus<RadianceCascadeRequests, RadianceCascadeBusTraits>;
    using RadianceCascadeInterface = AZ::Interface<RadianceCascadeRequests>;

} // namespace RadianceCascade
