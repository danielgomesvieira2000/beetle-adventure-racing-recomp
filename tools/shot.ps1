# shot.ps1 — capture ONLY the beetle-adventure-racing-recomp game window. Foregrounds it by title, verifies it is actually
# the foreground window, and captures just its rect. If it can't be brought to front, it ABORTS without
# capturing (so it can never accidentally grab another app's window). Usage:
#   powershell -ExecutionPolicy Bypass -File tools/shot.ps1 -Out C:\path\shot.png
param([string]$Out = "shot.png")
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System; using System.Text; using System.Runtime.InteropServices;
public class W {
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
[W]::Found = [IntPtr]::Zero
[W]::EnumWindows([W+EnumProc]{ param($h,$p) [W]::Cb($h,$p) }, [IntPtr]::Zero) | Out-Null
$h = [W]::Found
if ($h -eq [IntPtr]::Zero) { Write-Output "NO_GAME_WINDOW"; exit 1 }
$fg = [W]::GetForegroundWindow(); $tfg = [W]::GetWindowThreadProcessId($fg, [IntPtr]::Zero); $tme = [W]::GetCurrentThreadId()
[W]::AttachThreadInput($tme, $tfg, $true) | Out-Null
[W]::ShowWindow($h, 5) | Out-Null; [W]::BringWindowToTop($h) | Out-Null; [W]::SetForegroundWindow($h) | Out-Null
[W]::AttachThreadInput($tme, $tfg, $false) | Out-Null
Start-Sleep -Milliseconds 350
# SAFETY GATE: only capture if the game really is foreground now.
if ([W]::GetForegroundWindow() -ne $h) { Write-Output "GAME_NOT_FOREGROUND_ABORT (nothing captured)"; exit 2 }
$r = New-Object W+RC; [W]::GetWindowRect($h, [ref]$r) | Out-Null
$w = $r.R - $r.L; $ht = $r.B - $r.T
if ($w -le 0 -or $ht -le 0) { Write-Output "BAD_RECT"; exit 3 }
$bmp = New-Object System.Drawing.Bitmap $w, $ht
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.CopyFromScreen($r.L, $r.T, 0, 0, (New-Object System.Drawing.Size($w, $ht)))
$bmp.Save($Out)
$g.Dispose(); $bmp.Dispose()
Write-Output ("OK ${w}x${ht} -> $Out")
