using System.Collections.ObjectModel;
using Plugin.BLE;
using Plugin.BLE.Abstractions.Contracts;

namespace Controller;

public partial class MainPage : ContentPage
{
  private readonly IBluetoothLE BLE;
  private readonly IAdapter Adapter;
  private ObservableCollection<IDevice> FoundDevices;
  private IDevice Device;
  private IService TargetService;
  private ICharacteristic ToggleCharistic, ModeCharistic;
  private bool ToggleState;
  private readonly Guid
    TargetServiceUUID = Guid.Parse("19B10000-E8F2-537E-4F6C-D104768A1214"),
    ToggleCharacteristicUUID = Guid.Parse("19B10001-E8F2-537E-4F6C-D104768A1214"),
    OptionCharacteristicUUID = Guid.Parse("d02a2828-2b77-4685-89f0-2b9f0ba38da5");


  public MainPage()
  {
    InitializeComponent();

    BLE = CrossBluetoothLE.Current;
    Adapter = BLE.Adapter;
    FoundDevices = [];
    DeviceList.ItemsSource = FoundDevices;

    Adapter.DeviceDiscovered += (_, args) =>
    {
      string info = $"Discovered Device: {args.Device.Name} - {args.Device.Id}";
      StatusLabel.Text += "\r\n" + info;
      //Console.WriteLine(info);
      if (!FoundDevices.Any(d => d.Id == args.Device.Id))
      {
        FoundDevices.Add(args.Device);
      }
    };

    _ = ScanAsync();
  }


  private void ScanButton_Clicked(object sender, EventArgs e) => _ = ScanAsync();
  private async Task ScanAsync()
  {
    if (await ConnectKnownAsync()) return;
    try
    {
      if (BLE.State != BluetoothState.On)
      {
        await DisplayAlert("Bluetooth Off", "Bluetooth is off. Please enable it to scan for devices.", "OK");
        StatusLabel.Text = "Bluetooth is off. Enable it to scan.";
        return;
      }

      StatusLabel.Text = "Scanning for devices...";
      FoundDevices.Clear();

      await Adapter.StartScanningForDevicesAsync();

      StatusLabel.Text = "Scanning complete. Select a device to connect.";
    }
    catch (Exception ex)
    {
      await DisplayAlert("Scanning Error", ex.Message, "OK");
      StatusLabel.Text = $"Error: {ex.Message}";
    }
  }


  private async void DeviceList_ItemTapped(object sender, ItemTappedEventArgs e)
  {
    if (e.Item is IDevice sel)
    {
      StatusLabel.Text = $"Connecting to {sel.Name}...";
      await ConnectToDeviceAsync(sel);
    }
  }

  private async Task<bool> ConnectKnownAsync()
  {
    try
    {
     var device =  await Adapter.ConnectToKnownDeviceAsync(Guid.Parse("00000000-0000-0000-0000-c8f09ef25b96"));
      Device = device;
      TargetService = await device.GetServiceAsync(TargetServiceUUID);

      if (TargetService == null)
      {
        await DisplayAlert("Error", "Target service not found. Disconnecting.", "OK");
        await Adapter.DisconnectDeviceAsync(device);
        Device = null;
        return false;
      }

      StatusLabel.Text = "Connected";
      await ConnectedAsync();
        return true;
    }
    catch (Exception ex)
    {
      await DisplayAlert("Connection Error", ex.Message, "OK");
      StatusLabel.Text = $"Error: {ex.Message}";
      return false;
    }
  }

  private async Task ConnectToDeviceAsync(IDevice device)
  {
    try
    {
      await Adapter.ConnectToDeviceAsync(device);
      Device = device;
      TargetService = await device.GetServiceAsync(TargetServiceUUID);

      if (TargetService == null)
      {
        await DisplayAlert("Error", "Target service not found. Disconnecting.", "OK");
        await Adapter.DisconnectDeviceAsync(device);
        Device = null;
        return;
      }

      StatusLabel.Text = "Connected";
      await ConnectedAsync();
    }
    catch (Exception ex)
    {
      await DisplayAlert("Connection Error", ex.Message, "OK");
      StatusLabel.Text = $"Error: {ex.Message}";
    }
  }

  private async Task ConnectedAsync()
  {
    ToggleCharistic = await TargetService.GetCharacteristicAsync(ToggleCharacteristicUUID);
    ToggleButton.IsEnabled = ToggleCharistic != null;
    if (ToggleCharistic != null)
    {
      ToggleState = (await ToggleCharistic.ReadAsync()).data[0] > 0;

      if (ToggleCharistic.CanUpdate)
      {
        ToggleCharistic.ValueUpdated += (_, a) =>
        {
          ToggleState = a.Characteristic.Value[0] > 0;
          DisplayToggleValue();
        };
        _ = ToggleCharistic.StartUpdatesAsync();
      }
    }

    ModeCharistic = await TargetService.GetCharacteristicAsync(OptionCharacteristicUUID);
    OptionsPicker.IsEnabled = ModeCharistic != null;
    WriteButton.IsEnabled = ModeCharistic != null;
    if (ModeCharistic != null)
    {
      OptionsPicker.SelectedIndex = (await ModeCharistic.ReadAsync()).data[0];
    }
  }

  private void ToggleButton_Clicked(object sender, EventArgs e) => _ = SetToggleValue();

  private void DisplayToggleValue() => ToggleButton.Text = ToggleState ? "Toggle: ON" : "Toggle: OFF";
  private async Task SetToggleValue(bool? value = null)
  {
    try
    {
      ToggleState = value ?? !ToggleState;
      _ = await ToggleCharistic.WriteAsync([(byte)(ToggleState ? 0x01 : 0x00)]);
      StatusLabel.Text = "Toggle written successfully.";
    }
    catch (Exception ex)
    {
      await DisplayAlert("Write Error", ex.Message, "OK");
      StatusLabel.Text = $"Error: {ex.Message}";
    }
  }


  private async void WriteButton_Clicked(object sender, EventArgs e)
  {
    try
    {
      byte sel = (byte)OptionsPicker.SelectedIndex;
      if (sel < 0)
      {
        await DisplayAlert("Invalid Selection", "Please select an option first.", "OK");
        return;
      }

      _ = await ModeCharistic.WriteAsync([sel]);
      StatusLabel.Text = "Option written successfully.";
    }
    catch (Exception ex)
    {
      await DisplayAlert("Write Error", ex.Message, "OK");
      StatusLabel.Text = $"Error: {ex.Message}";
    }
  }
}
