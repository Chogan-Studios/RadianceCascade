
#pragma once

namespace RadianceCascade
{
    // System Component TypeIds
    inline constexpr const char* RadianceCascadeSystemComponentTypeId = "{88BA7F17-B661-46D0-94ED-EE1E6E916A9B}";
    inline constexpr const char* RadianceCascadeEditorSystemComponentTypeId = "{BAA70329-4AEC-431B-A90A-705FDC0CC29D}";

    // Module derived classes TypeIds
    inline constexpr const char* RadianceCascadeModuleInterfaceTypeId = "{536D0E7A-03BC-425C-9DFA-A03AEA554EBE}";
    inline constexpr const char* RadianceCascadeModuleTypeId = "{6FDB4B01-3E86-4B6C-9917-82497321674F}";
    // The Editor Module by default is mutually exclusive with the Client Module
    // so they use the Same TypeId
    inline constexpr const char* RadianceCascadeEditorModuleTypeId = RadianceCascadeModuleTypeId;

    // Interface TypeIds
    inline constexpr const char* RadianceCascadeRequestsTypeId = "{4F1B4642-FF79-4B70-B365-A97B5D0DA985}";
} // namespace RadianceCascade
