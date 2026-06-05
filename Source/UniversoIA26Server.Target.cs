using UnrealBuildTool;
using System.Collections.Generic;

public class UniversoIA26ServerTarget : TargetRules
{
    public UniversoIA26ServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.AddRange(new string[] { "UniversoIA26" });
    }
}