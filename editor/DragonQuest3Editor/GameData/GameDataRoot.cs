using System.IO;

namespace DragonQuest3Editor.GameData
{
   public class GameDataRoot
   {
      public GameDataVersion Version { get; set; } = new( Constants.GameDataLatestVersionMajor, Constants.GameDataLatestVersionMinor, Constants.GameDataLatestVersionMaint );

      public GameDataRoot()
      {
      }

      public void LoadFromFile( string filePath )
      {
         if( !File.Exists( filePath ) )
         {
            throw new FileNotFoundException( $"Game data file not found: {filePath}" );
         }
      }
   }
}
