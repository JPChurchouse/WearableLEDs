using Plugin.BLE;
using Plugin.BLE.Abstractions.Contracts;
using Plugin.BLE.Abstractions.EventArgs;
using Plugin.BLE.Abstractions.Exceptions;

namespace Controller;

public partial class MainPage : ContentPage
{
  private readonly IBluetoothLE _bluetoothLE;
  private readonly IAdapter _adapter;
  private IDevice _connectedDevice;
  private IService _requiredService;

  private static readonly Guid ServiceUuid = Guid.Parse("19B10000-E8F2-537E-4F6C-D104768A1214");
  private static readonly Guid ToggleCharacteristicUuid = Guid.Parse("19B10001-E8F2-537E-4F6C-D104768A1214");
  private static readonly Guid ComboBoxCharacteristicUuid = Guid.Parse("d02a2828-2b77-4685-89f0-2b9f0ba38da5");

  public List<string> Options { get; } = new() { "Option 1", "Option 2", "Option 3" };

  public MainPage()
  {
    InitializeComponent();
    _bluetoothLE = CrossBluetoothLE.Current;
    _adapter = _bluetoothLE.Adapter;

    BindingContext = this;
    StartScanningAsync();
  }

  private async Task StartScanningAsync()
  {
    try
    {
      if (_bluetoothLE.State != BluetoothState.On)
      {
        await DisplayAlert("Bluetooth Off", "Please enable Bluetooth to scan for devices.", "OK");
        StatusLabel.Text = "Bluetooth is off.";
        return;
      }

      StatusLabel.Text = "Scanning for devices...";
      _adapter.DeviceDiscovered += OnDeviceDiscovered;

      await _adapter.StartScanningForDevicesAsync();
      StatusLabel.Text = "Select a device to connect.";
    }
    catch (Exception ex)
    {
      await DisplayAlert("Error", $"Scanning failed: {ex.Message}", "OK");
    }
  }

  private void OnDeviceDiscovered(object sender, DeviceEventArgs args)
  {
    Dispatcher.Dispatch(() =>
    {
      DeviceList.ItemsSource ??= new List<IDevice>();
      var devices = (List<IDevice>)DeviceList.ItemsSource;

      if (!devices.Any(d => d.Id == args.Device.Id))
        devices.Add(args.Device);

      DeviceList.ItemsSource = null; // Refresh
      DeviceList.ItemsSource = devices;
    });
  }

  private async void DeviceList_ItemTapped(object sender, ItemTappedEventArgs e)
  {
    if (e.Item is not IDevice selectedDevice) return;

    StatusLabel.Text = $"Connecting to {selectedDevice.Name}...";
    await ConnectToDeviceAsync(selectedDevice);
  }

  private async Task ConnectToDeviceAsync(IDevice device)
  {
    try
    {
      await _adapter.StopScanningForDevicesAsync();
      await _adapter.ConnectToDeviceAsync(device);
      _connectedDevice = device;

      StatusLabel.Text = $"Connected to {device.Name}. Checking services...";
      _requiredService = await _connectedDevice.GetServiceAsync(ServiceUuid);

      if (_requiredService == null)
      {
        await DisplayAlert("Service Not Found", "Required service not available. Disconnecting.", "OK");
        await _adapter.DisconnectDeviceAsync(_connectedDevice);
        StatusLabel.Text = "Disconnected. Select another device.";
        _connectedDevice = null;
      }
      else
      {
        ToggleButton.IsEnabled = true;
        OptionsPicker.IsEnabled = true;
        WriteComboBoxButton.IsEnabled = true;
        StatusLabel.Text = $"Connected to {device.Name} with required service.";
      }
    }
    catch (Exception ex)
    {
      await DisplayAlert("Connection Error", $"Failed to connect: {ex.Message}", "OK");
    }
  }

  private async void ToggleButton_Clicked(object sender, EventArgs e)
  {
    try
    {
      var characteristic = await _requiredService.GetCharacteristicAsync(ToggleCharacteristicUuid);
      if (characteristic == null)
      {
        await DisplayAlert("Error", "Toggle characteristic not found.", "OK");
        return;
      }

      var valueToWrite = ToggleButton.Text == "Write Toggle (0x00)" ? new byte[] { 0x01 } : new byte[] { 0x00 };
      await characteristic.WriteAsync(valueToWrite);

      ToggleButton.Text = valueToWrite[0] == 0x01 ? "Write Toggle (0x01)" : "Write Toggle (0x00)";
    }
    catch (Exception ex)
    {
      await DisplayAlert("Write Error", $"Failed to write: {ex.Message}", "OK");
    }
  }

  private async void WriteComboBoxButton_Clicked(object sender, EventArgs e)
  {
    try
    {
      var selectedIndex = OptionsPicker.SelectedIndex;
      if (selectedIndex < 0)
      {
        await DisplayAlert("Error", "Please select an option before writing.", "OK");
        return;
      }

      var characteristic = await _requiredService.GetCharacteristicAsync(ComboBoxCharacteristicUuid);
      if (characteristic == null)
      {
        await DisplayAlert("Error", "Combo box characteristic not found.", "OK");
        return;
      }

      var valueToWrite = new byte[] { (byte)selectedIndex };
      await characteristic.WriteAsync(valueToWrite);

      await DisplayAlert("Success", $"Wrote value {selectedIndex} to combo box characteristic.", "OK");
    }
    catch (Exception ex)
    {
      await DisplayAlert("Write Error", $"Failed to write: {ex.Message}", "OK");
    }
  }
}

