using DragonQuest3Editor.ViewModels;
using System.Windows;

namespace DragonQuest3Editor
{
   public partial class MainWindow : Window
   {
      private readonly MainWindowViewModel _viewModel;

      public MainWindow()
      {
         InitializeComponent();
         _viewModel = new MainWindowViewModel();
         DataContext = _viewModel;
      }
   }
}