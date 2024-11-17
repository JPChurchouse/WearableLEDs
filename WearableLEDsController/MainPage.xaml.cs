using System;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Maui.Controls;
using Plugin.BLE;
using Plugin.BLE.Abstractions.Contracts;
using Plugin.BLE.Abstractions.Exceptions;

namespace WearableLEDsController;

public partial class MainPage : ContentPage
{
  private readonly IAdapter _adapter;
  private readonly IBluetoothLE _bluetoothLE;
  private IDevice _connectedDevice;

  // Replace these with your BLE Service and Characteristic UUIDs
  private readonly Guid _serviceUuid = Guid.Parse("19B10000-E8F2-537E-4F6C-D104768A1214");
  private readonly Guid _characteristicUuid = Guid.Parse("19B10001-E8F2-537E-4F6C-D104768A1214");

  public MainPage()
  {
    InitializeComponent();
    _bluetoothLE = CrossBluetoothLE.Current;
    _adapter = CrossBluetoothLE.Current.Adapter;
  }

  private async void StartScanClicked(object sender, EventArgs e)
  {
    DeviceListView.ItemsSource = null;

    if (_bluetoothLE.State != BluetoothState.On)
    {
      await DisplayAlert("Error", "Bluetooth is not enabled.", "OK");
      return;
    }

    _adapter.DeviceDiscovered += (s, a) =>
    {
      DeviceListView.ItemsSource = _adapter.DiscoveredDevices.ToList();
    };

    try
    {
      await _adapter.StartScanningForDevicesAsync();
    }
    catch (Exception ex)
    {
      await DisplayAlert("Error", $"Error while scanning: {ex.Message}", "OK");
    }
  }

  private async void DeviceSelected(object sender, SelectedItemChangedEventArgs e)
  {
    var selectedDevice = e.SelectedItem as IDevice;
    if (selectedDevice == null) return;

    try
    {
      await _adapter.ConnectToDeviceAsync(selectedDevice);
      _connectedDevice = selectedDevice;
      await DisplayAlert("Connected", $"Connected to {selectedDevice.Name}", "OK");
    }
    catch (DeviceConnectionException ex)
    {
      await DisplayAlert("Error", $"Could not connect to device: {ex.Message}", "OK");
    }
  }

  private async void WriteToCharacteristicClicked(object sender, EventArgs e)
  {
    if (_connectedDevice == null)
    {
      await DisplayAlert("Error", "No device connected.", "OK");
      return;
    }

    var service = await _connectedDevice.GetServiceAsync(_serviceUuid);
    if (service == null)
    {
      await DisplayAlert("Error", "Service not found.", "OK");
      return;
    }

    var characteristic = await service.GetCharacteristicAsync(_characteristicUuid);
    if (characteristic == null)
    {
      await DisplayAlert("Error", "Characteristic not found.", "OK");
      return;
    }

    var valueToWrite = new byte[] { (byte)(int.Parse(CharacteristicValueEntry.Text) > 0 ?0x01:0x00) };
    try
    {
      await characteristic.WriteAsync(valueToWrite);
      await DisplayAlert("Success", "Value written to characteristic.", "OK");
    }
    catch (Exception ex)
    {
      await DisplayAlert("Error", $"Failed to write: {ex.Message}", "OK");
    }
  }
}
