using Android;
using Android.App;
using Android.Content.PM;
using Android.OS;
using AndroidX.Core.App;
using AndroidX.Core.Content;
using Microsoft.Maui;

namespace Controller;

[Activity(
    Label = "LEDs Controller",
    MainLauncher = true,
    Theme = "@style/Maui.SplashTheme",
    ConfigurationChanges = ConfigChanges.ScreenSize | ConfigChanges.Orientation | ConfigChanges.UiMode | ConfigChanges.ScreenLayout | ConfigChanges.SmallestScreenSize | ConfigChanges.Density
)]
public class MainActivity : MauiAppCompatActivity
{
  private const int RequestCodePermissions = 100;

  protected override void OnCreate(Bundle savedInstanceState)
  {
    base.OnCreate(savedInstanceState);
    RequestPermissions();
  }

  private void RequestPermissions()
  {
    string[] requiredPermissions = 
    [
      Manifest.Permission.Bluetooth,
      Manifest.Permission.BluetoothAdmin,
      Manifest.Permission.BluetoothScan,
      Manifest.Permission.BluetoothConnect,
      Manifest.Permission.AccessFineLocation,
      Manifest.Permission.NearbyWifiDevices,
    ];

    var missingPermissions = requiredPermissions.Where(permission =>
        ContextCompat.CheckSelfPermission(this, permission) != Permission.Granted).ToArray();

    if (missingPermissions.Any())
    {
      ActivityCompat.RequestPermissions(this, missingPermissions, RequestCodePermissions);
    }
  }

  public override void OnRequestPermissionsResult(int requestCode, string[] permissions, Permission[] grantResults)
  {
    base.OnRequestPermissionsResult(requestCode, permissions, grantResults);

    if (requestCode == RequestCodePermissions)
    {
      for (int i = 0; i < permissions.Length; i++)
      {
        if (grantResults[i] != Permission.Granted)
        {
          Android.Widget.Toast.MakeText(this, $"Permission {permissions[i]} denied. BLE might not work.", Android.Widget.ToastLength.Long).Show();
        }
      }
    }
  }
}
