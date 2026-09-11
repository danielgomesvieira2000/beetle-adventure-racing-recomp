#!/bin/bash
# Foreground wrapper for the night run, meant to be launched from WINDOWS as a hidden,
# detached wsl.exe process (Start-Process — see .grind/start_night.ps1). The persistent
# wsl.exe client keeps the WSL distro alive for the whole run; setsid/nohup alone does NOT
# survive the launching client exiting (the distro tears down and kills everything).
if pgrep -f 'night_run.sh|permuter.py|m2c_seed_all.py' > /dev/null; then
  echo "ALREADY-RUNNING" >> /mnt/c/Users/Bryan/Projects/BeetleRecomp/.grind/night_run.log
  exit 1
fi
exec nice -n 10 bash /home/brysl/projects/bar-decomp/tools/decomp_helpers/grind/night_run.sh \
  >> /mnt/c/Users/Bryan/Projects/BeetleRecomp/.grind/night_run.log 2>&1
