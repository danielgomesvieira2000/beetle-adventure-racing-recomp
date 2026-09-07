# uitest.ps1 — robustly foreground the beetle-adventure-racing-recomp GAME window (found by title, not MainWindowHandle),
# synthesize a left click at a fractional position, then capture a screenshot. Targets the SDL game
# window specifically and uses the AttachThreadInput trick to beat the Win32 foreground lock.
# Usage: powershell -ExecutionPolicy Bypass -File tools/uitest.ps1 -Fx 0.5 -Fy 0.467 -Out shot.png
param([double]$Fx = 0.5, [double]$Fy = 0.467, [string]$Out = "uitest.png", [switch]$NoClick)

Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;
using System.Text;
using System.Runtime.InteropServices;
public class U {
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
  [StructLayout(LayoutKind.Sequential)] public struct RC { public int L, T, R, B; }
  public static IntPtr Found = IntPtr.Zero;
  public static bool Cb(IntPtr h, IntPtr p) {
    if (!IsWindowVisible(h)) return true;
    var sb = new StringBuilder(256); GetWindowText(h, sb, 256);
    var t = sb.ToString();
    if (t.Contains("Beetle Adventure Racing")) { Found = h; return false; }
    return true;
  }
}
"@
[U]::Found = [IntPtr]::Zero
[U]::EnumWindows([U+EnumProc]{ param($h,$p) [U]::Cb($h,$p) }, [IntPtr]::Zero) | Out-Null
$h = [U]::Found
if ($h -eq [IntPtr]::Zero) { Write-Output "NO_GAME_WINDOW"; exit 1 }

# Force foreground past the lock: attach to the current foreground thread's input queue.
$fg = [U]::GetForegroundWindow()
$tfg = [U]::GetWindowThreadProcessId($fg, [IntPtr]::Zero)
$tme = [U]::GetCurrentThreadId()
[U]::AttachThreadInput($tme, $tfg, $true) | Out-Null
[U]::ShowWindow($h, 5) | Out-Null
[U]::BringWindowToTop($h) | Out-Null
[U]::SetForegroundWindow($h) | Out-Null
[U]::AttachThreadInput($tme, $tfg, $false) | Out-Null
Start-Sleep -Milliseconds 500

$r = New-Object 'U+RC'; [U]::GetWindowRect($h, [ref]$r) | Out-Null
$w = $r.R - $r.L; $ht = $r.B - $r.T
if (-not $NoClick) {
  $x = [int]($r.L + $w * $Fx); $y = [int]($r.T + $ht * $Fy)
  [U]::SetCursorPos($x, $y) | Out-Null; Start-Sleep -Milliseconds 200
  [U]::mouse_event(0x0002, 0, 0, 0, [IntPtr]::Zero); Start-Sleep -Milliseconds 90
  [U]::mouse_event(0x0004, 0, 0, 0, [IntPtr]::Zero)
  Write-Output ("CLICKED $x,$y  win=${w}x${ht}@$($r.L),$($r.T)")
  Start-Sleep -Milliseconds 500
  [U]::GetWindowRect($h, [ref]$r) | Out-Null
}
$bmp = New-Object System.Drawing.Bitmap ($r.R-$r.L), ($r.B-$r.T)
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.CopyFromScreen($r.L, $r.T, 0, 0, $bmp.Size)
$bmp.Save($Out, [System.Drawing.Imaging.ImageFormat]::Png)
$g.Dispose(); $bmp.Dispose()
Write-Output ("SAVED $Out ($($r.R-$r.L) x $($r.B-$r.T))")
