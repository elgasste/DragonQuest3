using System.IO;

namespace DragonQuest3Editor.GameData
{
   public class GameDataRoot
   {
      public GameDataVersion Version { get; private set; } = new(Constants.GameDataLatestVersionMajor, Constants.GameDataLatestVersionMinor, Constants.GameDataLatestVersionMaint);

      public GameDataRoot()
      {
      }

      // MUFFINS:
      //
      // this is all wrong, we shouldn't be reading the current game data file and trying to reconcile everything,
      // we should be saving all this data to JSON, then writing out a new game data file from the JSON when we want to.
      // which means this class should have:
      //
      // - a method to load from JSON
      // - a method to save to a game data file

      public void LoadFromFile( string filePath )
      {
         if ( !File.Exists( filePath ) )
         {
            throw new FileNotFoundException( $"Game data file not found: {filePath}" );
         }

         byte[] fileBytes;

         try
         {
            fileBytes = File.ReadAllBytes( filePath);
         }
         catch (Exception ex)
         {
            throw new IOException( $"Error reading game data file: {ex.Message}", ex );
         }

         ReadFileHeader( fileBytes );
      }

      private void ReadFileHeader( byte[] fileBytes )
      {
         if ( fileBytes.Length < 19 )
         {
            throw new InvalidDataException( "Game data file is too short to contain a valid header");
         }

         // first 4 bytes are the magic number
         if ( fileBytes[0] != Constants.GameDataMagicNumber[0] ||
              fileBytes[1] != Constants.GameDataMagicNumber[1] ||
              fileBytes[2] != Constants.GameDataMagicNumber[2] ||
              fileBytes[3] != Constants.GameDataMagicNumber[3] )
         {
            throw new InvalidDataException("Game data file header contains invalid magic number");
         }

         // next 3 bytes are the version
         byte major = fileBytes[4];
         byte minor = fileBytes[5];
         byte maint = fileBytes[6];

         // TODO: later on we might want to implement version migration, but for now we'll just look for an exact match
         if ( fileBytes[4] != Constants.GameDataLatestVersionMajor ||
              fileBytes[5] != Constants.GameDataLatestVersionMinor ||
              fileBytes[6] != Constants.GameDataLatestVersionMaint )
         {
            throw new InvalidDataException($"Game data file version {major}.{minor}.{maint} is not supported. Expected version {Constants.GameDataLatestVersionMajor}.{Constants.GameDataLatestVersionMinor}.{Constants.GameDataLatestVersionMaint}.");
         }

         // last 12 bytes are the chunk offsets (4 bytes each, but we only care about reading map data)
         var mapDataFileOffset = BitConverter.ToInt32( fileBytes, 15 );

         if ( mapDataFileOffset < 0 || mapDataFileOffset >= fileBytes.Length )
         {
            throw new InvalidDataException( "Game data file header contains chunk offsets that are out of bounds" );
         }

         ReadMapDataChunk( fileBytes, mapDataFileOffset );
      }

      private void ReadMapDataChunk( byte[] fileBytes, int offset )
      {
         // TODO
      }
   }
}