using DragonQuest3Editor.GameData;
using System.Windows;

namespace DragonQuest3Editor.ViewModels
{
   public class MainWindowViewModel : ViewModelBase
   {
      private readonly GameDataRoot _gameData;

      public MainWindowViewModel()
      {
         _gameData = new();

         try
         {
            _gameData.LoadFromFile( Constants.GameDataFilePath );
         }
         catch ( Exception e )
         {
            var msg = $"Error loading game data: {e.Message}\n\nWould you like to continue with an empty game data file?";
            var result = MessageBox.Show( msg, "Error", MessageBoxButton.YesNo, MessageBoxImage.Error );

            if( result == MessageBoxResult.No )
            {
               Application.Current.Shutdown();
            }
         }
      }
   }
}
