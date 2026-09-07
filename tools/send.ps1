# send.ps1 — foreground the beetle-adventure-racing-recomp game window (by title) and send keystrokes and/or a mouse-wheel
# scroll, then capture a screenshot. For UI input testing.
# Usage: powershell -ExecutionPolicy Bypass -File tools/send.ps1 -Keys "{DOWN}{DOWN}{ENTER}" -Out k.png
#        powershell -ExecutionPolicy Bypass -File tools/send.ps1 -Wheel -3 -Fx 0.5 -Fy 0.5 -Out s.png
param([string]$Keys = "", [string]$Vks = "", [int]$Wheel = 0, [double]$Fx = 0.5, [double]$Fy = 0.5, [string]$Out = "send.png")
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System; using System.Text; using System.Runtime.InteropServices;
public class S {
  public delegate bool EnumProc(IntPtr h, IntPtr p);
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll")] public static extern int GetWindowText(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RC r);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool BringWindowToTop(IntPtr h);
  [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int n);
  [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, IntPtr pid);
  [DllImport("kernel32.dll")] public static extern uint GetCurrentThreadId();
  [DllImport("user32.dll")] public static extern bool AttachThreadInput(uint a, uint b, bool f);
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
  [DllImport("user32.dll")] public static extern void mouse_event(uint f, uint dx, uint dy, uint d, IntPtr e);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte scan, uint flags, IntPtr extra);
  [StructLayout(LayoutKind.Sequential)] public struct RC { public int L, T, R, B; }
  public static IntPtr Found = IntPtr.Zero;
  public static bool Cb(IntPtr h, IntPtr p) {
    if (!IsWindowVisible(h)) return true;
    var sb = new StringBuilder(256); GetWindowText(h, sb, 256);
    if (sb.ToString().Contains("Beetle Adventure Racing")) { Found = h; return false; }
    return true;
  }
}
"@
[S]::Found = [IntPtr]::Zero
[S]::EnumWindows([S+EnumProc]{ param($h,$p) [S]::Cb($h,$p) }, [IntPtr]::Zero) | Out-Null
$h = [S]::Found
if ($h -eq [IntPtr]::Zero) { Write-Output "NO_GAME_WINDOW"; exit 1 }
$fg = [S]::GetForegroundWindow(); $tfg = [S]::GetWindowThreadProcessId($fg, [IntPtr]::Zero); $tme = [S]::GetCurrentThreadId()
[S]::AttachThreadInput($tme, $tfg, $true) | Out-Null
[S]::ShowWindow($h, 5) | Out-Null; [S]::BringWindowToTop($h) | Out-Null; [S]::SetForegroundWindow($h) | Out-Null
[S]::AttachThreadInput($tme, $tfg, $false) | Out-Null
Start-Sleep -Milliseconds 500
$r = New-Object 'S+RC'; [S]::GetWindowRect($h, [ref]$r) | Out-Null
$w = $r.R - $r.L; $ht = $r.B - $r.T
if ($Wheel -ne 0) {
  [S]::SetCursorPos([int]($r.L + $w*$Fx), [int]($r.T + $ht*$Fy)) | Out-Null; Start-Sleep -Milliseconds 150
  [S]::mouse_event(0x0800, 0, 0, [uint32]($Wheel * 120), [IntPtr]::Zero)   # MOUSEEVENTF_WHEEL
  Write-Output ("WHEEL $Wheel at $($Fx),$($Fy)")
}
if ($Keys -ne "") {
  $wsh = New-Object -ComObject WScript.Shell
  $wsh.SendKeys($Keys)
  Write-Output ("KEYS '$Keys'")
}
if ($Vks -ne "") {
  foreach ($code in $Vks.Split(",")) {
    $vk = [byte][int]$code
    [S]::keybd_event($vk, 0, 0, [IntPtr]::Zero); Start-Sleep -Milliseconds 60
    [S]::keybd_event($vk, 0, 2, [IntPtr]::Zero); Start-Sleep -Milliseconds 120   # 2 = KEYUP
  }
  Write-Output ("VKS '$Vks'")
}
Start-Sleep -Milliseconds 500
[S]::GetWindowRect($h, [ref]$r) | Out-Null
$bmp = New-Object System.Drawing.Bitmap ($r.R-$r.L), ($r.B-$r.T)
$g = [System.Drawing.Graphics]::FromImage($bmp); $g.CopyFromScreen($r.L, $r.T, 0, 0, $bmp.Size)
$bmp.Save($Out, [System.Drawing.Imaging.ImageFormat]::Png); $g.Dispose(); $bmp.Dispose()
Write-Output ("SAVED $Out")
