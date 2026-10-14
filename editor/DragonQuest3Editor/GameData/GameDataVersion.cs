namespace DragonQuest3Editor.GameData
{
   public struct GameDataVersion( byte major, byte minor, byte maint )
   {
      public byte Major { get; set; } = major;
      public byte Minor { get; set; } = minor;
      public byte Maint { get; set; } = maint;
   }
}
