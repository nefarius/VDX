using System;
using System.Linq;
using Nuke.Common;
using Nuke.Common.Git;
using Nuke.Common.ProjectModel;
using Nuke.Common.Tools.MSBuild;
using RunProcessAsTask;
using static Nuke.Common.EnvironmentInfo;
using static Nuke.Common.IO.FileSystemTasks;
using static Nuke.Common.IO.PathConstruction;
using static Nuke.Common.Tools.MSBuild.MSBuildTasks;

class Build : NukeBuild
{
    public static int Main () => Execute<Build>(x => x.Compile);

    [Solution] readonly Solution Solution;
    [GitRepository] readonly GitRepository GitRepository;

    AbsolutePath SourceDirectory => RootDirectory / "src";
    AbsolutePath ArtifactsDirectory => RootDirectory / "artifacts";

    Target Clean => _ => _
        .Executes(() =>
        {
            DeleteDirectories(GlobDirectories(SourceDirectory, "**/bin", "**/obj"));
            EnsureCleanDirectory(ArtifactsDirectory);
        });

    Target Restore => _ => _
        .DependsOn(Clean)
        .Executes(() =>
        {
            MSBuild(s => s
                .SetTargetPath(SolutionFile)
                .SetTargets("Restore"));
        });

    Target Compile => _ => _
        .DependsOn(Restore)
        .Executes(() =>
        {
            //
            // Build x64 dependencies with Vcpkg
            // 
            Logger.Info("Building SFML (x64) and dependencies, this will take about 10 to 15 minutes...");
            var x64ret = ProcessEx.RunAsync("vcpkg.exe", "install sfml:x64-windows-static").Result;
            foreach (var output in x64ret.StandardOutput)
            {
                Logger.Info(output);
            }

            //
            // Build x86 dependencies with Vcpkg
            // 
            Logger.Info("Building SFML (x86) and dependencies, this will take about 10 to 15 minutes...");
            var x86ret = ProcessEx.RunAsync("vcpkg.exe", "install sfml:x86-windows-static").Result;
            foreach (var output in x86ret.StandardOutput)
            {
                Logger.Info(output);
            }

            //
            // Build x64
            // 
            MSBuild(s => s
                .SetTargetPath(SolutionFile)
                .SetTargets("Rebuild")
                .SetConfiguration(Configuration)
                .SetMaxCpuCount(Environment.ProcessorCount)
                .SetNodeReuse(IsLocalBuild)
                .SetTargetPlatform(MSBuildTargetPlatform.x64));

            //
            // Build x86
            // 
            MSBuild(s => s
                .SetTargetPath(SolutionFile)
                .SetTargets("Rebuild")
                .SetConfiguration(Configuration)
                .SetMaxCpuCount(Environment.ProcessorCount)
                .SetNodeReuse(IsLocalBuild)
                .SetTargetPlatform(MSBuildTargetPlatform.x86));
        });

    private Target Pack => _ => _
        .DependsOn(Compile)
        .Executes(() =>
        {
            MSBuild(s => s
                .SetTargetPath(SolutionFile)
                .SetTargets("Restore", "Pack")
                .SetPackageOutputPath(ArtifactsDirectory)
                .SetConfiguration(Configuration)
                .EnableIncludeSymbols());
        });
}
