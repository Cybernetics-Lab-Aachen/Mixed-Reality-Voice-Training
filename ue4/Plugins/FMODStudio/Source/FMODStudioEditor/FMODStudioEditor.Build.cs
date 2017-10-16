// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2017.

namespace UnrealBuildTool.Rules
{
	public class FMODStudioEditor : ModuleRules
	{
		public FMODStudioEditor(TargetInfo Target)
		{
			bFasterWithoutUnity = true;

			PublicIncludePaths.AddRange(
				new string[] {
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"FMODStudioEditor/Private",
					"FMODStudio/Private",
                    "Editor/MovieSceneTools/Private",
                    "Editor/MovieSceneTools/Private/CurveKeyEditors",
                    "Editor/MovieSceneTools/Private/TrackEditors",
                    "Editor/MovieSceneTools/Private/TrackEditors/PropertyTrackEditors",
                    "Editor/MovieSceneTools/Private/TrackEditorThumbnail",
                    "Editor/MovieSceneTools/Private/Sections"
				}
                );

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"FMODStudio",
                    "InputCore",
                    "UnrealEd",
                    "Sequencer"
                }
                );

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Slate",
					"SlateCore",
					"Settings",
					"EditorStyle",
					"LevelEditor",
					"AssetTools",
					"AssetRegistry",
					"PropertyEditor",
					"WorkspaceMenuStructure",
					"Sockets",
                    "LevelSequence",
                    "MovieScene",
                    "MovieSceneTracks",
                    "MovieSceneTools"
                }
                );

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
                }
                );
		}
	}
}