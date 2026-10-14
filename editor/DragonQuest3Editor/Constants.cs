namespace DragonQuest3Editor
{
   public static class Constants
   {
      public const string RepoBaseFolder = "..\\..\\..\\..\\..\\";

      public const string GameDataMagicNumber = "DW3D";
      public const string GameDataFileExtension = ".dw3d";
      public const string GameDataFileName = "data" + GameDataFileExtension;
      public const string GameAssetsFolder = RepoBaseFolder + "assets\\";
      public const string GameDataFilePath = GameAssetsFolder + GameDataFileName;

      public const byte GameDataLatestVersionMajor = 0;
      public const byte GameDataLatestVersionMinor = 0;
      public const byte GameDataLatestVersionMaint = 1;
   }
}
